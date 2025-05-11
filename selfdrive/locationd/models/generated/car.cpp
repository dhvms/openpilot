#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.13.2                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_500250024649247210) {
   out_500250024649247210[0] = delta_x[0] + nom_x[0];
   out_500250024649247210[1] = delta_x[1] + nom_x[1];
   out_500250024649247210[2] = delta_x[2] + nom_x[2];
   out_500250024649247210[3] = delta_x[3] + nom_x[3];
   out_500250024649247210[4] = delta_x[4] + nom_x[4];
   out_500250024649247210[5] = delta_x[5] + nom_x[5];
   out_500250024649247210[6] = delta_x[6] + nom_x[6];
   out_500250024649247210[7] = delta_x[7] + nom_x[7];
   out_500250024649247210[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_6015932050118063126) {
   out_6015932050118063126[0] = -nom_x[0] + true_x[0];
   out_6015932050118063126[1] = -nom_x[1] + true_x[1];
   out_6015932050118063126[2] = -nom_x[2] + true_x[2];
   out_6015932050118063126[3] = -nom_x[3] + true_x[3];
   out_6015932050118063126[4] = -nom_x[4] + true_x[4];
   out_6015932050118063126[5] = -nom_x[5] + true_x[5];
   out_6015932050118063126[6] = -nom_x[6] + true_x[6];
   out_6015932050118063126[7] = -nom_x[7] + true_x[7];
   out_6015932050118063126[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_256344900191455526) {
   out_256344900191455526[0] = 1.0;
   out_256344900191455526[1] = 0.0;
   out_256344900191455526[2] = 0.0;
   out_256344900191455526[3] = 0.0;
   out_256344900191455526[4] = 0.0;
   out_256344900191455526[5] = 0.0;
   out_256344900191455526[6] = 0.0;
   out_256344900191455526[7] = 0.0;
   out_256344900191455526[8] = 0.0;
   out_256344900191455526[9] = 0.0;
   out_256344900191455526[10] = 1.0;
   out_256344900191455526[11] = 0.0;
   out_256344900191455526[12] = 0.0;
   out_256344900191455526[13] = 0.0;
   out_256344900191455526[14] = 0.0;
   out_256344900191455526[15] = 0.0;
   out_256344900191455526[16] = 0.0;
   out_256344900191455526[17] = 0.0;
   out_256344900191455526[18] = 0.0;
   out_256344900191455526[19] = 0.0;
   out_256344900191455526[20] = 1.0;
   out_256344900191455526[21] = 0.0;
   out_256344900191455526[22] = 0.0;
   out_256344900191455526[23] = 0.0;
   out_256344900191455526[24] = 0.0;
   out_256344900191455526[25] = 0.0;
   out_256344900191455526[26] = 0.0;
   out_256344900191455526[27] = 0.0;
   out_256344900191455526[28] = 0.0;
   out_256344900191455526[29] = 0.0;
   out_256344900191455526[30] = 1.0;
   out_256344900191455526[31] = 0.0;
   out_256344900191455526[32] = 0.0;
   out_256344900191455526[33] = 0.0;
   out_256344900191455526[34] = 0.0;
   out_256344900191455526[35] = 0.0;
   out_256344900191455526[36] = 0.0;
   out_256344900191455526[37] = 0.0;
   out_256344900191455526[38] = 0.0;
   out_256344900191455526[39] = 0.0;
   out_256344900191455526[40] = 1.0;
   out_256344900191455526[41] = 0.0;
   out_256344900191455526[42] = 0.0;
   out_256344900191455526[43] = 0.0;
   out_256344900191455526[44] = 0.0;
   out_256344900191455526[45] = 0.0;
   out_256344900191455526[46] = 0.0;
   out_256344900191455526[47] = 0.0;
   out_256344900191455526[48] = 0.0;
   out_256344900191455526[49] = 0.0;
   out_256344900191455526[50] = 1.0;
   out_256344900191455526[51] = 0.0;
   out_256344900191455526[52] = 0.0;
   out_256344900191455526[53] = 0.0;
   out_256344900191455526[54] = 0.0;
   out_256344900191455526[55] = 0.0;
   out_256344900191455526[56] = 0.0;
   out_256344900191455526[57] = 0.0;
   out_256344900191455526[58] = 0.0;
   out_256344900191455526[59] = 0.0;
   out_256344900191455526[60] = 1.0;
   out_256344900191455526[61] = 0.0;
   out_256344900191455526[62] = 0.0;
   out_256344900191455526[63] = 0.0;
   out_256344900191455526[64] = 0.0;
   out_256344900191455526[65] = 0.0;
   out_256344900191455526[66] = 0.0;
   out_256344900191455526[67] = 0.0;
   out_256344900191455526[68] = 0.0;
   out_256344900191455526[69] = 0.0;
   out_256344900191455526[70] = 1.0;
   out_256344900191455526[71] = 0.0;
   out_256344900191455526[72] = 0.0;
   out_256344900191455526[73] = 0.0;
   out_256344900191455526[74] = 0.0;
   out_256344900191455526[75] = 0.0;
   out_256344900191455526[76] = 0.0;
   out_256344900191455526[77] = 0.0;
   out_256344900191455526[78] = 0.0;
   out_256344900191455526[79] = 0.0;
   out_256344900191455526[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_5940344432604237552) {
   out_5940344432604237552[0] = state[0];
   out_5940344432604237552[1] = state[1];
   out_5940344432604237552[2] = state[2];
   out_5940344432604237552[3] = state[3];
   out_5940344432604237552[4] = state[4];
   out_5940344432604237552[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_5940344432604237552[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_5940344432604237552[7] = state[7];
   out_5940344432604237552[8] = state[8];
}
void F_fun(double *state, double dt, double *out_8426177587016683373) {
   out_8426177587016683373[0] = 1;
   out_8426177587016683373[1] = 0;
   out_8426177587016683373[2] = 0;
   out_8426177587016683373[3] = 0;
   out_8426177587016683373[4] = 0;
   out_8426177587016683373[5] = 0;
   out_8426177587016683373[6] = 0;
   out_8426177587016683373[7] = 0;
   out_8426177587016683373[8] = 0;
   out_8426177587016683373[9] = 0;
   out_8426177587016683373[10] = 1;
   out_8426177587016683373[11] = 0;
   out_8426177587016683373[12] = 0;
   out_8426177587016683373[13] = 0;
   out_8426177587016683373[14] = 0;
   out_8426177587016683373[15] = 0;
   out_8426177587016683373[16] = 0;
   out_8426177587016683373[17] = 0;
   out_8426177587016683373[18] = 0;
   out_8426177587016683373[19] = 0;
   out_8426177587016683373[20] = 1;
   out_8426177587016683373[21] = 0;
   out_8426177587016683373[22] = 0;
   out_8426177587016683373[23] = 0;
   out_8426177587016683373[24] = 0;
   out_8426177587016683373[25] = 0;
   out_8426177587016683373[26] = 0;
   out_8426177587016683373[27] = 0;
   out_8426177587016683373[28] = 0;
   out_8426177587016683373[29] = 0;
   out_8426177587016683373[30] = 1;
   out_8426177587016683373[31] = 0;
   out_8426177587016683373[32] = 0;
   out_8426177587016683373[33] = 0;
   out_8426177587016683373[34] = 0;
   out_8426177587016683373[35] = 0;
   out_8426177587016683373[36] = 0;
   out_8426177587016683373[37] = 0;
   out_8426177587016683373[38] = 0;
   out_8426177587016683373[39] = 0;
   out_8426177587016683373[40] = 1;
   out_8426177587016683373[41] = 0;
   out_8426177587016683373[42] = 0;
   out_8426177587016683373[43] = 0;
   out_8426177587016683373[44] = 0;
   out_8426177587016683373[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_8426177587016683373[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_8426177587016683373[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8426177587016683373[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_8426177587016683373[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_8426177587016683373[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_8426177587016683373[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_8426177587016683373[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_8426177587016683373[53] = -9.8000000000000007*dt;
   out_8426177587016683373[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_8426177587016683373[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_8426177587016683373[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8426177587016683373[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8426177587016683373[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_8426177587016683373[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_8426177587016683373[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_8426177587016683373[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_8426177587016683373[62] = 0;
   out_8426177587016683373[63] = 0;
   out_8426177587016683373[64] = 0;
   out_8426177587016683373[65] = 0;
   out_8426177587016683373[66] = 0;
   out_8426177587016683373[67] = 0;
   out_8426177587016683373[68] = 0;
   out_8426177587016683373[69] = 0;
   out_8426177587016683373[70] = 1;
   out_8426177587016683373[71] = 0;
   out_8426177587016683373[72] = 0;
   out_8426177587016683373[73] = 0;
   out_8426177587016683373[74] = 0;
   out_8426177587016683373[75] = 0;
   out_8426177587016683373[76] = 0;
   out_8426177587016683373[77] = 0;
   out_8426177587016683373[78] = 0;
   out_8426177587016683373[79] = 0;
   out_8426177587016683373[80] = 1;
}
void h_25(double *state, double *unused, double *out_8699227308135263164) {
   out_8699227308135263164[0] = state[6];
}
void H_25(double *state, double *unused, double *out_8936843325720781711) {
   out_8936843325720781711[0] = 0;
   out_8936843325720781711[1] = 0;
   out_8936843325720781711[2] = 0;
   out_8936843325720781711[3] = 0;
   out_8936843325720781711[4] = 0;
   out_8936843325720781711[5] = 0;
   out_8936843325720781711[6] = 1;
   out_8936843325720781711[7] = 0;
   out_8936843325720781711[8] = 0;
}
void h_24(double *state, double *unused, double *out_8884156725129534728) {
   out_8884156725129534728[0] = state[4];
   out_8884156725129534728[1] = state[5];
}
void H_24(double *state, double *unused, double *out_7337251148983270339) {
   out_7337251148983270339[0] = 0;
   out_7337251148983270339[1] = 0;
   out_7337251148983270339[2] = 0;
   out_7337251148983270339[3] = 0;
   out_7337251148983270339[4] = 1;
   out_7337251148983270339[5] = 0;
   out_7337251148983270339[6] = 0;
   out_7337251148983270339[7] = 0;
   out_7337251148983270339[8] = 0;
   out_7337251148983270339[9] = 0;
   out_7337251148983270339[10] = 0;
   out_7337251148983270339[11] = 0;
   out_7337251148983270339[12] = 0;
   out_7337251148983270339[13] = 0;
   out_7337251148983270339[14] = 1;
   out_7337251148983270339[15] = 0;
   out_7337251148983270339[16] = 0;
   out_7337251148983270339[17] = 0;
}
void h_30(double *state, double *unused, double *out_5270445258636095993) {
   out_5270445258636095993[0] = state[4];
}
void H_30(double *state, double *unused, double *out_4982204417861161707) {
   out_4982204417861161707[0] = 0;
   out_4982204417861161707[1] = 0;
   out_4982204417861161707[2] = 0;
   out_4982204417861161707[3] = 0;
   out_4982204417861161707[4] = 1;
   out_4982204417861161707[5] = 0;
   out_4982204417861161707[6] = 0;
   out_4982204417861161707[7] = 0;
   out_4982204417861161707[8] = 0;
}
void h_26(double *state, double *unused, double *out_372284612444860842) {
   out_372284612444860842[0] = state[7];
}
void H_26(double *state, double *unused, double *out_5768397429114713681) {
   out_5768397429114713681[0] = 0;
   out_5768397429114713681[1] = 0;
   out_5768397429114713681[2] = 0;
   out_5768397429114713681[3] = 0;
   out_5768397429114713681[4] = 0;
   out_5768397429114713681[5] = 0;
   out_5768397429114713681[6] = 0;
   out_5768397429114713681[7] = 1;
   out_5768397429114713681[8] = 0;
}
void h_27(double *state, double *unused, double *out_3540358978093667397) {
   out_3540358978093667397[0] = state[3];
}
void H_27(double *state, double *unused, double *out_7205798489045104924) {
   out_7205798489045104924[0] = 0;
   out_7205798489045104924[1] = 0;
   out_7205798489045104924[2] = 0;
   out_7205798489045104924[3] = 1;
   out_7205798489045104924[4] = 0;
   out_7205798489045104924[5] = 0;
   out_7205798489045104924[6] = 0;
   out_7205798489045104924[7] = 0;
   out_7205798489045104924[8] = 0;
}
void h_29(double *state, double *unused, double *out_7978354806884783161) {
   out_7978354806884783161[0] = state[1];
}
void H_29(double *state, double *unused, double *out_5492435762175553891) {
   out_5492435762175553891[0] = 0;
   out_5492435762175553891[1] = 1;
   out_5492435762175553891[2] = 0;
   out_5492435762175553891[3] = 0;
   out_5492435762175553891[4] = 0;
   out_5492435762175553891[5] = 0;
   out_5492435762175553891[6] = 0;
   out_5492435762175553891[7] = 0;
   out_5492435762175553891[8] = 0;
}
void h_28(double *state, double *unused, double *out_9183088849174624232) {
   out_9183088849174624232[0] = state[0];
}
void H_28(double *state, double *unused, double *out_7456066033740880142) {
   out_7456066033740880142[0] = 1;
   out_7456066033740880142[1] = 0;
   out_7456066033740880142[2] = 0;
   out_7456066033740880142[3] = 0;
   out_7456066033740880142[4] = 0;
   out_7456066033740880142[5] = 0;
   out_7456066033740880142[6] = 0;
   out_7456066033740880142[7] = 0;
   out_7456066033740880142[8] = 0;
}
void h_31(double *state, double *unused, double *out_935962051314590723) {
   out_935962051314590723[0] = state[8];
}
void H_31(double *state, double *unused, double *out_8906197363843821283) {
   out_8906197363843821283[0] = 0;
   out_8906197363843821283[1] = 0;
   out_8906197363843821283[2] = 0;
   out_8906197363843821283[3] = 0;
   out_8906197363843821283[4] = 0;
   out_8906197363843821283[5] = 0;
   out_8906197363843821283[6] = 0;
   out_8906197363843821283[7] = 0;
   out_8906197363843821283[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_500250024649247210) {
  err_fun(nom_x, delta_x, out_500250024649247210);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6015932050118063126) {
  inv_err_fun(nom_x, true_x, out_6015932050118063126);
}
void car_H_mod_fun(double *state, double *out_256344900191455526) {
  H_mod_fun(state, out_256344900191455526);
}
void car_f_fun(double *state, double dt, double *out_5940344432604237552) {
  f_fun(state,  dt, out_5940344432604237552);
}
void car_F_fun(double *state, double dt, double *out_8426177587016683373) {
  F_fun(state,  dt, out_8426177587016683373);
}
void car_h_25(double *state, double *unused, double *out_8699227308135263164) {
  h_25(state, unused, out_8699227308135263164);
}
void car_H_25(double *state, double *unused, double *out_8936843325720781711) {
  H_25(state, unused, out_8936843325720781711);
}
void car_h_24(double *state, double *unused, double *out_8884156725129534728) {
  h_24(state, unused, out_8884156725129534728);
}
void car_H_24(double *state, double *unused, double *out_7337251148983270339) {
  H_24(state, unused, out_7337251148983270339);
}
void car_h_30(double *state, double *unused, double *out_5270445258636095993) {
  h_30(state, unused, out_5270445258636095993);
}
void car_H_30(double *state, double *unused, double *out_4982204417861161707) {
  H_30(state, unused, out_4982204417861161707);
}
void car_h_26(double *state, double *unused, double *out_372284612444860842) {
  h_26(state, unused, out_372284612444860842);
}
void car_H_26(double *state, double *unused, double *out_5768397429114713681) {
  H_26(state, unused, out_5768397429114713681);
}
void car_h_27(double *state, double *unused, double *out_3540358978093667397) {
  h_27(state, unused, out_3540358978093667397);
}
void car_H_27(double *state, double *unused, double *out_7205798489045104924) {
  H_27(state, unused, out_7205798489045104924);
}
void car_h_29(double *state, double *unused, double *out_7978354806884783161) {
  h_29(state, unused, out_7978354806884783161);
}
void car_H_29(double *state, double *unused, double *out_5492435762175553891) {
  H_29(state, unused, out_5492435762175553891);
}
void car_h_28(double *state, double *unused, double *out_9183088849174624232) {
  h_28(state, unused, out_9183088849174624232);
}
void car_H_28(double *state, double *unused, double *out_7456066033740880142) {
  H_28(state, unused, out_7456066033740880142);
}
void car_h_31(double *state, double *unused, double *out_935962051314590723) {
  h_31(state, unused, out_935962051314590723);
}
void car_H_31(double *state, double *unused, double *out_8906197363843821283) {
  H_31(state, unused, out_8906197363843821283);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
