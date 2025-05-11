#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_500250024649247210);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_6015932050118063126);
void car_H_mod_fun(double *state, double *out_256344900191455526);
void car_f_fun(double *state, double dt, double *out_5940344432604237552);
void car_F_fun(double *state, double dt, double *out_8426177587016683373);
void car_h_25(double *state, double *unused, double *out_8699227308135263164);
void car_H_25(double *state, double *unused, double *out_8936843325720781711);
void car_h_24(double *state, double *unused, double *out_8884156725129534728);
void car_H_24(double *state, double *unused, double *out_7337251148983270339);
void car_h_30(double *state, double *unused, double *out_5270445258636095993);
void car_H_30(double *state, double *unused, double *out_4982204417861161707);
void car_h_26(double *state, double *unused, double *out_372284612444860842);
void car_H_26(double *state, double *unused, double *out_5768397429114713681);
void car_h_27(double *state, double *unused, double *out_3540358978093667397);
void car_H_27(double *state, double *unused, double *out_7205798489045104924);
void car_h_29(double *state, double *unused, double *out_7978354806884783161);
void car_H_29(double *state, double *unused, double *out_5492435762175553891);
void car_h_28(double *state, double *unused, double *out_9183088849174624232);
void car_H_28(double *state, double *unused, double *out_7456066033740880142);
void car_h_31(double *state, double *unused, double *out_935962051314590723);
void car_H_31(double *state, double *unused, double *out_8906197363843821283);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}