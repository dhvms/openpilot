import numpy as np
from cereal import car
from openpilot.common.realtime import DT_CTRL
from openpilot.selfdrive.controls.lib.drive_helpers import CONTROL_N
from openpilot.common.pid import PIDController
from openpilot.selfdrive.controls.ntune import ntune_scc_get
from openpilot.selfdrive.modeld.constants import ModelConstants

CONTROL_N_T_IDX = ModelConstants.T_IDXS[:CONTROL_N]
LongCtrlState = car.CarControl.Actuators.LongControlState

def long_control_state_trans(CP, active, long_control_state, v_ego,
                             should_stop, brake_pressed, cruise_standstill, lead):
  stopping_condition = should_stop or cruise_standstill or brake_pressed
  starting_condition = not stopping_condition

  if lead.status:
    starting_condition = starting_condition and lead.vLeadK > 0.2 and lead.dRel > 4.

  started_condition = v_ego > CP.vEgoStarting

  if not active:
    return LongCtrlState.off

  if long_control_state == LongCtrlState.off:
    if not starting_condition:
      return LongCtrlState.stopping
    elif starting_condition and CP.startingState:
      return LongCtrlState.starting
    else:
      return LongCtrlState.pid

  elif long_control_state == LongCtrlState.stopping:
    if starting_condition and CP.startingState:
      return LongCtrlState.starting
    elif starting_condition:
      return LongCtrlState.pid

  elif long_control_state in [LongCtrlState.starting, LongCtrlState.pid]:
    if stopping_condition:
      return LongCtrlState.stopping
    elif started_condition:
      return LongCtrlState.pid

  return long_control_state


class LongControl:
  def __init__(self, CP):
    self.CP = CP
    self.long_control_state = LongCtrlState.off
    self.pid = PIDController(
      (CP.longitudinalTuning.kpBP, CP.longitudinalTuning.kpV),
      (CP.longitudinalTuning.kiBP, CP.longitudinalTuning.kiV),
      k_f=CP.longitudinalTuning.kf,
      rate=1 / DT_CTRL
    )
    self.last_output_accel = 0.0
    self.stopping_accel_weight = 0.0
    self.prev_long_control_state = self.long_control_state

  def reset(self):
    self.pid.reset()

  def update(self, active, CS, long_plan, accel_limits, sm):
    self.pid.neg_limit = accel_limits[0]
    self.pid.pos_limit = accel_limits[1]

    self.prev_long_control_state = self.long_control_state
    self.long_control_state = long_control_state_trans(
      self.CP, active, self.long_control_state, CS.vEgo,
      long_plan.shouldStop, CS.brakePressed,
      CS.cruiseState.standstill, sm['radarState'].leadOne)

    print(f"[DEBUG] long_control_state={self.long_control_state}, vEgo={CS.vEgo:.2f}, aEgo={CS.aEgo:.2f}, brake={CS.brakePressed}, stopReq={long_plan.shouldStop}, standstill={CS.cruiseState.standstill}")

    if self.long_control_state == LongCtrlState.off:
      self.reset()
      output_accel = 0.0
      self.stopping_accel_weight = 0.0

    elif self.long_control_state == LongCtrlState.stopping:
      output_accel = self.last_output_accel
      if output_accel > self.CP.stopAccel:
        output_accel = min(output_accel, 0.0)
        self.stopping_accel_weight = 1.0
        if self.prev_long_control_state == LongCtrlState.starting:
          output_accel -= self.CP.stoppingDecelRate * 1.5 * DT_CTRL
        else:
          m_accel = -0.6
          d_accel = np.interp(
            output_accel,
            [m_accel - 0.5, m_accel, m_accel + 0.5],
            [self.CP.stoppingDecelRate, 0.05, self.CP.stoppingDecelRate]
          )
          output_accel -= d_accel * DT_CTRL
      else:
        self.stopping_accel_weight = 0.0
      self.reset()

    elif self.long_control_state == LongCtrlState.starting:
      output_accel = self.CP.startAccel
      self.reset()
      self.stopping_accel_weight = 0.0

    else:  # LongCtrlState.pid
      error = long_plan.vTarget - CS.vEgo
      a_target_factor = ntune_scc_get('accelTargetFactor') if long_plan.aTarget > 0 else ntune_scc_get('decelTargetFactor')
      ff = long_plan.aTarget * a_target_factor

      output_accel = self.pid.update(error, speed=CS.vEgo, feedforward=ff)

      print(f"[LongControl] vEgo={CS.vEgo:.2f} error={error:.3f} aTarget={long_plan.aTarget:.3f} factor={a_target_factor:.2f} ff={ff:.3f} accelOut={output_accel:.3f}")

      self.stopping_accel_weight = max(self.stopping_accel_weight - 2. * DT_CTRL, 0.)
      output_accel = self.last_output_accel * self.stopping_accel_weight + output_accel * (1. - self.stopping_accel_weight)

    self.last_output_accel = np.clip(output_accel, accel_limits[0], accel_limits[1])
    return self.last_output_accel
