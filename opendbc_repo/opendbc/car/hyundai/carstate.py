from collections import deque
import copy
import math

from opendbc.can.parser import CANParser
from opendbc.can.can_define import CANDefine
from opendbc.car import Bus, create_button_events, structs
from opendbc.car.common.conversions import Conversions as CV
from opendbc.car.hyundai.hyundaicanfd import CanBus
from opendbc.car.hyundai.values import HyundaiFlags, CAR, DBC, Buttons, CarControllerParams, HyundaiExFlags
from opendbc.car.interfaces import CarStateBase

from openpilot.selfdrive.controls.neokii.cruise_state_manager import CruiseStateManager

ButtonType = structs.CarState.ButtonEvent.Type

PREV_BUTTON_SAMPLES = 8
CLUSTER_SAMPLE_RATE = 20  # frames
STANDSTILL_THRESHOLD = 12 * 0.03125 * CV.KPH_TO_MS

# Cancel button can sometimes be ACC pause/resume button, main button can also enable on some cars
ENABLE_BUTTONS = (Buttons.RES_ACCEL, Buttons.SET_DECEL, Buttons.CANCEL)
BUTTONS_DICT = {Buttons.RES_ACCEL: ButtonType.accelCruise, Buttons.SET_DECEL: ButtonType.decelCruise,
                Buttons.GAP_DIST: ButtonType.gapAdjustCruise, Buttons.CANCEL: ButtonType.cancel,
                Buttons.LFA_BUTTON: ButtonType.lfaButton}


class CarState(CarStateBase):
  def __init__(self, CP):
    super().__init__(CP)
    can_define = CANDefine(DBC[CP.carFingerprint][Bus.pt])

    self.cruise_buttons: deque = deque([Buttons.NONE] * PREV_BUTTON_SAMPLES, maxlen=PREV_BUTTON_SAMPLES)
    self.main_buttons: deque = deque([Buttons.NONE] * PREV_BUTTON_SAMPLES, maxlen=PREV_BUTTON_SAMPLES)
    self.lda_button = 0

    self.gear_msg_canfd = "ACCELERATOR" if CP.flags & HyundaiFlags.EV else \
                          "GEAR_ALT" if CP.flags & HyundaiFlags.CANFD_ALT_GEARS else \
                          "GEAR_ALT_2" if CP.flags & HyundaiFlags.CANFD_ALT_GEARS_2 else \
                          "GEAR_SHIFTER"
    if CP.flags & HyundaiFlags.CANFD:
      self.shifter_values = can_define.dv[self.gear_msg_canfd]["GEAR"]
    elif CP.flags & (HyundaiFlags.HYBRID | HyundaiFlags.EV):
      self.shifter_values = can_define.dv["ELECT_GEAR"]["Elect_Gear_Shifter"]
    elif self.CP.flags & HyundaiFlags.CLUSTER_GEARS:
      self.shifter_values = can_define.dv["CLU15"]["CF_Clu_Gear"]
    elif self.CP.flags & HyundaiFlags.TCU_GEARS:
      self.shifter_values = can_define.dv["TCU12"]["CUR_GR"]
    elif CP.flags & HyundaiFlags.FCEV:
      self.shifter_values = can_define.dv["EMS20"]["HYDROGEN_GEAR_SHIFTER"]
    else:
      self.shifter_values = can_define.dv["LVR12"]["CF_Lvr_Gear"]

    self.accelerator_msg_canfd = "ACCELERATOR" if CP.flags & HyundaiFlags.EV else \
                                 "ACCELERATOR_ALT" if CP.flags & HyundaiFlags.HYBRID else \
                                 "ACCELERATOR_BRAKE_ALT"
    self.cruise_btns_msg_canfd = "CRUISE_BUTTONS_ALT" if CP.flags & HyundaiFlags.CANFD_ALT_BUTTONS else \
                                 "CRUISE_BUTTONS"
    self.is_metric = False
    self.buttons_counter = 0
    self.low_speed_alert = False

    self.cruise_info = {}

    self.eps_error_cnt = 0

    self.lfa_info = {}
    self.lfa_alt_info = {}
    self.lfahda_cluster_info = {}
    self.mdps_info = {}
    self.steer_touch_info = {}

    self.ccnc_info_161 = None
    self.ccnc_info_162 = None
    self.adrv_info_200 = None
    self.adrv_info_1ea = None
    self.adrv_info_160 = None
    self.hda_info_4a3 = None

    self.cruise_buttons_msg = None
    self.lfa_block_msg = None

    # On some cars, CLU15->CF_Clu_VehicleSpeed can oscillate faster than the dash updates. Sample at 5 Hz
    self.cluster_speed = 0
    self.cluster_speed_counter = CLUSTER_SAMPLE_RATE

    self.params = CarControllerParams(CP)

    self.lfa_btn = 0
    self.lfa_enabled = False
    self.main_enabled = False

    self.canfd_buttons = None

    self.MainMode_ACC = False
    self.LFA_ICON = 0

    self.ff_distance = 0
    self.lf_distance = 0
    self.rf_distance = 0
    self.lr_distance = 0
    self.rr_distance = 0

  def recent_button_interaction(self) -> bool:
    # On some newer model years, the CANCEL button acts as a pause/resume button based on the PCM state
    # To avoid re-engaging when openpilot cancels, check user engagement intention via buttons
    # Main button also can trigger an engagement on these cars
    return any(btn in ENABLE_BUTTONS for btn in self.cruise_buttons) or any(self.main_buttons)

  def update(self, can_parsers) -> structs.CarState:
    cp = can_parsers[Bus.pt]
    cp_cam = can_parsers[Bus.cam]

    if self.CP.flags & HyundaiFlags.CANFD:
      return self.update_canfd(can_parsers)

    ret = structs.CarState()
    cp_cruise = cp_cam if self.CP.flags & HyundaiFlags.CAMERA_SCC else cp
    self.is_metric = cp.vl["CLU11"]["CF_Clu_SPEED_UNIT"] == 0
    speed_factor = CV.KPH_TO_MS if self.is_metric else CV.MPH_TO_MS

    ret.doorOpen = any([cp.vl["CGW1"]["CF_Gway_DrvDrSw"], cp.vl["CGW1"]["CF_Gway_AstDrSw"],
                        cp.vl["CGW2"]["CF_Gway_RLDrSw"], cp.vl["CGW2"]["CF_Gway_RRDrSw"]])

    ret.seatbeltUnlatched = cp.vl["CGW1"]["CF_Gway_DrvSeatBeltSw"] == 0

    ret.wheelSpeeds = self.get_wheel_speeds(
      cp.vl["WHL_SPD11"]["WHL_SPD_FL"],
      cp.vl["WHL_SPD11"]["WHL_SPD_FR"],
      cp.vl["WHL_SPD11"]["WHL_SPD_RL"],
      cp.vl["WHL_SPD11"]["WHL_SPD_RR"],
    )
    ret.vEgoRaw = (ret.wheelSpeeds.fl + ret.wheelSpeeds.fr + ret.wheelSpeeds.rl + ret.wheelSpeeds.rr) / 4.
    ret.vEgo, ret.aEgo = self.update_speed_kf(ret.vEgoRaw)
    ret.standstill = ret.wheelSpeeds.fl <= STANDSTILL_THRESHOLD and ret.wheelSpeeds.rr <= STANDSTILL_THRESHOLD

    self.cluster_speed_counter += 1
    if self.cluster_speed_counter > CLUSTER_SAMPLE_RATE:
      self.cluster_speed = cp.vl["CLU15"]["CF_Clu_VehicleSpeed"]
      self.cluster_speed_counter = 0

      # Mimic how dash converts to imperial.
      if not self.is_metric:
        self.cluster_speed = math.floor(self.cluster_speed * CV.KPH_TO_MPH + CV.KPH_TO_MPH)

    #ret.vEgoCluster = self.cluster_speed * speed_factor
    #ret.vEgoCluster = ret.vEgo * 1.037
    cluSpeed = cp.vl["CLU11"]["CF_Clu_Vanz"]
    decimal = cp.vl["CLU11"]["CF_Clu_VanzDecimal"]
    if 0. < decimal < 0.5:
      cluSpeed += decimal

    ret.vEgoCluster = cluSpeed * speed_factor
    vEgoClu, aEgoClu = self.update_clu_speed_kf(ret.vEgoCluster)
    ret.exState.vCluRatio = (ret.vEgo / vEgoClu) if (vEgoClu > 3. and ret.vEgo > 3.) else 1.0

    ret.steeringAngleDeg = cp.vl["SAS11"]["SAS_Angle"]
    ret.steeringRateDeg = cp.vl["SAS11"]["SAS_Speed"]
    ret.yawRate = cp.vl["ESP12"]["YAW_RATE"]
    ret.leftBlinker, ret.rightBlinker = self.update_blinker_from_lamp(50, cp.vl["CGW1"]["CF_Gway_TurnSigLh"],
                                                                      cp.vl["CGW1"]["CF_Gway_TurnSigRh"])
    ret.steeringTorque = cp.vl["MDPS12"]["CR_Mdps_StrColTq"]
    ret.steeringTorqueEps = cp.vl["MDPS12"]["CR_Mdps_OutTq"]
    ret.steeringPressed = self.update_steering_pressed(abs(ret.steeringTorque) > self.params.STEER_THRESHOLD, 5)
    self.eps_error_cnt += 1 if not ret.standstill and cp.vl["MDPS12"]["CF_Mdps_ToiUnavail"] != 0 else -self.eps_error_cnt
    #ret.steerFaultTemporary = cp.vl["MDPS12"]["CF_Mdps_ToiUnavail"] != 0 or cp.vl["MDPS12"]["CF_Mdps_ToiFlt"] != 0
    ret.steerFaultTemporary = self.eps_error_cnt > 100

    # cruise state
    if self.CP.openpilotLongitudinalControl:
      # These are not used for engage/disengage since openpilot keeps track of state using the buttons
      ret.cruiseState.available = cp.vl["TCS13"]["ACCEnable"] == 0
      ret.cruiseState.enabled = cp.vl["TCS13"]["ACC_REQ"] == 1
      ret.cruiseState.standstill = False
      ret.cruiseState.nonAdaptive = False
    else:
      ret.cruiseState.available = cp_cruise.vl["SCC11"]["MainMode_ACC"] == 1
      ret.cruiseState.enabled = cp_cruise.vl["SCC12"]["ACCMode"] != 0
      ret.cruiseState.standstill = cp_cruise.vl["SCC11"]["SCCInfoDisplay"] == 4.
      ret.cruiseState.nonAdaptive = cp_cruise.vl["SCC11"]["SCCInfoDisplay"] == 2.  # Shows 'Cruise Control' on dash
      ret.cruiseState.speed = cp_cruise.vl["SCC11"]["VSetDis"] * speed_factor

    # TODO: Find brake pressure
    ret.brake = 0
    ret.brakePressed = cp.vl["TCS13"]["DriverOverride"] == 2  # 2 includes regen braking by user on HEV/EV
    ret.brakeHoldActive = cp.vl["TCS15"]["AVH_LAMP"] == 2  # 0 OFF, 1 ERROR, 2 ACTIVE, 3 READY
    ret.parkingBrake = cp.vl["TCS13"]["PBRAKE_ACT"] == 1
    ret.espDisabled = cp.vl["TCS11"]["TCS_PAS"] == 1
    ret.espActive = cp.vl["TCS11"]["ABS_ACT"] == 1
    ret.accFaulted = cp.vl["TCS13"]["ACCEnable"] != 0  # 0 ACC CONTROL ENABLED, 1-3 ACC CONTROL DISABLED
    ret.brakeLights = bool(ret.brakePressed)

    if self.CP.flags & (HyundaiFlags.HYBRID | HyundaiFlags.EV | HyundaiFlags.FCEV):
      if self.CP.flags & HyundaiFlags.FCEV:
        ret.gas = cp.vl["FCEV_ACCELERATOR"]["ACCELERATOR_PEDAL"] / 254.
      elif self.CP.flags & HyundaiFlags.HYBRID:
        ret.gas = cp.vl["E_EMS11"]["CR_Vcu_AccPedDep_Pos"] / 254.
      else:
        ret.gas = cp.vl["E_EMS11"]["Accel_Pedal_Pos"] / 254.
      ret.gasPressed = ret.gas > 0
    else:
      ret.gas = cp.vl["EMS12"]["PV_AV_CAN"] / 100.
      ret.gasPressed = bool(cp.vl["EMS16"]["CF_Ems_AclAct"])

    # Gear Selection via Cluster - For those Kia/Hyundai which are not fully discovered, we can use the Cluster Indicator for Gear Selection,
    # as this seems to be standard over all cars, but is not the preferred method.
    if self.CP.flags & (HyundaiFlags.HYBRID | HyundaiFlags.EV):
      gear = cp.vl["ELECT_GEAR"]["Elect_Gear_Shifter"]
    elif self.CP.flags & HyundaiFlags.FCEV:
      gear = cp.vl["EMS20"]["HYDROGEN_GEAR_SHIFTER"]
    elif self.CP.flags & HyundaiFlags.CLUSTER_GEARS:
      gear = cp.vl["CLU15"]["CF_Clu_Gear"]
    elif self.CP.flags & HyundaiFlags.TCU_GEARS:
      gear = cp.vl["TCU12"]["CUR_GR"]
    else:
      gear = cp.vl["LVR12"]["CF_Lvr_Gear"]

    ret.gearShifter = self.parse_gear_shifter(self.shifter_values.get(gear))

    if not self.CP.openpilotLongitudinalControl or self.CP.flags & HyundaiFlags.CAMERA_SCC:
      aeb_src = "FCA11" if self.CP.flags & HyundaiFlags.USE_FCA.value else "SCC12"
      aeb_sig = "FCA_CmdAct" if self.CP.flags & HyundaiFlags.USE_FCA.value else "AEB_CmdAct"
      aeb_warning = cp_cruise.vl[aeb_src]["CF_VSM_Warn"] != 0
      scc_warning = cp_cruise.vl["SCC12"]["TakeOverReq"] == 1  # sometimes only SCC system shows an FCW
      aeb_braking = cp_cruise.vl[aeb_src]["CF_VSM_DecCmdAct"] != 0 or cp_cruise.vl[aeb_src][aeb_sig] != 0
      ret.stockFcw = (aeb_warning or scc_warning) and not aeb_braking
      ret.stockAeb = aeb_warning and aeb_braking

    if self.CP.enableBsm:
      ret.leftBlindspot = cp.vl["LCA11"]["CF_Lca_IndLeft"] != 0
      ret.rightBlindspot = cp.vl["LCA11"]["CF_Lca_IndRight"] != 0

    # save the entire LKAS11, CLU11, MDPS12, LFAHDA_MFC, SCC11, SCC12, SCC13, SCC14
    self.lkas11 = copy.copy(cp_cam.vl["LKAS11"])
    self.clu11 = copy.copy(cp.vl["CLU11"])
    self.mdps12 = copy.copy(cp.vl["MDPS12"])
    self.scc11 = copy.copy(cp_cruise.vl["SCC11"]) if "SCC11" in cp_cruise.vl else None
    self.scc12 = copy.copy(cp_cruise.vl["SCC12"]) if "SCC12" in cp_cruise.vl else None
    self.scc13 = copy.copy(cp_cruise.vl["SCC13"]) if self.CP.exFlags & HyundaiExFlags.SCC13 else None
    self.scc14 = copy.copy(cp_cruise.vl["SCC14"]) if self.CP.exFlags & HyundaiExFlags.SCC14 else None

    #self.fca11 = copy.copy(cp_cruise.vl["FCA11"]) if "FCA11" in cp_cruise.vl else None
    #self.fca12 = copy.copy(cp_cruise.vl["FCA12"]) if "FCA12" in cp_cruise.vl else None

    self.steer_state = cp.vl["MDPS12"]["CF_Mdps_ToiActive"]  # 0 NOT ACTIVE, 1 ACTIVE
    prev_cruise_buttons = self.cruise_buttons[-1]
    #self.cruise_buttons.extend(cp.vl_all["CLU11"]["CF_Clu_CruiseSwState"])
    prev_lda_button = self.lda_button

    if self.CP.flags & HyundaiFlags.HAS_LDA_BUTTON:
      self.lda_button = cp.vl["BCM_PO_11"]["LDA_BTN"]
      if cp.vl["BCM_PO_11"]["LDA_BTN"]:
        cruise_button = [Buttons.LFA_BUTTON]
      else:
        cruise_button = cp.vl_all["CLU11"]["CF_Clu_CruiseSwState"]
    else:
      cruise_button = cp.vl_all["CLU11"]["CF_Clu_CruiseSwState"]
    self.cruise_buttons.extend(cruise_button)

    prev_main_buttons = self.main_buttons[-1]
    self.main_buttons.extend(cp.vl_all["CLU11"]["CF_Clu_CruiseSwMain"])

    ret.buttonEvents = [*create_button_events(self.cruise_buttons[-1], prev_cruise_buttons, BUTTONS_DICT),
                        *create_button_events(self.main_buttons[-1], prev_main_buttons, {1: ButtonType.mainCruise}),
                        *create_button_events(self.lda_button, prev_lda_button, {1: ButtonType.lkas})]

    ret.blockPcmEnable = not self.recent_button_interaction()

    # low speed steer alert hysteresis logic (only for cars with steer cut off above 10 m/s)
    if ret.vEgo < (self.CP.minSteerSpeed + 2.) and self.CP.minSteerSpeed > 10.:
      self.low_speed_alert = True
    if ret.vEgo > (self.CP.minSteerSpeed + 4.):
      self.low_speed_alert = False
    ret.lowSpeedAlert = self.low_speed_alert

    if self.CP.exFlags & HyundaiExFlags.TPMS:
      tpms = ret.exState.tpms
      tpms_unit = cp.vl["TPMS11"]["UNIT"] * 0.725 if int(cp.vl["TPMS11"]["UNIT"]) > 0 else 1.
      tpms.fl = tpms_unit * cp.vl["TPMS11"]["PRESSURE_FL"]
      tpms.fr = tpms_unit * cp.vl["TPMS11"]["PRESSURE_FR"]
      tpms.rl = tpms_unit * cp.vl["TPMS11"]["PRESSURE_RL"]
      tpms.rr = tpms_unit * cp.vl["TPMS11"]["PRESSURE_RR"]

    if self.CP.exFlags & HyundaiExFlags.AUTOHOLD:
      ret.exState.autoHold = cp.vl["ESP11"]["AVH_STAT"]

    if self.CP.exFlags & HyundaiExFlags.NAVI:
      ret.exState.navLimitSpeed = cp.vl["Navi_HU"]["SpeedLim_Nav_Clu"]

    if self.CP.flags & HyundaiFlags.HAS_LDA_BUTTON and not self.CP.openpilotLongitudinalControl:
      prev_lfa_btn = self.lfa_btn
      self.lfa_btn = cp.vl["BCM_PO_11"]["LDA_BTN"]
      if prev_lfa_btn != 1 and self.lfa_btn == 1:
        self.lfa_enabled = not self.lfa_enabled
      ret.cruiseState.available = self.lfa_enabled

    if prev_main_buttons == 0 and self.main_buttons[-1] != 0:
      self.main_enabled = not self.main_enabled
      ret.cruiseState.available = self.main_enabled

    if self.CP.openpilotLongitudinalControl and CruiseStateManager.instance().cruise_state_control:
      CruiseStateManager.instance().update(ret, self.main_buttons)

    return ret

  def update_canfd(self, can_parsers) -> structs.CarState:
    cp = can_parsers[Bus.pt]
    cp_cam = can_parsers[Bus.cam]

    ret = structs.CarState()

    self.is_metric = cp.vl["CRUISE_BUTTONS_ALT"]["DISTANCE_UNIT"] != 1
    speed_factor = CV.KPH_TO_MS if self.is_metric else CV.MPH_TO_MS

    if self.CP.flags & (HyundaiFlags.EV | HyundaiFlags.HYBRID):
      offset = 255. if self.CP.flags & HyundaiFlags.EV else 1023.
      ret.gas = cp.vl[self.accelerator_msg_canfd]["ACCELERATOR_PEDAL"] / offset
      ret.gasPressed = ret.gas > 1e-5
    else:
      ret.gasPressed = bool(cp.vl[self.accelerator_msg_canfd]["ACCELERATOR_PEDAL_PRESSED"])

    ret.brakePressed = cp.vl["TCS"]["DriverBraking"] == 1
    ret.brakeLights = bool(ret.brakePressed)

    ret.doorOpen = cp.vl["DOORS_SEATBELTS"]["DRIVER_DOOR"] == 1
    ret.seatbeltUnlatched = cp.vl["DOORS_SEATBELTS"]["DRIVER_SEATBELT"] == 0

    gear = cp.vl[self.gear_msg_canfd]["GEAR"]
    ret.gearShifter = self.parse_gear_shifter(self.shifter_values.get(gear))

    # TODO: figure out positions
    ret.wheelSpeeds = self.get_wheel_speeds(
      cp.vl["WHEEL_SPEEDS"]["WHL_SpdFLVal"],
      cp.vl["WHEEL_SPEEDS"]["WHL_SpdFRVal"],
      cp.vl["WHEEL_SPEEDS"]["WHL_SpdRLVal"],
      cp.vl["WHEEL_SPEEDS"]["WHL_SpdRRVal"],
    )
    ret.vEgoRaw = (ret.wheelSpeeds.fl + ret.wheelSpeeds.fr + ret.wheelSpeeds.rl + ret.wheelSpeeds.rr) / 4.
    ret.vEgo, ret.aEgo = self.update_speed_kf(ret.vEgoRaw)
    #ret.vEgoCluster = ret.vEgo * 1.037
    cluSpeed = cp.vl["CRUISE_BUTTONS_ALT"]["CLUSTER_SPEED_KPH"]
    ret.vEgoCluster = cluSpeed * speed_factor

    vEgoClu, aEgoClu = self.update_clu_speed_kf(ret.vEgoCluster)
    ret.exState.vCluRatio = (ret.vEgo / vEgoClu) if (vEgoClu > 3. and ret.vEgo > 3.) else 1.0

    ret.standstill = ret.wheelSpeeds.fl <= STANDSTILL_THRESHOLD and ret.wheelSpeeds.fr <= STANDSTILL_THRESHOLD and \
                     ret.wheelSpeeds.rl <= STANDSTILL_THRESHOLD and ret.wheelSpeeds.rr <= STANDSTILL_THRESHOLD

    ret.steeringRateDeg = cp.vl["STEERING_SENSORS"]["STEERING_RATE"]
    ret.steeringAngleDeg = cp.vl["STEERING_SENSORS"]["STEERING_ANGLE"]
    ret.steeringTorque = cp.vl["MDPS"]["STEERING_COL_TORQUE"]
    ret.steeringTorqueEps = cp.vl["MDPS"]["STEERING_OUT_TORQUE"]
    ret.steeringPressed = self.update_steering_pressed(abs(ret.steeringTorque) > self.params.STEER_THRESHOLD, 5)
    ret.steerFaultTemporary = cp.vl["MDPS"]["LKA_FAULT"] or cp.vl["MDPS"]["LKA_ANGLE_FAULT"] != 0
    #if self.CP.flags & HyundaiFlags.CANFD_ANGLE_STEERING:
    #  ret.steerFaultTemporary = ret.steerFaultTemporary or cp.vl["MDPS"]["LKA_ANGLE_FAULT"] != 0

    left_blinker_sig = cp.vl["BLINKERS"]["LEFT_LAMP"] or cp.vl["BLINKERS"]["LEFT_LAMP_ALT"]
    right_blinker_sig = cp.vl["BLINKERS"]["RIGHT_LAMP"] or cp.vl["BLINKERS"]["RIGHT_LAMP_ALT"]
    ret.leftBlinker, ret.rightBlinker = self.update_blinker_from_lamp(50, left_blinker_sig, right_blinker_sig)

    if self.CP.enableBsm:
      cp_bsm_info = cp_cam if (self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC and self.CP.exFlags & HyundaiExFlags.CCNC_HDA2.value) else cp
      if self.CP.exFlags & HyundaiExFlags.CCNC_HDA2.value:
        ret.leftBlindspot = cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["FL_INDICATOR_ALT"] != 0 or cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["INDICATOR_LEFT_THREE"] != 0
        ret.rightBlindspot = cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["FR_INDICATOR_ALT"] != 0 or cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["INDICATOR_RIGHT_THREE"] != 0
      else:
        ret.leftBlindspot = cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["FL_INDICATOR"] != 0 or cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["INDICATOR_LEFT_TWO"] != 0
        ret.rightBlindspot = cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["FR_INDICATOR"] != 0 or cp_bsm_info.vl["BLINDSPOTS_REAR_CORNERS"]["INDICATOR_RIGHT_TWO"] != 0

    # cruise state
    # CAN FD cars enable on main button press, set available if no TCS faults preventing engagement
    ret.cruiseState.available = cp.vl["TCS"]["ACCEnable"] == 0
    if self.CP.openpilotLongitudinalControl:
      # These are not used for engage/disengage since openpilot keeps track of state using the buttons
      ret.cruiseState.enabled = cp.vl["TCS"]["ACC_REQ"] == 1
      ret.cruiseState.standstill = False
      if self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC.value:
        self.MainMode_ACC = cp_cam.vl["SCC_CONTROL"]["MainMode_ACC"] == 1
        self.LFA_ICON = cp_cam.vl["LFAHDA_CLUSTER"]["LFA_ICON"]
    else:
      cp_cruise_info = cp_cam if self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC else cp
      ret.cruiseState.available = cp_cruise_info.vl["SCC_CONTROL"]["MainMode_ACC"] == 1
      ret.cruiseState.enabled = cp_cruise_info.vl["SCC_CONTROL"]["ACCMode"] in (1, 2)
      ret.cruiseState.standstill = cp_cruise_info.vl["SCC_CONTROL"]["CRUISE_STANDSTILL"] == 1
      ret.cruiseState.speed = cp_cruise_info.vl["SCC_CONTROL"]["VSetDis"] * speed_factor

      self.cruise_info = copy.copy(cp_cruise_info.vl["SCC_CONTROL"])
      ret.brakeHoldActive = cp.vl["ESP_STATUS"]["AUTO_HOLD"] == 1 and cp_cruise_info.vl["SCC_CONTROL"]["ACCMode"] not in (1, 2)

    if self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC.value:
      self.cruise_info = copy.copy(cp_cam.vl["SCC_CONTROL"])
      self.lfa_info = copy.copy(cp_cam.vl["LFA"])
      self.lfa_alt_info = copy.copy(cp_cam.vl["LFA_ALT"])
      self.lfahda_cluster_info = copy.copy(cp_cam.vl["LFAHDA_CLUSTER"])
      self.mdps_info = copy.copy(cp.vl["MDPS"])

      if self.CP.exFlags & HyundaiExFlags.STEER_TOUCH:
        self.steer_touch_info = copy.copy(cp.vl["STEER_TOUCH_2AF"])

      if self.CP.exFlags & HyundaiExFlags.CCNC.value:
        if "CCNC_0x161" in cp_cam.vl:
          self.ccnc_info_161 = copy.copy(cp_cam.vl.get("CCNC_0x161", {}))
        if "CCNC_0x162" in cp_cam.vl:
          self.ccnc_info_162 = copy.copy(cp_cam.vl.get("CCNC_0x162", {}))
          self.ff_distance = cp_cam.vl["CCNC_0x162"]["FF_DISTANCE"]
          self.lf_distance = cp_cam.vl["CCNC_0x162"]["LF_DISTANCE"]
          self.rf_distance = cp_cam.vl["CCNC_0x162"]["RF_DISTANCE"]
          self.lr_distance = cp_cam.vl["CCNC_0x162"]["LR_DISTANCE"]
          self.rr_distance = cp_cam.vl["CCNC_0x162"]["RR_DISTANCE"]
      if "ADRV_0x160" in cp_cam.vl:
        self.adrv_info_160 = copy.copy(cp_cam.vl.get("ADRV_0x160", {}))
      if "ADRV_0x200" in cp_cam.vl:
        self.adrv_info_200 = copy.copy(cp_cam.vl.get("ADRV_0x200", {}))
      if "ADRV_0x1ea" in cp_cam.vl:
        self.adrv_info_1ea = copy.copy(cp_cam.vl.get("ADRV_0x1ea", {}))
      if "HDA_INFO_0x4a3" in cp.vl:
        self.hda_info_4a3 = copy.copy(cp.vl.get("HDA_INFO_0x4a3", {}))

    # Manual Speed Limit Assist is a feature that replaces non-adaptive cruise control on EV CAN FD platforms.
    # It limits the vehicle speed, overridable by pressing the accelerator past a certain point.
    # The car will brake, but does not respect positive acceleration commands in this mode
    # TODO: find this message on ICE & HYBRID cars + cruise control signals (if exists)
    if self.CP.flags & HyundaiFlags.EV:
      ret.cruiseState.nonAdaptive = cp.vl["MANUAL_SPEED_LIMIT_ASSIST"]["MSLA_ENABLED"] == 1

    prev_cruise_buttons = self.cruise_buttons[-1]
    #self.cruise_buttons.extend(cp.vl_all[self.cruise_btns_msg_canfd]["CRUISE_BUTTONS"])
    prev_lda_button = self.lda_button

    if cp.vl[self.cruise_btns_msg_canfd]["LDA_BTN"]:
      cruise_button = [Buttons.LFA_BUTTON]
    else:
      cruise_button = cp.vl_all[self.cruise_btns_msg_canfd]["CRUISE_BUTTONS"]
    self.cruise_buttons.extend(cruise_button)

    if self.cruise_btns_msg_canfd in cp.vl_all: #carrot
      if not cp.vl_all[self.cruise_btns_msg_canfd]["CRUISE_BUTTONS"]:
        pass
        #print("empty cruise btns...")
      else:
        self.cruise_buttons_msg = copy.copy(cp.vl_all[self.cruise_btns_msg_canfd])

    prev_main_buttons = self.main_buttons[-1]
    self.main_buttons.extend(cp.vl_all[self.cruise_btns_msg_canfd]["ADAPTIVE_CRUISE_MAIN_BTN"])
    self.lda_button = cp.vl[self.cruise_btns_msg_canfd]["LDA_BTN"]

    self.buttons_counter = cp.vl[self.cruise_btns_msg_canfd]["COUNTER"]
    ret.accFaulted = cp.vl["TCS"]["ACCEnable"] != 0  # 0 ACC CONTROL ENABLED, 1-3 ACC CONTROL DISABLED

    if self.CP.flags & HyundaiFlags.CANFD_LKA_STEERING and not self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC:
      self.lfa_block_msg = copy.copy(cp_cam.vl["CAM_0x362"] if self.CP.flags & HyundaiFlags.CANFD_LKA_STEERING_ALT
                                     else cp_cam.vl["CAM_0x2a4"])

    ret.buttonEvents = [*create_button_events(self.cruise_buttons[-1], prev_cruise_buttons, BUTTONS_DICT),
                        *create_button_events(self.main_buttons[-1], prev_main_buttons, {1: ButtonType.mainCruise}),
                        *create_button_events(self.lda_button, prev_lda_button, {1: ButtonType.lkas})]

    ret.blockPcmEnable = not self.recent_button_interaction()

    if self.CP.exFlags & HyundaiExFlags.TPMS:
      tpms = ret.exState.tpms
      tpms_unit = cp.vl["TPMS"]["UNIT"] * 0.725 if int(cp.vl["TPMS"]["UNIT"]) > 0 else 1.
      tpms.fl = tpms_unit * cp.vl["TPMS"]["PRESSURE_FL"]
      tpms.fr = tpms_unit * cp.vl["TPMS"]["PRESSURE_FR"]
      tpms.rl = tpms_unit * cp.vl["TPMS"]["PRESSURE_RL"]
      tpms.rr = tpms_unit * cp.vl["TPMS"]["PRESSURE_RR"]

    if self.CP.exFlags & HyundaiExFlags.AUTOHOLD:
      ret.exState.autoHold = cp.vl["ESP_STATUS"]["AUTO_HOLD"]

    if self.CP.exFlags & HyundaiExFlags.NAVI:
      if self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC and self.CP.exFlags & HyundaiExFlags.CCNC.value:
        ret.exState.navLimitSpeed = cp_cam.vl["CCNC_0x162"]["SPEEDLIMIT"]
      elif self.CP.flags & HyundaiFlags.CANFD_CAMERA_SCC:
        ret.exState.navLimitSpeed = cp_cam.vl["CLUSTER_SPEED_LIMIT"]["SPEED_LIMIT_1"]
      else:
        ret.exState.navLimitSpeed = cp.vl["CLUSTER_SPEED_LIMIT"]["SPEED_LIMIT_1"]

    self.canfd_buttons = cp.vl[self.cruise_btns_msg_canfd]

    if self.CP.flags & HyundaiFlags.HAS_LDA_BUTTON and not self.CP.openpilotLongitudinalControl:
      prev_lfa_btn = self.lfa_btn
      self.lfa_btn = cp.vl[self.cruise_btns_msg_canfd]["LDA_BTN"]
      if prev_lfa_btn != 1 and self.lfa_btn == 1:
        self.lfa_enabled = not self.lfa_enabled
      ret.cruiseState.available = self.lfa_enabled

    if self.main_buttons[-1] != prev_main_buttons and not self.main_buttons[-1]:
      self.main_enabled = not self.main_enabled
      ret.cruiseState.available = self.main_enabled

    if self.CP.openpilotLongitudinalControl and CruiseStateManager.instance().cruise_state_control:
      CruiseStateManager.instance().update(ret, self.main_buttons)

    return ret

  def get_can_parsers_canfd(self, CP):
    pt_messages = [
      ("WHEEL_SPEEDS", 100),
      ("STEERING_SENSORS", 100),
      ("MDPS", 100),
      ("TCS", 50),
      ("CRUISE_BUTTONS_ALT", 50),
      ("BLINKERS", 4),
      ("DOORS_SEATBELTS", 4),
    ]

    if CP.flags & HyundaiFlags.EV:
      pt_messages += [
        ("ACCELERATOR", 100),
        ("MANUAL_SPEED_LIMIT_ASSIST", 10),
      ]
    else:
      pt_messages += [
        (self.gear_msg_canfd, 100),
        (self.accelerator_msg_canfd, 100),
      ]

    if not CP.flags & HyundaiFlags.CANFD_ALT_BUTTONS:
      pt_messages += [
        ("CRUISE_BUTTONS", 50)
      ]

    if CP.exFlags & HyundaiExFlags.STEER_TOUCH:
      pt_messages += [
        ("STEER_TOUCH_2AF", 10),
      ]

    if CP.enableBsm:
      if CP.flags & HyundaiFlags.CANFD_CAMERA_SCC.value and CP.exFlags & HyundaiExFlags.CCNC_HDA2.value:
        pass
      else:
        pt_messages += [
          ("BLINDSPOTS_REAR_CORNERS", 20),
        ]

    if not CP.flags & HyundaiFlags.CANFD_CAMERA_SCC.value and not CP.openpilotLongitudinalControl:
      pt_messages += [
        ("SCC_CONTROL", 50),
      ]

    if CP.exFlags & HyundaiExFlags.TPMS:
      pt_messages.append(("TPMS", 0))

    if CP.exFlags & HyundaiExFlags.AUTOHOLD:
      pt_messages.append(("ESP_STATUS", 0))

    if CP.flags & HyundaiFlags.CANFD_LKA_STEERING and CP.exFlags & HyundaiExFlags.NAVI and not CP.flags & HyundaiFlags.CANFD_CAMERA_SCC:
      pt_messages.append(("CLUSTER_SPEED_LIMIT", 10))

    if CP.flags & HyundaiFlags.CANFD_LKA_STEERING and CP.flags & HyundaiFlags.CANFD_CAMERA_SCC:
      pt_messages += [
        ("HDA_INFO_0x4a3", 5),
      ]

    cam_messages = []
    if CP.flags & HyundaiFlags.CANFD_LKA_STEERING and not CP.flags & HyundaiFlags.CANFD_CAMERA_SCC:
      block_lfa_msg = "CAM_0x362" if CP.flags & HyundaiFlags.CANFD_LKA_STEERING_ALT else "CAM_0x2a4"
      cam_messages += [(block_lfa_msg, 20)]

    if CP.flags & HyundaiFlags.CANFD_CAMERA_SCC:
      cam_messages += [
        ("SCC_CONTROL", 50),
        ("LFA", 20),
        ("LFA_ALT", 100),
        ("LFAHDA_CLUSTER", 20),
      ]
      if CP.flags & HyundaiFlags.CANFD_LKA_STEERING:
        cam_messages += [
          ("ADRV_0x200", 20),
          ("ADRV_0x1ea", 20),
          ("ADRV_0x160", 50),
        ]
      if CP.exFlags & HyundaiExFlags.CCNC:
        cam_messages += [
          ("CCNC_0x161", 20),
          ("CCNC_0x162", 20),
        ]

    if not CP.flags & HyundaiFlags.CANFD_LKA_STEERING and CP.exFlags & HyundaiExFlags.NAVI:
      cam_messages.append(("CLUSTER_SPEED_LIMIT", 10))

    if CP.enableBsm:
      if CP.flags & HyundaiFlags.CANFD_CAMERA_SCC.value and CP.exFlags & HyundaiExFlags.CCNC_HDA2.value:
        cam_messages += [
          ("BLINDSPOTS_REAR_CORNERS", 20),
        ]

    return {
      Bus.pt: CANParser(DBC[CP.carFingerprint][Bus.pt], pt_messages, CanBus(CP).ECAN),
      Bus.cam: CANParser(DBC[CP.carFingerprint][Bus.pt], cam_messages, CanBus(CP).CAM),
    }

  def get_can_parsers(self, CP):
    if CP.flags & HyundaiFlags.CANFD:
      return self.get_can_parsers_canfd(CP)

    pt_messages = [
      # address, frequency
      ("MDPS12", 50),
      ("TCS11", 100),
      ("TCS13", 50),
      ("TCS15", 10),
      ("CLU11", 50),
      ("CLU15", 5),
      ("ESP12", 100),
      ("CGW1", 10),
      ("CGW2", 5),
      ("CGW4", 5),
      ("WHL_SPD11", 50),
      ("SAS11", 100),
    ]

    if not CP.openpilotLongitudinalControl:
      pt_messages += [
        ("SCC11", 50),
        ("SCC12", 50),
      ]
      if CP.flags & HyundaiFlags.USE_FCA.value:
        pt_messages.append(("FCA11", 50))

    if CP.enableBsm:
      pt_messages.append(("LCA11", 50))

    if CP.exFlags & HyundaiExFlags.TPMS:
      pt_messages.append(("TPMS11", 0))

    if CP.exFlags & HyundaiExFlags.AUTOHOLD:
      pt_messages.append(("ESP11", 50))

    if CP.exFlags & HyundaiExFlags.NAVI:
      pt_messages.append(("Navi_HU", 5))

    if CP.flags & (HyundaiFlags.HYBRID | HyundaiFlags.EV):
      pt_messages.append(("E_EMS11", 50))
    elif CP.flags & HyundaiFlags.FCEV:
      pt_messages.append(("FCEV_ACCELERATOR", 100))
    else:
      pt_messages += [
        ("EMS12", 100),
        ("EMS16", 100),
      ]

    if CP.flags & (HyundaiFlags.HYBRID | HyundaiFlags.EV):
      pt_messages.append(("ELECT_GEAR", 20))
    elif CP.flags & HyundaiFlags.FCEV:
      pt_messages.append(("EMS20", 100))
    elif CP.flags & HyundaiFlags.CLUSTER_GEARS:
      pass
    elif CP.flags & HyundaiFlags.TCU_GEARS:
      pt_messages.append(("TCU12", 100))
    else:
      pt_messages.append(("LVR12", 100))

    if CP.flags & HyundaiFlags.HAS_LDA_BUTTON:
      pt_messages.append(("BCM_PO_11", 50))

    cam_messages = [
      ("LKAS11", 100)
    ]

    if CP.openpilotLongitudinalControl and CP.flags & HyundaiFlags.CAMERA_SCC:
      cam_messages += [
        ("SCC11", 50),
        ("SCC12", 50),
      ]

      if CP.exFlags & HyundaiExFlags.SCC13:
        cam_messages.append(("SCC13", 50))

      if CP.exFlags & HyundaiExFlags.SCC14:
        cam_messages.append(("SCC14", 50))

      if CP.flags & HyundaiFlags.USE_FCA.value:
        cam_messages.append(("FCA11", 50))

    return {
      Bus.pt: CANParser(DBC[CP.carFingerprint][Bus.pt], pt_messages, 0),
      Bus.cam: CANParser(DBC[CP.carFingerprint][Bus.pt], cam_messages, 2),
    }
