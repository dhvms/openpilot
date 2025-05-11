import numpy as np
from opendbc.can.packer import CANPacker
from opendbc.car import (Bus, DT_CTRL, apply_driver_steer_torque_limits, apply_std_steer_angle_limits, common_fault_avoidance, make_tester_present_msg, structs)
from opendbc.car.common.conversions import Conversions as CV
from opendbc.car.hyundai import hyundaicanfd, hyundaican
from opendbc.car.hyundai.carstate import CarState
from opendbc.car.hyundai.hyundaicanfd import CanBus
from opendbc.car.hyundai.values import HyundaiFlags, Buttons, CarControllerParams, CAR, CAN_GEARS
from opendbc.car.interfaces import CarControllerBase, ACCEL_MIN, ACCEL_MAX

from openpilot.selfdrive.controls.neokii.navi_controller import SpeedLimiter
from openpilot.common.params import Params

VisualAlert = structs.CarControl.HUDControl.VisualAlert
LongCtrlState = structs.CarControl.Actuators.LongControlState

# EPS faults if you apply torque while the steering angle is above 90 degrees for more than 1 second
# All slightly below EPS thresholds to avoid fault
MAX_FAULT_ANGLE = 85
MAX_FAULT_ANGLE_FRAMES = 89
MAX_FAULT_ANGLE_CONSECUTIVE_FRAMES = 2


def process_hud_alert(enabled, fingerprint, hud_control):
  sys_warning = (hud_control.visualAlert in (VisualAlert.steerRequired, VisualAlert.ldw))

  # initialize to no line visible
  # TODO: this is not accurate for all cars
  sys_state = 1
  if hud_control.leftLaneVisible and hud_control.rightLaneVisible or sys_warning:  # HUD alert only display when LKAS status is active
    sys_state = 3 if enabled or sys_warning else 4
  elif hud_control.leftLaneVisible:
    sys_state = 5
  elif hud_control.rightLaneVisible:
    sys_state = 6

  # initialize to no warnings
  left_lane_warning = 0
  right_lane_warning = 0
  if hud_control.leftLaneDepart:
    left_lane_warning = 1 if fingerprint in (CAR.GENESIS_G90, CAR.GENESIS_G80) else 2
  if hud_control.rightLaneDepart:
    right_lane_warning = 1 if fingerprint in (CAR.GENESIS_G90, CAR.GENESIS_G80) else 2

  return sys_warning, sys_state, left_lane_warning, right_lane_warning


class CarController(CarControllerBase):
  def __init__(self, dbc_names, CP):
    super().__init__(dbc_names, CP)
    self.CAN = CanBus(CP)
    self.params = CarControllerParams(CP)
    self.packer = CANPacker(dbc_names[Bus.pt])
    self.car_fingerprint = CP.carFingerprint

    self.accel_last = 0
    self.apply_torque_last = 0
    self.apply_angle_last = 0
    self.lkas_max_torque = 0
    self.last_button_frame = 0
    self.angle_limit_counter = 0
    self.turningSignalTimer = 0

    self.MainMode_ACC_trigger = 0
    self.LFA_trigger = 0

  def update(self, CC, CS, now_nanos):
    actuators = CC.actuators
    hud_control = CC.hudControl
    apply_torque = 0

    # TODO: needed for angle control cars?
    # >90 degree steering fault prevention
    self.angle_limit_counter, apply_steer_req = common_fault_avoidance(abs(CS.out.steeringAngleDeg) >= MAX_FAULT_ANGLE, CC.latActive,
                                                                       self.angle_limit_counter, MAX_FAULT_ANGLE_FRAMES,
                                                                       MAX_FAULT_ANGLE_CONSECUTIVE_FRAMES)

    # steering torque
    if not self.CP.flags & HyundaiFlags.CANFD_ANGLE_STEERING:
      new_torque = int(round(actuators.torque * self.params.STEER_MAX))
      apply_torque = apply_driver_steer_torque_limits(new_torque, self.apply_torque_last, CS.out.steeringTorque, self.params)

    # angle control
    else:
      new_angle = actuators.steeringAngleDeg
      adjusted_alpha = float(np.interp(CS.out.vEgoRaw,
                                       self.params.ANGLE_PARAMS['SMOOTHING_ANGLE_VEGO_MATRIX'],
                                       self.params.ANGLE_PARAMS['SMOOTHING_ANGLE_ALPHA_MATRIX']))
      new_angle = (new_angle * adjusted_alpha + (1 - adjusted_alpha) * self.apply_angle_last)

      # Reset apply_angle_last if the driver is intervening
      if CS.out.steeringPressed:
        self.apply_angle_last = actuators.steeringAngleDeg

      self.apply_angle_last = apply_std_steer_angle_limits(new_angle, self.apply_angle_last, CS.out.vEgoRaw,
                                                           CS.out.steeringAngleDeg, CC.latActive, self.params.ANGLE_LIMITS)

      current_torque = abs(CS.out.steeringTorque)
      curvature = abs(actuators.curvature)
      angle_from_center = abs(self.apply_angle_last)
      min_torque = self.params.ANGLE_MIN_TORQUE
      max_torque = self.params.ANGLE_MAX_TORQUE

      speed_multiplier = np.interp(CS.out.vEgoRaw, [0, 15, 30.0], [1.0, 1.2, 1.4])
      scaled_torque = [min(factor * max_torque * speed_multiplier, max_torque)
                       for factor in self.params.ANGLE_PARAMS['TORQUE_FACTOR']]

      dynamic_up_rate = float(np.interp(CS.out.vEgoRaw, [0, 15, 30], [2.0, 1.5, 1.0]))
      dynamic_down_rate = float(np.interp(CS.out.vEgoRaw, [0, 15, 30], [4.0, 3.5, 3.0]))

      # Override handling
      if current_torque > max_torque:
        torque_diff = current_torque - max_torque
        available_reduction = self.lkas_max_torque - min_torque
        reduction_factor = np.max([dynamic_down_rate,
                                   torque_diff / self.params.ANGLE_PARAMS['TORQUE_DIFF_SCALE'],
                                   available_reduction / self.params.ANGLE_PARAMS['OVERRIDE_CYCLES']])
        self.lkas_max_torque = max(self.lkas_max_torque - reduction_factor, min_torque)

      # Normal torque adjustment
      else:
        # Curvature-based target calculation
        target_torque = float(np.interp(curvature,
                                        self.params.ANGLE_PARAMS['CURVATURE_BP'], scaled_torque))

        # Near-center adjustment
        if angle_from_center < self.params.ANGLE_PARAMS['NEAR_CENTER_THRESHOLD']:
          max_torque_scale = float(np.interp(angle_from_center,
                                             [0, self.params.ANGLE_PARAMS['NEAR_CENTER_THRESHOLD']],
                                             self.params.ANGLE_PARAMS['MAX_TORQUE_RANGE']))
          target_torque = min(target_torque, max_torque * max_torque_scale)

        # Torque ramping logic
        if self.lkas_max_torque > target_torque:
          torque_diff = self.lkas_max_torque - target_torque
          reduction_factor = np.max([dynamic_down_rate,
                                     torque_diff / self.params.ANGLE_PARAMS['TORQUE_DIFF_SCALE']])
          self.lkas_max_torque = max(self.lkas_max_torque - reduction_factor, target_torque)
        else:
          torque_diff = target_torque - self.lkas_max_torque
          increase_factor = np.min([dynamic_up_rate,
                                    torque_diff / self.params.ANGLE_PARAMS['TORQUE_DIFF_SCALE']])
          self.lkas_max_torque = min(self.lkas_max_torque + increase_factor, target_torque)

    # Disable steering while turning blinker on and speed below 60 kph
    if CS.out.leftBlinker or CS.out.rightBlinker:
      self.turningSignalTimer = 0.5 / DT_CTRL  # Disable for 0.5 Seconds after blinker turned off
    if self.turningSignalTimer > 0:
      self.turningSignalTimer -= 1

    if not CC.latActive:
      apply_torque = 0
      self.lkas_max_torque = 0

    # Hold torque with induced temporary fault when cutting the actuation bit
    # FIXME: we don't use this with CAN FD?
    torque_fault = CC.latActive and not apply_steer_req

    self.apply_torque_last = apply_torque

    # accel + longitudinal
    accel = float(np.clip(actuators.accel, ACCEL_MIN, ACCEL_MAX))
    stopping = actuators.longControlState == LongCtrlState.stopping
    set_speed_in_units = hud_control.setSpeed * (CV.MS_TO_KPH if CS.is_metric else CV.MS_TO_MPH)

    can_sends = []

    # *** common hyundai stuff ***

    # tester present - w/ no response (keeps relevant ECU disabled)
    if self.frame % 100 == 0 and not (self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC) and self.CP.openpilotLongitudinalControl:
      # for longitudinal control, either radar or ADAS driving ECU
      addr, bus = 0x7d0, self.CAN.ECAN if self.CP.flags & HyundaiFlags.CANFD else 0
      if self.CP.flags & HyundaiFlags.CANFD_LKA_STEERING.value:
        addr, bus = 0x730, self.CAN.ECAN
      can_sends.append(make_tester_present_msg(addr, bus, suppress_response=True))

      # for blinkers
      if self.CP.flags & HyundaiFlags.ENABLE_BLINKERS:
        can_sends.append(make_tester_present_msg(0x7b1, self.CAN.ECAN, suppress_response=True))

    # *** CAN/CAN FD specific ***
    if self.CP.flags & HyundaiFlags.CANFD:
      can_sends.extend(self.create_canfd_msgs(apply_steer_req, apply_torque, set_speed_in_units, accel,
                                              stopping, hud_control, actuators, CS, CC))
    else:
      can_sends.extend(self.create_can_msgs(apply_steer_req, apply_torque, torque_fault, set_speed_in_units, accel,
                                            stopping, hud_control, actuators, CS, CC))

    new_actuators = actuators.as_builder()
    new_actuators.torque = apply_torque / self.params.STEER_MAX
    new_actuators.torqueOutputCan = apply_torque
    new_actuators.steeringAngleDeg = self.apply_angle_last
    new_actuators.accel = accel

    self.frame += 1
    return new_actuators, can_sends

  def create_can_msgs(self, apply_steer_req, apply_torque, torque_fault, set_speed_in_units, accel, stopping, hud_control, actuators, CS, CC):
    can_sends = []

    send_lfa = self.CP.flags & HyundaiFlags.SEND_LFA.value
    use_fca = self.CP.flags & HyundaiFlags.USE_FCA.value
    camera_scc = self.CP.flags & HyundaiFlags.CAMERA_SCC

    # HUD messages
    sys_warning, sys_state, left_lane_warning, right_lane_warning = process_hud_alert(CC.enabled, self.car_fingerprint,
                                                                                      hud_control)

    can_sends.append(hyundaican.create_lkas11(self.packer, self.frame, self.CP, apply_torque, apply_steer_req, torque_fault, sys_warning,
                                              sys_state, CC.enabled, hud_control.leftLaneVisible, hud_control.rightLaneVisible,
                                              left_lane_warning, right_lane_warning, CS.lkas11))

    # Button messages
    if not self.CP.openpilotLongitudinalControl:
      if CC.cruiseControl.cancel:
        can_sends.append(hyundaican.create_clu11(self.packer, self.frame, self.CP, Buttons.CANCEL, CS.clu11))
      elif CC.cruiseControl.resume:
        # send resume at a max freq of 10Hz
        if (self.frame - self.last_button_frame) * DT_CTRL > 0.1:
          # send 25 messages at a time to increases the likelihood of resume being accepted
          can_sends.extend([hyundaican.create_clu11(self.packer, self.frame, self.CP, Buttons.RES_ACCEL, CS.clu11)] * 25)
          if (self.frame - self.last_button_frame) * DT_CTRL >= 0.15:
            self.last_button_frame = self.frame

    if self.frame % 2 == 0 and self.CP.openpilotLongitudinalControl:
      # TODO: unclear if this is needed
      jerk = 3.0 if actuators.longControlState == LongCtrlState.pid else 1.0
      can_sends.extend(hyundaican.create_scc_commands(self.packer, accel, jerk, int(self.frame / 2),
                                                      hud_control, set_speed_in_units, stopping, CC, CS, use_fca))

    # 20 Hz LFA MFA message
    if self.frame % 5 == 0 and send_lfa:
      can_sends.append(hyundaican.create_lfahda_mfc(self.packer, CC.enabled, SpeedLimiter.instance().get_active()))

    # 5 Hz ACC options
    if self.frame % 20 == 0 and self.CP.openpilotLongitudinalControl and not camera_scc:
      can_sends.extend(hyundaican.create_acc_opt(self.packer, use_fca))
    elif CS.scc13 is not None:
      can_sends.append(hyundaican.create_acc_opt_none(self.packer, CS))

    if self.CP.carFingerprint in CAN_GEARS["send_mdps12"]:  # send mdps12 to LKAS to prevent LKAS error
      can_sends.append(hyundaican.create_mdps12(self.packer, self.frame, CS.mdps12))

    # 2 Hz front radar options
    if self.frame % 50 == 0 and self.CP.openpilotLongitudinalControl and not camera_scc:
      can_sends.append(hyundaican.create_frt_radar_opt(self.packer))

    return can_sends

  def create_canfd_msgs(self, apply_steer_req, apply_torque, set_speed_in_units, accel, stopping, hud_control, actuators, CS, CC):
    can_sends = []

    lka_steering = self.CP.flags & HyundaiFlags.CANFD_LKA_STEERING or Params().get_bool("IsHda2")
    lka_steering_long = lka_steering and self.CP.openpilotLongitudinalControl
    camera_scc = self.CP.flags & HyundaiFlags.CAMERA_SCC

    # steering control
    can_sends.extend(hyundaicanfd.create_steering_messages(self.packer, self.CP, CC, CS, self.CAN, self.frame, apply_steer_req, apply_torque,
                                                           self.apply_angle_last, self.lkas_max_torque))

    # prevent LFA from activating on LKA steering cars by sending "no lane lines detected" to ADAS ECU
    if self.frame % 5 == 0 and (lka_steering and not camera_scc):
      can_sends.append(hyundaicanfd.create_suppress_lfa(self.packer, self.CP, CC, CS, self.CAN))

    # LFA and HDA icons
    if self.frame % 5 == 0 and (not lka_steering or lka_steering_long):
      can_sends.append(hyundaicanfd.create_lfahda_cluster(self.packer, CC, self.CAN))

    # blinkers
    if lka_steering and self.CP.flags & HyundaiFlags.ENABLE_BLINKERS:
      can_sends.extend(hyundaicanfd.create_spas_messages(self.packer, CC, self.CAN))

    if self.CP.openpilotLongitudinalControl:
      if camera_scc:
        self.canfd_toggle_adas(CC, CS)
      if lka_steering:
        can_sends.extend(hyundaicanfd.create_adrv_messages(self.packer, self.CP, CC, CS, self.CAN, self.frame,
                                                           hud_control, self.apply_angle_last))
      else:
        can_sends.extend(hyundaicanfd.create_fca_warning_light(self.packer, self.CP, self.CAN, self.frame))
      if self.frame % 2 == 0:
        can_sends.append(hyundaicanfd.create_acc_control(self.packer, self.CP, CC, CS, self.CAN, self.accel_last,
                                                         accel, stopping, set_speed_in_units, hud_control))
        self.accel_last = accel
    else:
      # button presses
      if (self.frame - self.last_button_frame) * DT_CTRL > 0.25:
        # cruise cancel
        if CC.cruiseControl.cancel:
          if self.CP.flags & HyundaiFlags.CANFD_ALT_BUTTONS:
            can_sends.append(hyundaicanfd.create_acc_cancel(self.packer, self.CP, CS, self.CAN))
            self.last_button_frame = self.frame
          else:
            for _ in range(20):
              can_sends.append(hyundaicanfd.create_buttons(self.packer, self.CP, self.CAN, CS.buttons_counter + 1, Buttons.CANCEL))
            self.last_button_frame = self.frame

        # cruise standstill resume
        elif CC.cruiseControl.resume:
          if self.CP.flags & HyundaiFlags.CANFD_ALT_BUTTONS and CS.canfd_buttons:
            for _ in range(20):
              can_sends.append(hyundaicanfd.create_buttons_canfd_alt(self.packer, self.CP, self.CAN, CS.buttons_counter + 1, Buttons.RES_ACCEL))
            self.last_button_frame = self.frame
          else:
            for _ in range(20):
              can_sends.append(hyundaicanfd.create_buttons(self.packer, self.CP, self.CAN, CS.buttons_counter + 1, Buttons.RES_ACCEL))
            self.last_button_frame = self.frame

    return can_sends

  def canfd_toggle_adas(self, CC, CS):
    trigger_min = -200
    trigger_start = 6
    self.MainMode_ACC_trigger = max(trigger_min, self.MainMode_ACC_trigger - 1)
    self.LFA_trigger = max(trigger_min, self.LFA_trigger - 1)
    if self.MainMode_ACC_trigger == trigger_min and self.LFA_trigger == trigger_min:
      if CC.enabled and not CS.MainMode_ACC and CS.out.vEgo > 3.:
        self.MainMode_ACC_trigger = trigger_start
      elif CC.latActive and CS.MainMode_ACC and CS.LFA_ICON == 0:
        self.LFA_trigger = trigger_start
