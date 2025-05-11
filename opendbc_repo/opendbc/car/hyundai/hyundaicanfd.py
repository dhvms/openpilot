import copy
import numpy as np
from opendbc.car import CanBusBase
from opendbc.car.hyundai.values import HyundaiFlags, HyundaiExFlags

from openpilot.common.params import Params
from openpilot.selfdrive.controls.neokii.navi_controller import SpeedLimiter
from opendbc.car.common.conversions import Conversions as CV

def hyundai_crc8(data: bytes) -> int:
  poly = 0x2F
  crc = 0xFF

  for byte in data:
    crc ^= byte
    for _ in range(8):
      if crc & 0x80:
        crc = ((crc << 1) ^ poly) & 0xFF
      else:
        crc = (crc << 1) & 0xFF

  return crc ^ 0xFF

class CanBus(CanBusBase):
  def __init__(self, CP, fingerprint=None, lka_steering=None) -> None:
    super().__init__(CP, fingerprint)

    if lka_steering is None:
      lka_steering = CP.flags & HyundaiFlags.CANFD_LKA_STEERING.value if CP is not None else False

    # On the CAN-FD platforms, the LKAS camera is on both A-CAN and E-CAN. LKA steering cars
    # have a different harness than the LFA steering variants in order to split
    # a different bus, since the steering is done by different ECUs.
    self._a, self._e = 1, 0
    if lka_steering and not Params().get_bool("CameraSccEnable"):  #배선개조는 무조건 Bus0가 ECAN임.
      self._a, self._e = 0, 1

    self._a += self.offset
    self._e += self.offset
    self._cam = 2 + self.offset

  @property
  def ECAN(self):
    return self._e

  @property
  def ACAN(self):
    return self._a

  @property
  def CAM(self):
    return self._cam


def create_steering_messages(packer, CP, CC, CS, CAN, frame, lat_active, apply_torque, apply_angle, angle_max_torque):
  enabled = CC.enabled
  angle_control = CP.flags & HyundaiFlags.CANFD_ANGLE_STEERING
  camerascc = CP.flags & HyundaiFlags.CANFD_CAMERA_SCC

  common_values = {
    "LKA_MODE": 2,
    "LKA_ICON": 2 if enabled else 1,
    "TORQUE_REQUEST": apply_torque,
    "LKA_ASSIST": 0,
    "STEER_REQ": 1 if lat_active else 0,
    "STEER_MODE": 0,
    "HAS_LANE_SAFETY": 0,  # hide LKAS settings
    "NEW_SIGNAL_2": 0,
    "LKA_AVAILABLE": 0,
  }

  lfa_values = copy.copy(common_values)

  lkas_values = copy.copy(common_values)

  # For cars with an ADAS ECU (commonly HDA2), by sending LKAS actuation messages we're
  # telling the ADAS ECU to forward our steering and disable stock LFA lane centering.
  ret = []

  values = CS.mdps_info
  if angle_control:
    if CS.lfa_alt_info is not None:
      values["LKA_ANGLE_ACTIVE"] = CS.lfa_alt_info["LKAS_ANGLE_ACTIVE"]
  else:
    if CS.lfa_info is not None:
      values["LKA_ACTIVE"] = 1 if CS.lfa_info["STEER_REQ"] == 1 else 0

  if frame % 1000 < 40:
    values["STEERING_COL_TORQUE"] += 220
  ret.append(packer.make_can_msg("MDPS", CAN.CAM, values))

  if frame % 10 == 0:
    if CP.exFlags & HyundaiExFlags.STEER_TOUCH:
      values = CS.steer_touch_info
      if frame % 1000 < 40:
        values["TOUCH_DETECT"] = 3
        values["TOUCH1"] = 50
        values["TOUCH2"] = 50
        values["CHECKSUM_"] = 0
        dat = packer.make_can_msg("STEER_TOUCH_2AF", 0, values)[1]
        values["CHECKSUM_"] = hyundai_crc8(dat[1:8])

      ret.append(packer.make_can_msg("STEER_TOUCH_2AF", CAN.CAM, values))

  if angle_control:
    if camerascc:
      lfa_values |= {
        "LKA_MODE": 0,  # TODO: not used by the stock system
        "TORQUE_REQUEST": 0,  # we don't use torque
        "STEER_REQ": 0,  # we don't use torque
        # this goes 0 when LFA lane changes, 3 when LKA_ICON is >=green
        "LKA_AVAILABLE": 3 if lat_active else 0,
        #"LKAS_ANGLE_CMD": 0,
        #"LKAS_ANGLE_ACTIVE": 0,
        #"LKAS_ANGLE_MAX_TORQUE": 0,
      }

      values = {
        "LKAS_ANGLE_CMD": apply_angle,
        "LKAS_ANGLE_ACTIVE": 2 if lat_active else 1,
        "LKAS_ANGLE_MAX_TORQUE": angle_max_torque if lat_active else 0,
      }
      ret.append(packer.make_can_msg("LFA_ALT", CAN.ECAN, values))

    else:
      lkas_values |= {
        "LKA_MODE": 0,  # TODO: not used by the stock system
        "TORQUE_REQUEST": 0,  # we don't use torque
        "STEER_REQ": 0,  # we don't use torque
        # this goes 0 when LFA lane changes, 3 when LKA_ICON is >=green
        "LKA_AVAILABLE": 3 if lat_active else 0,
        "LKAS_ANGLE_CMD": apply_angle,
        "LKAS_ANGLE_ACTIVE": 2 if lat_active else 1,
        "LKAS_ANGLE_MAX_TORQUE": angle_max_torque if lat_active else 0,
      }

    if CP.flags & HyundaiFlags.CANFD_LKA_STEERING:
      lkas_msg = "LKAS_ALT" if CP.flags & HyundaiFlags.CANFD_LKA_STEERING_ALT else "LKAS"
      if CP.openpilotLongitudinalControl:
        ret.append(packer.make_can_msg("LFA", CAN.ECAN, lfa_values))
      if not (CP.flags & HyundaiFlags.CANFD_CAMERA_SCC):
        ret.append(packer.make_can_msg(lkas_msg, CAN.ACAN, lkas_values))
    else:
      ret.append(packer.make_can_msg("LFA", CAN.ECAN, lfa_values))

  return ret


def create_suppress_lfa(packer, CP, CC, CS, CAN):
  enabled = CC.enabled
  lfa_block_msg = CS.lfa_block_msg
  lka_steering_alt = CP.flags & HyundaiFlags.CANFD_LKA_STEERING_ALT

  suppress_msg = "CAM_0x362" if lka_steering_alt else "CAM_0x2a4"
  msg_bytes = 32 if lka_steering_alt else 24

  values = {f"BYTE{i}": lfa_block_msg[f"BYTE{i}"] for i in range(3, msg_bytes) if i != 7}
  values["COUNTER"] = lfa_block_msg["COUNTER"]
  values["SET_ME_0"] = 0
  values["SET_ME_0_2"] = 0
  values["LEFT_LANE_LINE"] = 0 if enabled else 3
  values["RIGHT_LANE_LINE"] = 0 if enabled else 3
  return packer.make_can_msg(suppress_msg, CAN.ACAN, values)


def create_buttons(packer, CP, CAN, cnt, btn):
  values = {
    "COUNTER": cnt,
    "SET_ME_1": 1,
    "CRUISE_BUTTONS": btn,
  }
  bus = CAN.ECAN if CP.flags & HyundaiFlags.CANFD_LKA_STEERING else CAN.CAM
  return packer.make_can_msg("CRUISE_BUTTONS", bus, values)


def create_buttons_canfd_alt(packer, CP, CAN, cnt, btn):
  values = {
    "COUNTER": cnt % 256,
    "CRUISE_BUTTONS": btn,
  }
  bus = CAN.ECAN if CP.flags & HyundaiFlags.CANFD_LKA_STEERING else CAN.CAM
  return packer.make_can_msg("CRUISE_BUTTONS_ALT", bus, values)


def create_acc_cancel(packer, CP, CS, CAN):
  cruise_info_copy = CS.cruise_info
  camerascc = CP.flags & HyundaiFlags.CANFD_CAMERA_SCC

  # TODO: why do we copy different values here?
  if camerascc:
    values = {s: cruise_info_copy[s] for s in [
      "COUNTER",
      "CHECKSUM",
      "ACCMode",
      "VSetDis",
      "CRUISE_STANDSTILL",
      "MainMode_ACC",
      "ZEROS_5",
      "DISTANCE_SETTING",
    ]}
  else:
    values = {s: cruise_info_copy[s] for s in [
      "COUNTER",
      "CHECKSUM",
      "ACCMode",
      "VSetDis",
      "CRUISE_STANDSTILL",
    ]}
  values.update({
    "ACCMode": 4,
    "aReqRaw": 0.0,
    "aReqValue": 0.0,
  })
  return packer.make_can_msg("SCC_CONTROL", CAN.ECAN, values)


def create_lfahda_cluster(packer, CC, CAN):
  enabled = CC.enabled

  values = {
    "HDA_ICON": 1 if enabled else 0,
    "LFA_ICON": 2 if enabled else 0,
  }
  return packer.make_can_msg("LFAHDA_CLUSTER", CAN.ECAN, values)


def create_acc_control(packer, CP, CC, CS, CAN, accel_last, accel, stopping, set_speed, hud):
  enabled = CC.enabled
  gas_override = CC.cruiseControl.override
  camerascc = CP.flags & HyundaiFlags.CANFD_CAMERA_SCC

  jerk = 5
  jn = jerk / 50
  if not enabled or gas_override:
    a_val, a_raw = 0, 0
  else:
    a_raw = accel
    a_val = np.clip(accel, accel_last - jn, accel_last + jn)

  if camerascc:
    values = CS.cruise_info
    values.update({s: CS.cruise_info[s] for s in ["ACC_ObjDist", "ACC_ObjRelSpd"]})
    values |= {
      "ACCMode": 0 if not enabled else (2 if gas_override else 1),
      "MainMode_ACC": 1,
      "StopReq": 1 if stopping else 0,
      "aReqValue": a_val,
      "aReqRaw": a_raw,
      "VSetDis": set_speed,
      "JerkLowerLimit": jerk if enabled else 1,
      "JerkUpperLimit": 3.0,

      "SET_ME_2": 4,
      "SET_ME_TMP_64": 100,
      "DISTANCE_SETTING": hud.leadDistanceBars,
      "ZEROS_5": 0,

      "TARGET_DISTANCE": CS.out.vEgo * 1.0 + 4.0,
      "CRUISE_STANDSTILL": 1 if stopping and CS.out.aEgo > -0.1 else 0,

      "NEW_SIGNAL_2": 0,  # 이것이 켜지면 가속을 안하는듯함.
      "NEW_SIGNAL_4": 9 if hud.leadVisible else 0,
      "NEW_SIGNAL_1": 0,  # 눈이 묻어 레이더오류시... 2가 됨. 이때 가속을 안함...
    }

    hud_lead_info = 0
    if hud.leadVisible:
      hud_lead_info = 1 if values["ACC_ObjRelSpd"] > 0 else 2
    values["HUD_LEAD_INFO"] = hud_lead_info

  else:
    values = {
      "ACCMode": 0 if not enabled else (2 if gas_override else 1),
      "MainMode_ACC": 1,
      "StopReq": 1 if stopping else 0,
      "aReqValue": a_val,
      "aReqRaw": a_raw,
      "VSetDis": set_speed,
      "JerkLowerLimit": jerk if enabled else 1,
      "JerkUpperLimit": 3.0,

      "ACC_ObjDist": 1,
      "SET_ME_2": 4,
      "SET_ME_TMP_64": 100,
      "DISTANCE_SETTING": hud.leadDistanceBars, # + 5,
      "CRUISE_STANDSTILL": 1 if stopping and CS.out.cruiseState.standstill else 0,
    }

  return packer.make_can_msg("SCC_CONTROL", CAN.ECAN, values)


def create_spas_messages(packer, CC, CAN):
  ret = []

  values = {
  }
  ret.append(packer.make_can_msg("SPAS1", CAN.ECAN, values))

  blink = 0
  if CC.leftBlinker:
    blink = 3
  elif CC.rightBlinker:
    blink = 4
  values = {
    "BLINKER_CONTROL": blink,
  }
  ret.append(packer.make_can_msg("SPAS2", CAN.ECAN, values))

  return ret


def create_fca_warning_light(packer, CP, CAN, frame):
  ret = []
  if CP.flags & HyundaiFlags.CANFD_CAMERA_SCC.value:
    return ret

  if frame % 2 == 0:
    values = {
      'AEB_SETTING': 1,  # show AEB disabled icon
      'SET_ME_2': 2,
      'SET_ME_FF': 255,
      'SET_ME_FC': 252,
      'SET_ME_9': 9,
    }
    ret.append(packer.make_can_msg("ADRV_0x160", CAN.ECAN, values))
  return ret


def create_adrv_messages(packer, CP, CC, CS, CAN, frame, hud, disp_angle):
  main_enabled = CS.out.cruiseState.available
  cruise_enabled = CC.enabled
  lat_active = CC.latActive
  ccnc = CP.exFlags & HyundaiExFlags.CCNC
  nav_active = SpeedLimiter.instance().get_active()
  hdp_active = cruise_enabled and nav_active
  set_speed_in_units = hud.setSpeed * (CV.MS_TO_KPH if CS.is_metric else CV.MS_TO_MPH)

  # messages needed to car happy after disabling
  # the ADAS Driving ECU to do longitudinal control

  ret = []
  if CP.flags & HyundaiFlags.CANFD_CAMERA_SCC:
    if frame % 5 == 0 and CS.ccnc_info_161 is not None and ccnc:
      values = CS.ccnc_info_161
      values |= {
        "SETSPEED": 6 if hdp_active else 3 if main_enabled else 0,
        "SETSPEED_HUD": 5 if hdp_active else 2 if cruise_enabled else 1,
        "vSetDis": int(set_speed_in_units + 0.5),

        "DISTANCE": 4 if hdp_active else hud.leadDistanceBars,
        "DISTANCE_LEAD": 2 if cruise_enabled and hud.leadVisible else 0,
        "DISTANCE_CAR": 3 if hdp_active else 2 if cruise_enabled else 1 if main_enabled else 0,
        "DISTANCE_SPACING": 5 if hdp_active else 1 if cruise_enabled else 0,

        "TARGET": 1 if cruise_enabled else 0,
        "TARGET_DISTANCE": int(hud.leadDistance),

        "BACKGROUND": 1 if cruise_enabled else 3 if main_enabled else 7,
        "CENTERLINE": 1 if lat_active else 0,
        "CAR_CIRCLE": 2 if hdp_active else 1 if lat_active else 0,

        "NAV_ICON": 2 if nav_active else 1,
        "HDA_ICON": 5 if hdp_active else 2 if lat_active else 1,
        "LFA_ICON": 5 if hdp_active else 2 if lat_active else 1,
        "LKA_ICON": 4 if lat_active else 3,
        "FCA_ALT_ICON": 0,
        "DAW_ICON": 0,

        "LANELINE_LEFT_POSITION": 15,
        "LANELINE_RIGHT_POSITION": 15,

        "LCA_LEFT_ARROW": 2 if CS.out.leftBlinker else 0,
        "LCA_RIGHT_ARROW": 2 if CS.out.rightBlinker else 0,

        "LCA_LEFT_ICON": 1 if CS.out.leftBlindspot else 2,
        "LCA_RIGHT_ICON": 1 if CS.out.rightBlindspot else 2,

        "SOUNDS_2": 0,
      }

      alerts_disable_map = {
        "ALERTS_2": [1, 2, 5],
        "ALERTS_3": [17, 26],
        "ALERTS_5": [1, 4, 5],
      }

      for key, reset_values in alerts_disable_map.items():
        if values.get(key) in reset_values:
          values[key] = 0

      curvature = {
        i: (31 if i == -1 else 13 - abs(i + 15)) if i < 0 else 15 + i
        for i in range(-15, 16)
      }
      values["LANELINE_CURVATURE"] = curvature.get(max(-15, min(round(disp_angle / 3), 15)), 14) if lat_active else 15

      def get_lane_value(depart, visible, frame):
        if depart:
          return 4 if (frame // 50) % 2 == 0 else 1
        return 2 if visible else 0

      values["LANELINE_LEFT"] = get_lane_value(hud.leftLaneDepart, hud.leftLaneVisible, frame)
      values["LANELINE_RIGHT"] = get_lane_value(hud.rightLaneDepart, hud.rightLaneVisible, frame)

      ret.append(packer.make_can_msg("CCNC_0x161", CAN.ECAN, values))

    if frame % 5 == 0 and CS.ccnc_info_162 is not None and ccnc:
      values = CS.ccnc_info_162
      for f in {"FAULT_FCA", "FAULT_LSS", "FAULT_DAS"}:
        values[f] = 0

      #if hud.leadDistance > 0:
      #  values["FF_DISTANCE"] = hud.leadDistance
      #  values["FF_DETECT"] = 4 if hud.leadRelSpeed > -0.1 else 3

      sensors = [
        ('ff', 'FF_DETECT'),
        ('lf', 'LF_DETECT'),
        ('rf', 'RF_DETECT'),
        ('lr', 'LR_DETECT'),
        ('rr', 'RR_DETECT')
      ]

      for sensor_key, detect_key in sensors:
        distance = getattr(CS, f"{sensor_key}_distance")
        if distance > 0:
          values[detect_key] = 3 if distance > 30 else 4

      if hud.leftLaneDepart or hud.rightLaneDepart:
        values["VIBRATE"] = 1

      ret.append(packer.make_can_msg("CCNC_0x162", CAN.ECAN, values))

    #if frame % 2 == 0 and CS.adrv_info_160 is not None:
    #  values = CS.adrv_info_160
    #  values |= {
    #  }
    #  ret.append(packer.make_can_msg("ADRV_0x160", CAN.ECAN, values))

    if frame % 5 == 0 and CS.adrv_info_200 is not None:
      values = CS.adrv_info_200
      values |= {
        "TauGapSet": hud.leadDistanceBars,
      }
      ret.append(packer.make_can_msg("ADRV_0x200", CAN.ECAN, values))

    if frame % 5 == 0 and CS.adrv_info_1ea is not None:
      values = CS.adrv_info_1ea
      values |= {
        "HDA_MODE1": 8,
        "HDA_MODE2": 1,
      }
      ret.append(packer.make_can_msg("ADRV_0x1ea", CAN.ECAN, values))

    if frame % 20 == 0 and CS.hda_info_4a3 is not None:
      values = CS.hda_info_4a3
      values |= {
        "SIGNAL_0": 5,
        "NEW_SIGNAL_1": 4,
        "SPEED_LIMIT": 80,
        "NEW_SIGNAL_3": 154,
        "NEW_SIGNAL_4": 9,
        "NEW_SIGNAL_5": 0,
        "NEW_SIGNAL_6": 256,
      }
      ret.append(packer.make_can_msg("HDA_INFO_4A3", CAN.CAM, values))

    return ret

  else:

    ret.extend(create_fca_warning_light(packer, CP, CAN, frame))
    if frame % 5 == 0:
      values = {
        'HDA_MODE1': 8,
        'HDA_MODE2': 1,
        'SET_ME_FF': 255,
      }
      ret.append(packer.make_can_msg("ADRV_0x1ea", CAN.ECAN, values))

      values = {
        'SET_ME_E1': 225,
        'TauGapSet' : 1,
        'NEW_SIGNAL_2': 3,
      }
      ret.append(packer.make_can_msg("ADRV_0x200", CAN.ECAN, values))

    if frame % 20 == 0:
      values = {
        'SET_ME_15': 21,
      }
      ret.append(packer.make_can_msg("ADRV_0x345", CAN.ECAN, values))

    if frame % 100 == 0:
      values = {
        'SET_ME_22': 34,
        'SET_ME_41': 65,
      }
      ret.append(packer.make_can_msg("ADRV_0x1da", CAN.ECAN, values))

    return ret
