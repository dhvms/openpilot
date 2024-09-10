#!/usr/bin/env python3
# The MIT License
#
# Copyright (c) 2019-, Rick Lan, dragonpilot community, and a number of other of contributors.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# Last update: June 5, 2024

from common.numpy_fast import interp

DP_ACCEL_STOCK = 0
DP_ACCEL_ECO = 1
DP_ACCEL_NORMAL = 2
DP_ACCEL_SPORT = 3

# accel profile by @arne182 modified by cgw
_DP_CRUISE_MIN_V =       [-0.15, -0.15, -0.06, -0.06, -0.06, -0.06, -0.23, -0.23, -0.63, -0.63, -0.79, -0.79, -1.0,  -1.0]
_DP_CRUISE_MIN_V_ECO =    [-0.10, -0.10, -0.07, -0.07, -0.05, -0.05, -0.22, -0.22, -0.64, -0.64, -0.78, -0.78, -1.0,  -1.0]
_DP_CRUISE_MIN_V_SPORT =  [-0.20, -0.20, -0.09, -0.09, -0.07, -0.07, -0.24, -0.24, -0.66, -0.66, -0.80, -0.80, -1.0,  -1.0]
_DP_CRUISE_MIN_BP =      [0.,     3.0,    3.01,   8.0,    8.01,  12.,   12.01, 16.,   16.01, 20.,   20.01,  25.,  25.01, 30.]

_DP_CRUISE_MAX_V =       [2.0, 2.0, 1.75, 1.14, .64,  .54,  .38,  .17]
_DP_CRUISE_MAX_V_ECO =   [2.0, 2.0, 1.60, 0.91, .56,  .45,  .32,  .09]
_DP_CRUISE_MAX_V_SPORT = [2.0, 2.0, 2.00, 1.35, .84,  .70,  .50,  .30]
_DP_CRUISE_MAX_BP =      [0.,  6.0,  8.,   11.,  20.,  25.,  30.,  40.]


class AccelController:

    def __init__(self):
        # self._params = Params()
        self._profile = DP_ACCEL_STOCK

    def set_profile(self, profile):
        try:
            self._profile = int(profile) if int(profile) in [DP_ACCEL_STOCK, DP_ACCEL_ECO, DP_ACCEL_NORMAL, DP_ACCEL_SPORT] else DP_ACCEL_STOCK
        except:
            self._profile = DP_ACCEL_STOCK

    def _dp_calc_cruise_accel_limits(self, v_ego):
        if self._profile == DP_ACCEL_ECO:
            min_v = _DP_CRUISE_MIN_V_ECO
            max_v = _DP_CRUISE_MAX_V_ECO
        elif self._profile == DP_ACCEL_SPORT:
            min_v = _DP_CRUISE_MIN_V_SPORT
            max_v = _DP_CRUISE_MAX_V_SPORT
        else:
            min_v = _DP_CRUISE_MIN_V
            max_v = _DP_CRUISE_MAX_V

        a_cruise_min = interp(v_ego, _DP_CRUISE_MIN_BP, min_v)
        a_cruise_max = interp(v_ego, _DP_CRUISE_MAX_BP, max_v)
        return a_cruise_min, a_cruise_max

    def get_accel_limits(self, v_ego, accel_limits):
        return accel_limits if self._profile == DP_ACCEL_STOCK else self._dp_calc_cruise_accel_limits(v_ego)

    def is_enabled(self):
        return self._profile != DP_ACCEL_STOCK
