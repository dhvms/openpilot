#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_354310023345513326);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1844677192131749047);
void pose_H_mod_fun(double *state, double *out_803274849067133910);
void pose_f_fun(double *state, double dt, double *out_1058845708616673013);
void pose_F_fun(double *state, double dt, double *out_6637133899774644224);
void pose_h_4(double *state, double *unused, double *out_930779811893977904);
void pose_H_4(double *state, double *unused, double *out_1858787399404689635);
void pose_h_10(double *state, double *unused, double *out_6196868944639979348);
void pose_H_10(double *state, double *unused, double *out_4232608186599470764);
void pose_h_13(double *state, double *unused, double *out_180148709017634934);
void pose_H_13(double *state, double *unused, double *out_1353486425927643166);
void pose_h_14(double *state, double *unused, double *out_8530894024565618123);
void pose_H_14(double *state, double *unused, double *out_2104453456934794894);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}