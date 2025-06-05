from opendbc_repo.opendbc.car.hyundai.values import CAMERA_SCC_CAR
from opendbc.car import Bus, get_safety_config, structs
from opendbc.car.hyundai.hyundaicanfd import CanBus
from opendbc.car.hyundai.values import HyundaiFlags, CAR, DBC, \
  CANFD_UNSUPPORTED_LONGITUDINAL_CAR, \
  UNSUPPORTED_LONGITUDINAL_CAR, HyundaiSafetyFlags, Buttons, CANFD_RADAR_SCC_CAR
from opendbc.car.hyundai.radar_interface import RADAR_START_ADDR
from opendbc.car.interfaces import CarInterfaceBase, ACCEL_MIN, ACCEL_MAX
from opendbc.car.disable_ecu import disable_ecu
from opendbc.car.hyundai.carcontroller import CarController
from opendbc.car.hyundai.carstate import CarState
from opendbc.car.hyundai.radar_interface import RadarInterface

from openpilot.common.conversions import Conversions as CV
from openpilot.selfdrive.controls.neokii.cruise_state_manager import is_radar_disabler
from openpilot.common.params import Params
from opendbc.car.hyundai.values import HyundaiExFlags
from common.numpy_fast import interp
from selfdrive.ntune import ntune_common_get
import copy

ButtonType = structs.CarState.ButtonEvent.Type
Ecu = structs.CarParams.Ecu

ENABLE_BUTTONS = (ButtonType.accelCruise, ButtonType.decelCruise, ButtonType.cancel, ButtonType.mainCruise)

BUTTONS_DICT = {Buttons.RES_ACCEL: ButtonType.accelCruise, Buttons.SET_DECEL: ButtonType.decelCruise,
                Buttons.GAP_DIST: ButtonType.gapAdjustCruise, Buttons.CANCEL: ButtonType.cancel}


class CarInterface(CarInterfaceBase):
  CarState = CarState
  CarController = CarController
  RadarInterface = RadarInterface

  @staticmethod
  def get_pid_accel_limits(CP, current_speed, cruise_speed):
    v_current_kph = current_speed * CV.MS_TO_KPH
    gas_max_bp = [0., 10., 30., 70., 130., 150.]
    gas_max_v = [ACCEL_MAX, 1.5, 1.0, 0.5, 0.15, 0.1]
    return ACCEL_MIN, interp(v_current_kph, gas_max_bp, gas_max_v)

  @staticmethod
  def _get_params(ret: structs.CarParams, candidate, fingerprint, car_fw, alpha_long, docs) -> structs.CarParams:
    ret.brand = "hyundai"

    cam_can = CanBus(None, fingerprint).CAM
    lka_steering = 0x50 in fingerprint[cam_can] or 0x110 in fingerprint[cam_can] or Params().get_bool('CanFdHda2')
    CAN = CanBus(None, fingerprint, lka_steering)

    if ret.flags & HyundaiFlags.CANFD:
      ret.alphaLongitudinalAvailable = candidate not in CANFD_UNSUPPORTED_LONGITUDINAL_CAR
      if lka_steering and Ecu.adas not in [fw.ecu for fw in car_fw]:
        ret.alphaLongitudinalAvailable = False

      ret.enableBsm = 0x1e5 in fingerprint[CAN.ECAN]

      if 0x105 in fingerprint[CAN.ECAN]:
        ret.flags |= HyundaiFlags.HYBRID.value

      if lka_steering:
        ret.flags |= HyundaiFlags.CANFD_LKA_STEERING.value
        if 0x110 in fingerprint[CAN.CAM]:
          ret.flags |= HyundaiFlags.CANFD_LKA_STEERING_ALT.value
      else:
        if candidate not in CANFD_RADAR_SCC_CAR:
          ret.flags |= HyundaiFlags.CANFD_CAMERA_SCC.value

      if 0x1cf not in fingerprint[CAN.ECAN]:
        ret.flags |= HyundaiFlags.CANFD_ALT_BUTTONS.value

      if 0x130 not in fingerprint[CAN.ECAN]:
        if 0x40 not in fingerprint[CAN.ECAN]:
          ret.flags |= HyundaiFlags.CANFD_ALT_GEARS_2.value
        else:
          ret.flags |= HyundaiFlags.CANFD_ALT_GEARS.value

    ret.radarUnavailable = RADAR_START_ADDR not in fingerprint[1] or Bus.radar not in DBC[ret.carFingerprint]
    ret.steerActuatorDelay = ntune_common_get("steerActuatorDelay", default=0.2)
    ret.steerLimitTimer = 0.4
    CarInterfaceBase.configure_torque_tune(candidate, ret.lateralTuning)

    if ret.flags & HyundaiFlags.CANFD:
      ret.longitudinalTuning.kpBP = [0.]
      ret.longitudinalTuning.kpV = [0.8]
      ret.longitudinalTuning.kf = 0.5
      ret.alphaLongitudinalAvailable = candidate not in (CANFD_UNSUPPORTED_LONGITUDINAL_CAR | CANFD_RADAR_SCC_CAR)
    else:
      ret.longitudinalTuning.kpBP = [0.]
      ret.longitudinalTuning.kpV = [0.9]
      ret.longitudinalTuning.kf = 0.5
      ret.alphaLongitudinalAvailable = True

    ret.openpilotLongitudinalControl = alpha_long and ret.alphaLongitudinalAvailable
    ret.pcmCruise = not ret.openpilotLongitudinalControl

    ret.startingState = True
    ret.stoppingDecelRate = 0.3
    ret.steerLimitTimer = 2.0
    ret.vEgoStarting = 0.1
    ret.vEgoStopping = 0.1
    ret.startAccel = 1.0
    ret.longitudinalActuatorDelay = 0.5

    if ret.flags & HyundaiFlags.CANFD:
      ret.enableBsm = 0x1e5 in fingerprint[CAN.ECAN]
    else:
      ret.enableBsm = 0x58b in fingerprint[0]

    ret.sccBus = 2 if (candidate in CAMERA_SCC_CAR or Params().get_bool('SccOnBus2')) else 0

    # 이후 코드는 그대로 유지 (생략)

    return ret
