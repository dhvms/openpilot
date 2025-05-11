#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.13.2                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_354310023345513326) {
   out_354310023345513326[0] = delta_x[0] + nom_x[0];
   out_354310023345513326[1] = delta_x[1] + nom_x[1];
   out_354310023345513326[2] = delta_x[2] + nom_x[2];
   out_354310023345513326[3] = delta_x[3] + nom_x[3];
   out_354310023345513326[4] = delta_x[4] + nom_x[4];
   out_354310023345513326[5] = delta_x[5] + nom_x[5];
   out_354310023345513326[6] = delta_x[6] + nom_x[6];
   out_354310023345513326[7] = delta_x[7] + nom_x[7];
   out_354310023345513326[8] = delta_x[8] + nom_x[8];
   out_354310023345513326[9] = delta_x[9] + nom_x[9];
   out_354310023345513326[10] = delta_x[10] + nom_x[10];
   out_354310023345513326[11] = delta_x[11] + nom_x[11];
   out_354310023345513326[12] = delta_x[12] + nom_x[12];
   out_354310023345513326[13] = delta_x[13] + nom_x[13];
   out_354310023345513326[14] = delta_x[14] + nom_x[14];
   out_354310023345513326[15] = delta_x[15] + nom_x[15];
   out_354310023345513326[16] = delta_x[16] + nom_x[16];
   out_354310023345513326[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_1844677192131749047) {
   out_1844677192131749047[0] = -nom_x[0] + true_x[0];
   out_1844677192131749047[1] = -nom_x[1] + true_x[1];
   out_1844677192131749047[2] = -nom_x[2] + true_x[2];
   out_1844677192131749047[3] = -nom_x[3] + true_x[3];
   out_1844677192131749047[4] = -nom_x[4] + true_x[4];
   out_1844677192131749047[5] = -nom_x[5] + true_x[5];
   out_1844677192131749047[6] = -nom_x[6] + true_x[6];
   out_1844677192131749047[7] = -nom_x[7] + true_x[7];
   out_1844677192131749047[8] = -nom_x[8] + true_x[8];
   out_1844677192131749047[9] = -nom_x[9] + true_x[9];
   out_1844677192131749047[10] = -nom_x[10] + true_x[10];
   out_1844677192131749047[11] = -nom_x[11] + true_x[11];
   out_1844677192131749047[12] = -nom_x[12] + true_x[12];
   out_1844677192131749047[13] = -nom_x[13] + true_x[13];
   out_1844677192131749047[14] = -nom_x[14] + true_x[14];
   out_1844677192131749047[15] = -nom_x[15] + true_x[15];
   out_1844677192131749047[16] = -nom_x[16] + true_x[16];
   out_1844677192131749047[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_803274849067133910) {
   out_803274849067133910[0] = 1.0;
   out_803274849067133910[1] = 0.0;
   out_803274849067133910[2] = 0.0;
   out_803274849067133910[3] = 0.0;
   out_803274849067133910[4] = 0.0;
   out_803274849067133910[5] = 0.0;
   out_803274849067133910[6] = 0.0;
   out_803274849067133910[7] = 0.0;
   out_803274849067133910[8] = 0.0;
   out_803274849067133910[9] = 0.0;
   out_803274849067133910[10] = 0.0;
   out_803274849067133910[11] = 0.0;
   out_803274849067133910[12] = 0.0;
   out_803274849067133910[13] = 0.0;
   out_803274849067133910[14] = 0.0;
   out_803274849067133910[15] = 0.0;
   out_803274849067133910[16] = 0.0;
   out_803274849067133910[17] = 0.0;
   out_803274849067133910[18] = 0.0;
   out_803274849067133910[19] = 1.0;
   out_803274849067133910[20] = 0.0;
   out_803274849067133910[21] = 0.0;
   out_803274849067133910[22] = 0.0;
   out_803274849067133910[23] = 0.0;
   out_803274849067133910[24] = 0.0;
   out_803274849067133910[25] = 0.0;
   out_803274849067133910[26] = 0.0;
   out_803274849067133910[27] = 0.0;
   out_803274849067133910[28] = 0.0;
   out_803274849067133910[29] = 0.0;
   out_803274849067133910[30] = 0.0;
   out_803274849067133910[31] = 0.0;
   out_803274849067133910[32] = 0.0;
   out_803274849067133910[33] = 0.0;
   out_803274849067133910[34] = 0.0;
   out_803274849067133910[35] = 0.0;
   out_803274849067133910[36] = 0.0;
   out_803274849067133910[37] = 0.0;
   out_803274849067133910[38] = 1.0;
   out_803274849067133910[39] = 0.0;
   out_803274849067133910[40] = 0.0;
   out_803274849067133910[41] = 0.0;
   out_803274849067133910[42] = 0.0;
   out_803274849067133910[43] = 0.0;
   out_803274849067133910[44] = 0.0;
   out_803274849067133910[45] = 0.0;
   out_803274849067133910[46] = 0.0;
   out_803274849067133910[47] = 0.0;
   out_803274849067133910[48] = 0.0;
   out_803274849067133910[49] = 0.0;
   out_803274849067133910[50] = 0.0;
   out_803274849067133910[51] = 0.0;
   out_803274849067133910[52] = 0.0;
   out_803274849067133910[53] = 0.0;
   out_803274849067133910[54] = 0.0;
   out_803274849067133910[55] = 0.0;
   out_803274849067133910[56] = 0.0;
   out_803274849067133910[57] = 1.0;
   out_803274849067133910[58] = 0.0;
   out_803274849067133910[59] = 0.0;
   out_803274849067133910[60] = 0.0;
   out_803274849067133910[61] = 0.0;
   out_803274849067133910[62] = 0.0;
   out_803274849067133910[63] = 0.0;
   out_803274849067133910[64] = 0.0;
   out_803274849067133910[65] = 0.0;
   out_803274849067133910[66] = 0.0;
   out_803274849067133910[67] = 0.0;
   out_803274849067133910[68] = 0.0;
   out_803274849067133910[69] = 0.0;
   out_803274849067133910[70] = 0.0;
   out_803274849067133910[71] = 0.0;
   out_803274849067133910[72] = 0.0;
   out_803274849067133910[73] = 0.0;
   out_803274849067133910[74] = 0.0;
   out_803274849067133910[75] = 0.0;
   out_803274849067133910[76] = 1.0;
   out_803274849067133910[77] = 0.0;
   out_803274849067133910[78] = 0.0;
   out_803274849067133910[79] = 0.0;
   out_803274849067133910[80] = 0.0;
   out_803274849067133910[81] = 0.0;
   out_803274849067133910[82] = 0.0;
   out_803274849067133910[83] = 0.0;
   out_803274849067133910[84] = 0.0;
   out_803274849067133910[85] = 0.0;
   out_803274849067133910[86] = 0.0;
   out_803274849067133910[87] = 0.0;
   out_803274849067133910[88] = 0.0;
   out_803274849067133910[89] = 0.0;
   out_803274849067133910[90] = 0.0;
   out_803274849067133910[91] = 0.0;
   out_803274849067133910[92] = 0.0;
   out_803274849067133910[93] = 0.0;
   out_803274849067133910[94] = 0.0;
   out_803274849067133910[95] = 1.0;
   out_803274849067133910[96] = 0.0;
   out_803274849067133910[97] = 0.0;
   out_803274849067133910[98] = 0.0;
   out_803274849067133910[99] = 0.0;
   out_803274849067133910[100] = 0.0;
   out_803274849067133910[101] = 0.0;
   out_803274849067133910[102] = 0.0;
   out_803274849067133910[103] = 0.0;
   out_803274849067133910[104] = 0.0;
   out_803274849067133910[105] = 0.0;
   out_803274849067133910[106] = 0.0;
   out_803274849067133910[107] = 0.0;
   out_803274849067133910[108] = 0.0;
   out_803274849067133910[109] = 0.0;
   out_803274849067133910[110] = 0.0;
   out_803274849067133910[111] = 0.0;
   out_803274849067133910[112] = 0.0;
   out_803274849067133910[113] = 0.0;
   out_803274849067133910[114] = 1.0;
   out_803274849067133910[115] = 0.0;
   out_803274849067133910[116] = 0.0;
   out_803274849067133910[117] = 0.0;
   out_803274849067133910[118] = 0.0;
   out_803274849067133910[119] = 0.0;
   out_803274849067133910[120] = 0.0;
   out_803274849067133910[121] = 0.0;
   out_803274849067133910[122] = 0.0;
   out_803274849067133910[123] = 0.0;
   out_803274849067133910[124] = 0.0;
   out_803274849067133910[125] = 0.0;
   out_803274849067133910[126] = 0.0;
   out_803274849067133910[127] = 0.0;
   out_803274849067133910[128] = 0.0;
   out_803274849067133910[129] = 0.0;
   out_803274849067133910[130] = 0.0;
   out_803274849067133910[131] = 0.0;
   out_803274849067133910[132] = 0.0;
   out_803274849067133910[133] = 1.0;
   out_803274849067133910[134] = 0.0;
   out_803274849067133910[135] = 0.0;
   out_803274849067133910[136] = 0.0;
   out_803274849067133910[137] = 0.0;
   out_803274849067133910[138] = 0.0;
   out_803274849067133910[139] = 0.0;
   out_803274849067133910[140] = 0.0;
   out_803274849067133910[141] = 0.0;
   out_803274849067133910[142] = 0.0;
   out_803274849067133910[143] = 0.0;
   out_803274849067133910[144] = 0.0;
   out_803274849067133910[145] = 0.0;
   out_803274849067133910[146] = 0.0;
   out_803274849067133910[147] = 0.0;
   out_803274849067133910[148] = 0.0;
   out_803274849067133910[149] = 0.0;
   out_803274849067133910[150] = 0.0;
   out_803274849067133910[151] = 0.0;
   out_803274849067133910[152] = 1.0;
   out_803274849067133910[153] = 0.0;
   out_803274849067133910[154] = 0.0;
   out_803274849067133910[155] = 0.0;
   out_803274849067133910[156] = 0.0;
   out_803274849067133910[157] = 0.0;
   out_803274849067133910[158] = 0.0;
   out_803274849067133910[159] = 0.0;
   out_803274849067133910[160] = 0.0;
   out_803274849067133910[161] = 0.0;
   out_803274849067133910[162] = 0.0;
   out_803274849067133910[163] = 0.0;
   out_803274849067133910[164] = 0.0;
   out_803274849067133910[165] = 0.0;
   out_803274849067133910[166] = 0.0;
   out_803274849067133910[167] = 0.0;
   out_803274849067133910[168] = 0.0;
   out_803274849067133910[169] = 0.0;
   out_803274849067133910[170] = 0.0;
   out_803274849067133910[171] = 1.0;
   out_803274849067133910[172] = 0.0;
   out_803274849067133910[173] = 0.0;
   out_803274849067133910[174] = 0.0;
   out_803274849067133910[175] = 0.0;
   out_803274849067133910[176] = 0.0;
   out_803274849067133910[177] = 0.0;
   out_803274849067133910[178] = 0.0;
   out_803274849067133910[179] = 0.0;
   out_803274849067133910[180] = 0.0;
   out_803274849067133910[181] = 0.0;
   out_803274849067133910[182] = 0.0;
   out_803274849067133910[183] = 0.0;
   out_803274849067133910[184] = 0.0;
   out_803274849067133910[185] = 0.0;
   out_803274849067133910[186] = 0.0;
   out_803274849067133910[187] = 0.0;
   out_803274849067133910[188] = 0.0;
   out_803274849067133910[189] = 0.0;
   out_803274849067133910[190] = 1.0;
   out_803274849067133910[191] = 0.0;
   out_803274849067133910[192] = 0.0;
   out_803274849067133910[193] = 0.0;
   out_803274849067133910[194] = 0.0;
   out_803274849067133910[195] = 0.0;
   out_803274849067133910[196] = 0.0;
   out_803274849067133910[197] = 0.0;
   out_803274849067133910[198] = 0.0;
   out_803274849067133910[199] = 0.0;
   out_803274849067133910[200] = 0.0;
   out_803274849067133910[201] = 0.0;
   out_803274849067133910[202] = 0.0;
   out_803274849067133910[203] = 0.0;
   out_803274849067133910[204] = 0.0;
   out_803274849067133910[205] = 0.0;
   out_803274849067133910[206] = 0.0;
   out_803274849067133910[207] = 0.0;
   out_803274849067133910[208] = 0.0;
   out_803274849067133910[209] = 1.0;
   out_803274849067133910[210] = 0.0;
   out_803274849067133910[211] = 0.0;
   out_803274849067133910[212] = 0.0;
   out_803274849067133910[213] = 0.0;
   out_803274849067133910[214] = 0.0;
   out_803274849067133910[215] = 0.0;
   out_803274849067133910[216] = 0.0;
   out_803274849067133910[217] = 0.0;
   out_803274849067133910[218] = 0.0;
   out_803274849067133910[219] = 0.0;
   out_803274849067133910[220] = 0.0;
   out_803274849067133910[221] = 0.0;
   out_803274849067133910[222] = 0.0;
   out_803274849067133910[223] = 0.0;
   out_803274849067133910[224] = 0.0;
   out_803274849067133910[225] = 0.0;
   out_803274849067133910[226] = 0.0;
   out_803274849067133910[227] = 0.0;
   out_803274849067133910[228] = 1.0;
   out_803274849067133910[229] = 0.0;
   out_803274849067133910[230] = 0.0;
   out_803274849067133910[231] = 0.0;
   out_803274849067133910[232] = 0.0;
   out_803274849067133910[233] = 0.0;
   out_803274849067133910[234] = 0.0;
   out_803274849067133910[235] = 0.0;
   out_803274849067133910[236] = 0.0;
   out_803274849067133910[237] = 0.0;
   out_803274849067133910[238] = 0.0;
   out_803274849067133910[239] = 0.0;
   out_803274849067133910[240] = 0.0;
   out_803274849067133910[241] = 0.0;
   out_803274849067133910[242] = 0.0;
   out_803274849067133910[243] = 0.0;
   out_803274849067133910[244] = 0.0;
   out_803274849067133910[245] = 0.0;
   out_803274849067133910[246] = 0.0;
   out_803274849067133910[247] = 1.0;
   out_803274849067133910[248] = 0.0;
   out_803274849067133910[249] = 0.0;
   out_803274849067133910[250] = 0.0;
   out_803274849067133910[251] = 0.0;
   out_803274849067133910[252] = 0.0;
   out_803274849067133910[253] = 0.0;
   out_803274849067133910[254] = 0.0;
   out_803274849067133910[255] = 0.0;
   out_803274849067133910[256] = 0.0;
   out_803274849067133910[257] = 0.0;
   out_803274849067133910[258] = 0.0;
   out_803274849067133910[259] = 0.0;
   out_803274849067133910[260] = 0.0;
   out_803274849067133910[261] = 0.0;
   out_803274849067133910[262] = 0.0;
   out_803274849067133910[263] = 0.0;
   out_803274849067133910[264] = 0.0;
   out_803274849067133910[265] = 0.0;
   out_803274849067133910[266] = 1.0;
   out_803274849067133910[267] = 0.0;
   out_803274849067133910[268] = 0.0;
   out_803274849067133910[269] = 0.0;
   out_803274849067133910[270] = 0.0;
   out_803274849067133910[271] = 0.0;
   out_803274849067133910[272] = 0.0;
   out_803274849067133910[273] = 0.0;
   out_803274849067133910[274] = 0.0;
   out_803274849067133910[275] = 0.0;
   out_803274849067133910[276] = 0.0;
   out_803274849067133910[277] = 0.0;
   out_803274849067133910[278] = 0.0;
   out_803274849067133910[279] = 0.0;
   out_803274849067133910[280] = 0.0;
   out_803274849067133910[281] = 0.0;
   out_803274849067133910[282] = 0.0;
   out_803274849067133910[283] = 0.0;
   out_803274849067133910[284] = 0.0;
   out_803274849067133910[285] = 1.0;
   out_803274849067133910[286] = 0.0;
   out_803274849067133910[287] = 0.0;
   out_803274849067133910[288] = 0.0;
   out_803274849067133910[289] = 0.0;
   out_803274849067133910[290] = 0.0;
   out_803274849067133910[291] = 0.0;
   out_803274849067133910[292] = 0.0;
   out_803274849067133910[293] = 0.0;
   out_803274849067133910[294] = 0.0;
   out_803274849067133910[295] = 0.0;
   out_803274849067133910[296] = 0.0;
   out_803274849067133910[297] = 0.0;
   out_803274849067133910[298] = 0.0;
   out_803274849067133910[299] = 0.0;
   out_803274849067133910[300] = 0.0;
   out_803274849067133910[301] = 0.0;
   out_803274849067133910[302] = 0.0;
   out_803274849067133910[303] = 0.0;
   out_803274849067133910[304] = 1.0;
   out_803274849067133910[305] = 0.0;
   out_803274849067133910[306] = 0.0;
   out_803274849067133910[307] = 0.0;
   out_803274849067133910[308] = 0.0;
   out_803274849067133910[309] = 0.0;
   out_803274849067133910[310] = 0.0;
   out_803274849067133910[311] = 0.0;
   out_803274849067133910[312] = 0.0;
   out_803274849067133910[313] = 0.0;
   out_803274849067133910[314] = 0.0;
   out_803274849067133910[315] = 0.0;
   out_803274849067133910[316] = 0.0;
   out_803274849067133910[317] = 0.0;
   out_803274849067133910[318] = 0.0;
   out_803274849067133910[319] = 0.0;
   out_803274849067133910[320] = 0.0;
   out_803274849067133910[321] = 0.0;
   out_803274849067133910[322] = 0.0;
   out_803274849067133910[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_1058845708616673013) {
   out_1058845708616673013[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_1058845708616673013[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_1058845708616673013[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_1058845708616673013[3] = dt*state[12] + state[3];
   out_1058845708616673013[4] = dt*state[13] + state[4];
   out_1058845708616673013[5] = dt*state[14] + state[5];
   out_1058845708616673013[6] = state[6];
   out_1058845708616673013[7] = state[7];
   out_1058845708616673013[8] = state[8];
   out_1058845708616673013[9] = state[9];
   out_1058845708616673013[10] = state[10];
   out_1058845708616673013[11] = state[11];
   out_1058845708616673013[12] = state[12];
   out_1058845708616673013[13] = state[13];
   out_1058845708616673013[14] = state[14];
   out_1058845708616673013[15] = state[15];
   out_1058845708616673013[16] = state[16];
   out_1058845708616673013[17] = state[17];
}
void F_fun(double *state, double dt, double *out_6637133899774644224) {
   out_6637133899774644224[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6637133899774644224[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6637133899774644224[2] = 0;
   out_6637133899774644224[3] = 0;
   out_6637133899774644224[4] = 0;
   out_6637133899774644224[5] = 0;
   out_6637133899774644224[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6637133899774644224[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6637133899774644224[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_6637133899774644224[9] = 0;
   out_6637133899774644224[10] = 0;
   out_6637133899774644224[11] = 0;
   out_6637133899774644224[12] = 0;
   out_6637133899774644224[13] = 0;
   out_6637133899774644224[14] = 0;
   out_6637133899774644224[15] = 0;
   out_6637133899774644224[16] = 0;
   out_6637133899774644224[17] = 0;
   out_6637133899774644224[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6637133899774644224[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6637133899774644224[20] = 0;
   out_6637133899774644224[21] = 0;
   out_6637133899774644224[22] = 0;
   out_6637133899774644224[23] = 0;
   out_6637133899774644224[24] = 0;
   out_6637133899774644224[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6637133899774644224[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_6637133899774644224[27] = 0;
   out_6637133899774644224[28] = 0;
   out_6637133899774644224[29] = 0;
   out_6637133899774644224[30] = 0;
   out_6637133899774644224[31] = 0;
   out_6637133899774644224[32] = 0;
   out_6637133899774644224[33] = 0;
   out_6637133899774644224[34] = 0;
   out_6637133899774644224[35] = 0;
   out_6637133899774644224[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6637133899774644224[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6637133899774644224[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6637133899774644224[39] = 0;
   out_6637133899774644224[40] = 0;
   out_6637133899774644224[41] = 0;
   out_6637133899774644224[42] = 0;
   out_6637133899774644224[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6637133899774644224[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_6637133899774644224[45] = 0;
   out_6637133899774644224[46] = 0;
   out_6637133899774644224[47] = 0;
   out_6637133899774644224[48] = 0;
   out_6637133899774644224[49] = 0;
   out_6637133899774644224[50] = 0;
   out_6637133899774644224[51] = 0;
   out_6637133899774644224[52] = 0;
   out_6637133899774644224[53] = 0;
   out_6637133899774644224[54] = 0;
   out_6637133899774644224[55] = 0;
   out_6637133899774644224[56] = 0;
   out_6637133899774644224[57] = 1;
   out_6637133899774644224[58] = 0;
   out_6637133899774644224[59] = 0;
   out_6637133899774644224[60] = 0;
   out_6637133899774644224[61] = 0;
   out_6637133899774644224[62] = 0;
   out_6637133899774644224[63] = 0;
   out_6637133899774644224[64] = 0;
   out_6637133899774644224[65] = 0;
   out_6637133899774644224[66] = dt;
   out_6637133899774644224[67] = 0;
   out_6637133899774644224[68] = 0;
   out_6637133899774644224[69] = 0;
   out_6637133899774644224[70] = 0;
   out_6637133899774644224[71] = 0;
   out_6637133899774644224[72] = 0;
   out_6637133899774644224[73] = 0;
   out_6637133899774644224[74] = 0;
   out_6637133899774644224[75] = 0;
   out_6637133899774644224[76] = 1;
   out_6637133899774644224[77] = 0;
   out_6637133899774644224[78] = 0;
   out_6637133899774644224[79] = 0;
   out_6637133899774644224[80] = 0;
   out_6637133899774644224[81] = 0;
   out_6637133899774644224[82] = 0;
   out_6637133899774644224[83] = 0;
   out_6637133899774644224[84] = 0;
   out_6637133899774644224[85] = dt;
   out_6637133899774644224[86] = 0;
   out_6637133899774644224[87] = 0;
   out_6637133899774644224[88] = 0;
   out_6637133899774644224[89] = 0;
   out_6637133899774644224[90] = 0;
   out_6637133899774644224[91] = 0;
   out_6637133899774644224[92] = 0;
   out_6637133899774644224[93] = 0;
   out_6637133899774644224[94] = 0;
   out_6637133899774644224[95] = 1;
   out_6637133899774644224[96] = 0;
   out_6637133899774644224[97] = 0;
   out_6637133899774644224[98] = 0;
   out_6637133899774644224[99] = 0;
   out_6637133899774644224[100] = 0;
   out_6637133899774644224[101] = 0;
   out_6637133899774644224[102] = 0;
   out_6637133899774644224[103] = 0;
   out_6637133899774644224[104] = dt;
   out_6637133899774644224[105] = 0;
   out_6637133899774644224[106] = 0;
   out_6637133899774644224[107] = 0;
   out_6637133899774644224[108] = 0;
   out_6637133899774644224[109] = 0;
   out_6637133899774644224[110] = 0;
   out_6637133899774644224[111] = 0;
   out_6637133899774644224[112] = 0;
   out_6637133899774644224[113] = 0;
   out_6637133899774644224[114] = 1;
   out_6637133899774644224[115] = 0;
   out_6637133899774644224[116] = 0;
   out_6637133899774644224[117] = 0;
   out_6637133899774644224[118] = 0;
   out_6637133899774644224[119] = 0;
   out_6637133899774644224[120] = 0;
   out_6637133899774644224[121] = 0;
   out_6637133899774644224[122] = 0;
   out_6637133899774644224[123] = 0;
   out_6637133899774644224[124] = 0;
   out_6637133899774644224[125] = 0;
   out_6637133899774644224[126] = 0;
   out_6637133899774644224[127] = 0;
   out_6637133899774644224[128] = 0;
   out_6637133899774644224[129] = 0;
   out_6637133899774644224[130] = 0;
   out_6637133899774644224[131] = 0;
   out_6637133899774644224[132] = 0;
   out_6637133899774644224[133] = 1;
   out_6637133899774644224[134] = 0;
   out_6637133899774644224[135] = 0;
   out_6637133899774644224[136] = 0;
   out_6637133899774644224[137] = 0;
   out_6637133899774644224[138] = 0;
   out_6637133899774644224[139] = 0;
   out_6637133899774644224[140] = 0;
   out_6637133899774644224[141] = 0;
   out_6637133899774644224[142] = 0;
   out_6637133899774644224[143] = 0;
   out_6637133899774644224[144] = 0;
   out_6637133899774644224[145] = 0;
   out_6637133899774644224[146] = 0;
   out_6637133899774644224[147] = 0;
   out_6637133899774644224[148] = 0;
   out_6637133899774644224[149] = 0;
   out_6637133899774644224[150] = 0;
   out_6637133899774644224[151] = 0;
   out_6637133899774644224[152] = 1;
   out_6637133899774644224[153] = 0;
   out_6637133899774644224[154] = 0;
   out_6637133899774644224[155] = 0;
   out_6637133899774644224[156] = 0;
   out_6637133899774644224[157] = 0;
   out_6637133899774644224[158] = 0;
   out_6637133899774644224[159] = 0;
   out_6637133899774644224[160] = 0;
   out_6637133899774644224[161] = 0;
   out_6637133899774644224[162] = 0;
   out_6637133899774644224[163] = 0;
   out_6637133899774644224[164] = 0;
   out_6637133899774644224[165] = 0;
   out_6637133899774644224[166] = 0;
   out_6637133899774644224[167] = 0;
   out_6637133899774644224[168] = 0;
   out_6637133899774644224[169] = 0;
   out_6637133899774644224[170] = 0;
   out_6637133899774644224[171] = 1;
   out_6637133899774644224[172] = 0;
   out_6637133899774644224[173] = 0;
   out_6637133899774644224[174] = 0;
   out_6637133899774644224[175] = 0;
   out_6637133899774644224[176] = 0;
   out_6637133899774644224[177] = 0;
   out_6637133899774644224[178] = 0;
   out_6637133899774644224[179] = 0;
   out_6637133899774644224[180] = 0;
   out_6637133899774644224[181] = 0;
   out_6637133899774644224[182] = 0;
   out_6637133899774644224[183] = 0;
   out_6637133899774644224[184] = 0;
   out_6637133899774644224[185] = 0;
   out_6637133899774644224[186] = 0;
   out_6637133899774644224[187] = 0;
   out_6637133899774644224[188] = 0;
   out_6637133899774644224[189] = 0;
   out_6637133899774644224[190] = 1;
   out_6637133899774644224[191] = 0;
   out_6637133899774644224[192] = 0;
   out_6637133899774644224[193] = 0;
   out_6637133899774644224[194] = 0;
   out_6637133899774644224[195] = 0;
   out_6637133899774644224[196] = 0;
   out_6637133899774644224[197] = 0;
   out_6637133899774644224[198] = 0;
   out_6637133899774644224[199] = 0;
   out_6637133899774644224[200] = 0;
   out_6637133899774644224[201] = 0;
   out_6637133899774644224[202] = 0;
   out_6637133899774644224[203] = 0;
   out_6637133899774644224[204] = 0;
   out_6637133899774644224[205] = 0;
   out_6637133899774644224[206] = 0;
   out_6637133899774644224[207] = 0;
   out_6637133899774644224[208] = 0;
   out_6637133899774644224[209] = 1;
   out_6637133899774644224[210] = 0;
   out_6637133899774644224[211] = 0;
   out_6637133899774644224[212] = 0;
   out_6637133899774644224[213] = 0;
   out_6637133899774644224[214] = 0;
   out_6637133899774644224[215] = 0;
   out_6637133899774644224[216] = 0;
   out_6637133899774644224[217] = 0;
   out_6637133899774644224[218] = 0;
   out_6637133899774644224[219] = 0;
   out_6637133899774644224[220] = 0;
   out_6637133899774644224[221] = 0;
   out_6637133899774644224[222] = 0;
   out_6637133899774644224[223] = 0;
   out_6637133899774644224[224] = 0;
   out_6637133899774644224[225] = 0;
   out_6637133899774644224[226] = 0;
   out_6637133899774644224[227] = 0;
   out_6637133899774644224[228] = 1;
   out_6637133899774644224[229] = 0;
   out_6637133899774644224[230] = 0;
   out_6637133899774644224[231] = 0;
   out_6637133899774644224[232] = 0;
   out_6637133899774644224[233] = 0;
   out_6637133899774644224[234] = 0;
   out_6637133899774644224[235] = 0;
   out_6637133899774644224[236] = 0;
   out_6637133899774644224[237] = 0;
   out_6637133899774644224[238] = 0;
   out_6637133899774644224[239] = 0;
   out_6637133899774644224[240] = 0;
   out_6637133899774644224[241] = 0;
   out_6637133899774644224[242] = 0;
   out_6637133899774644224[243] = 0;
   out_6637133899774644224[244] = 0;
   out_6637133899774644224[245] = 0;
   out_6637133899774644224[246] = 0;
   out_6637133899774644224[247] = 1;
   out_6637133899774644224[248] = 0;
   out_6637133899774644224[249] = 0;
   out_6637133899774644224[250] = 0;
   out_6637133899774644224[251] = 0;
   out_6637133899774644224[252] = 0;
   out_6637133899774644224[253] = 0;
   out_6637133899774644224[254] = 0;
   out_6637133899774644224[255] = 0;
   out_6637133899774644224[256] = 0;
   out_6637133899774644224[257] = 0;
   out_6637133899774644224[258] = 0;
   out_6637133899774644224[259] = 0;
   out_6637133899774644224[260] = 0;
   out_6637133899774644224[261] = 0;
   out_6637133899774644224[262] = 0;
   out_6637133899774644224[263] = 0;
   out_6637133899774644224[264] = 0;
   out_6637133899774644224[265] = 0;
   out_6637133899774644224[266] = 1;
   out_6637133899774644224[267] = 0;
   out_6637133899774644224[268] = 0;
   out_6637133899774644224[269] = 0;
   out_6637133899774644224[270] = 0;
   out_6637133899774644224[271] = 0;
   out_6637133899774644224[272] = 0;
   out_6637133899774644224[273] = 0;
   out_6637133899774644224[274] = 0;
   out_6637133899774644224[275] = 0;
   out_6637133899774644224[276] = 0;
   out_6637133899774644224[277] = 0;
   out_6637133899774644224[278] = 0;
   out_6637133899774644224[279] = 0;
   out_6637133899774644224[280] = 0;
   out_6637133899774644224[281] = 0;
   out_6637133899774644224[282] = 0;
   out_6637133899774644224[283] = 0;
   out_6637133899774644224[284] = 0;
   out_6637133899774644224[285] = 1;
   out_6637133899774644224[286] = 0;
   out_6637133899774644224[287] = 0;
   out_6637133899774644224[288] = 0;
   out_6637133899774644224[289] = 0;
   out_6637133899774644224[290] = 0;
   out_6637133899774644224[291] = 0;
   out_6637133899774644224[292] = 0;
   out_6637133899774644224[293] = 0;
   out_6637133899774644224[294] = 0;
   out_6637133899774644224[295] = 0;
   out_6637133899774644224[296] = 0;
   out_6637133899774644224[297] = 0;
   out_6637133899774644224[298] = 0;
   out_6637133899774644224[299] = 0;
   out_6637133899774644224[300] = 0;
   out_6637133899774644224[301] = 0;
   out_6637133899774644224[302] = 0;
   out_6637133899774644224[303] = 0;
   out_6637133899774644224[304] = 1;
   out_6637133899774644224[305] = 0;
   out_6637133899774644224[306] = 0;
   out_6637133899774644224[307] = 0;
   out_6637133899774644224[308] = 0;
   out_6637133899774644224[309] = 0;
   out_6637133899774644224[310] = 0;
   out_6637133899774644224[311] = 0;
   out_6637133899774644224[312] = 0;
   out_6637133899774644224[313] = 0;
   out_6637133899774644224[314] = 0;
   out_6637133899774644224[315] = 0;
   out_6637133899774644224[316] = 0;
   out_6637133899774644224[317] = 0;
   out_6637133899774644224[318] = 0;
   out_6637133899774644224[319] = 0;
   out_6637133899774644224[320] = 0;
   out_6637133899774644224[321] = 0;
   out_6637133899774644224[322] = 0;
   out_6637133899774644224[323] = 1;
}
void h_4(double *state, double *unused, double *out_930779811893977904) {
   out_930779811893977904[0] = state[6] + state[9];
   out_930779811893977904[1] = state[7] + state[10];
   out_930779811893977904[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_1858787399404689635) {
   out_1858787399404689635[0] = 0;
   out_1858787399404689635[1] = 0;
   out_1858787399404689635[2] = 0;
   out_1858787399404689635[3] = 0;
   out_1858787399404689635[4] = 0;
   out_1858787399404689635[5] = 0;
   out_1858787399404689635[6] = 1;
   out_1858787399404689635[7] = 0;
   out_1858787399404689635[8] = 0;
   out_1858787399404689635[9] = 1;
   out_1858787399404689635[10] = 0;
   out_1858787399404689635[11] = 0;
   out_1858787399404689635[12] = 0;
   out_1858787399404689635[13] = 0;
   out_1858787399404689635[14] = 0;
   out_1858787399404689635[15] = 0;
   out_1858787399404689635[16] = 0;
   out_1858787399404689635[17] = 0;
   out_1858787399404689635[18] = 0;
   out_1858787399404689635[19] = 0;
   out_1858787399404689635[20] = 0;
   out_1858787399404689635[21] = 0;
   out_1858787399404689635[22] = 0;
   out_1858787399404689635[23] = 0;
   out_1858787399404689635[24] = 0;
   out_1858787399404689635[25] = 1;
   out_1858787399404689635[26] = 0;
   out_1858787399404689635[27] = 0;
   out_1858787399404689635[28] = 1;
   out_1858787399404689635[29] = 0;
   out_1858787399404689635[30] = 0;
   out_1858787399404689635[31] = 0;
   out_1858787399404689635[32] = 0;
   out_1858787399404689635[33] = 0;
   out_1858787399404689635[34] = 0;
   out_1858787399404689635[35] = 0;
   out_1858787399404689635[36] = 0;
   out_1858787399404689635[37] = 0;
   out_1858787399404689635[38] = 0;
   out_1858787399404689635[39] = 0;
   out_1858787399404689635[40] = 0;
   out_1858787399404689635[41] = 0;
   out_1858787399404689635[42] = 0;
   out_1858787399404689635[43] = 0;
   out_1858787399404689635[44] = 1;
   out_1858787399404689635[45] = 0;
   out_1858787399404689635[46] = 0;
   out_1858787399404689635[47] = 1;
   out_1858787399404689635[48] = 0;
   out_1858787399404689635[49] = 0;
   out_1858787399404689635[50] = 0;
   out_1858787399404689635[51] = 0;
   out_1858787399404689635[52] = 0;
   out_1858787399404689635[53] = 0;
}
void h_10(double *state, double *unused, double *out_6196868944639979348) {
   out_6196868944639979348[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_6196868944639979348[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_6196868944639979348[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_4232608186599470764) {
   out_4232608186599470764[0] = 0;
   out_4232608186599470764[1] = 9.8100000000000005*cos(state[1]);
   out_4232608186599470764[2] = 0;
   out_4232608186599470764[3] = 0;
   out_4232608186599470764[4] = -state[8];
   out_4232608186599470764[5] = state[7];
   out_4232608186599470764[6] = 0;
   out_4232608186599470764[7] = state[5];
   out_4232608186599470764[8] = -state[4];
   out_4232608186599470764[9] = 0;
   out_4232608186599470764[10] = 0;
   out_4232608186599470764[11] = 0;
   out_4232608186599470764[12] = 1;
   out_4232608186599470764[13] = 0;
   out_4232608186599470764[14] = 0;
   out_4232608186599470764[15] = 1;
   out_4232608186599470764[16] = 0;
   out_4232608186599470764[17] = 0;
   out_4232608186599470764[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_4232608186599470764[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_4232608186599470764[20] = 0;
   out_4232608186599470764[21] = state[8];
   out_4232608186599470764[22] = 0;
   out_4232608186599470764[23] = -state[6];
   out_4232608186599470764[24] = -state[5];
   out_4232608186599470764[25] = 0;
   out_4232608186599470764[26] = state[3];
   out_4232608186599470764[27] = 0;
   out_4232608186599470764[28] = 0;
   out_4232608186599470764[29] = 0;
   out_4232608186599470764[30] = 0;
   out_4232608186599470764[31] = 1;
   out_4232608186599470764[32] = 0;
   out_4232608186599470764[33] = 0;
   out_4232608186599470764[34] = 1;
   out_4232608186599470764[35] = 0;
   out_4232608186599470764[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_4232608186599470764[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_4232608186599470764[38] = 0;
   out_4232608186599470764[39] = -state[7];
   out_4232608186599470764[40] = state[6];
   out_4232608186599470764[41] = 0;
   out_4232608186599470764[42] = state[4];
   out_4232608186599470764[43] = -state[3];
   out_4232608186599470764[44] = 0;
   out_4232608186599470764[45] = 0;
   out_4232608186599470764[46] = 0;
   out_4232608186599470764[47] = 0;
   out_4232608186599470764[48] = 0;
   out_4232608186599470764[49] = 0;
   out_4232608186599470764[50] = 1;
   out_4232608186599470764[51] = 0;
   out_4232608186599470764[52] = 0;
   out_4232608186599470764[53] = 1;
}
void h_13(double *state, double *unused, double *out_180148709017634934) {
   out_180148709017634934[0] = state[3];
   out_180148709017634934[1] = state[4];
   out_180148709017634934[2] = state[5];
}
void H_13(double *state, double *unused, double *out_1353486425927643166) {
   out_1353486425927643166[0] = 0;
   out_1353486425927643166[1] = 0;
   out_1353486425927643166[2] = 0;
   out_1353486425927643166[3] = 1;
   out_1353486425927643166[4] = 0;
   out_1353486425927643166[5] = 0;
   out_1353486425927643166[6] = 0;
   out_1353486425927643166[7] = 0;
   out_1353486425927643166[8] = 0;
   out_1353486425927643166[9] = 0;
   out_1353486425927643166[10] = 0;
   out_1353486425927643166[11] = 0;
   out_1353486425927643166[12] = 0;
   out_1353486425927643166[13] = 0;
   out_1353486425927643166[14] = 0;
   out_1353486425927643166[15] = 0;
   out_1353486425927643166[16] = 0;
   out_1353486425927643166[17] = 0;
   out_1353486425927643166[18] = 0;
   out_1353486425927643166[19] = 0;
   out_1353486425927643166[20] = 0;
   out_1353486425927643166[21] = 0;
   out_1353486425927643166[22] = 1;
   out_1353486425927643166[23] = 0;
   out_1353486425927643166[24] = 0;
   out_1353486425927643166[25] = 0;
   out_1353486425927643166[26] = 0;
   out_1353486425927643166[27] = 0;
   out_1353486425927643166[28] = 0;
   out_1353486425927643166[29] = 0;
   out_1353486425927643166[30] = 0;
   out_1353486425927643166[31] = 0;
   out_1353486425927643166[32] = 0;
   out_1353486425927643166[33] = 0;
   out_1353486425927643166[34] = 0;
   out_1353486425927643166[35] = 0;
   out_1353486425927643166[36] = 0;
   out_1353486425927643166[37] = 0;
   out_1353486425927643166[38] = 0;
   out_1353486425927643166[39] = 0;
   out_1353486425927643166[40] = 0;
   out_1353486425927643166[41] = 1;
   out_1353486425927643166[42] = 0;
   out_1353486425927643166[43] = 0;
   out_1353486425927643166[44] = 0;
   out_1353486425927643166[45] = 0;
   out_1353486425927643166[46] = 0;
   out_1353486425927643166[47] = 0;
   out_1353486425927643166[48] = 0;
   out_1353486425927643166[49] = 0;
   out_1353486425927643166[50] = 0;
   out_1353486425927643166[51] = 0;
   out_1353486425927643166[52] = 0;
   out_1353486425927643166[53] = 0;
}
void h_14(double *state, double *unused, double *out_8530894024565618123) {
   out_8530894024565618123[0] = state[6];
   out_8530894024565618123[1] = state[7];
   out_8530894024565618123[2] = state[8];
}
void H_14(double *state, double *unused, double *out_2104453456934794894) {
   out_2104453456934794894[0] = 0;
   out_2104453456934794894[1] = 0;
   out_2104453456934794894[2] = 0;
   out_2104453456934794894[3] = 0;
   out_2104453456934794894[4] = 0;
   out_2104453456934794894[5] = 0;
   out_2104453456934794894[6] = 1;
   out_2104453456934794894[7] = 0;
   out_2104453456934794894[8] = 0;
   out_2104453456934794894[9] = 0;
   out_2104453456934794894[10] = 0;
   out_2104453456934794894[11] = 0;
   out_2104453456934794894[12] = 0;
   out_2104453456934794894[13] = 0;
   out_2104453456934794894[14] = 0;
   out_2104453456934794894[15] = 0;
   out_2104453456934794894[16] = 0;
   out_2104453456934794894[17] = 0;
   out_2104453456934794894[18] = 0;
   out_2104453456934794894[19] = 0;
   out_2104453456934794894[20] = 0;
   out_2104453456934794894[21] = 0;
   out_2104453456934794894[22] = 0;
   out_2104453456934794894[23] = 0;
   out_2104453456934794894[24] = 0;
   out_2104453456934794894[25] = 1;
   out_2104453456934794894[26] = 0;
   out_2104453456934794894[27] = 0;
   out_2104453456934794894[28] = 0;
   out_2104453456934794894[29] = 0;
   out_2104453456934794894[30] = 0;
   out_2104453456934794894[31] = 0;
   out_2104453456934794894[32] = 0;
   out_2104453456934794894[33] = 0;
   out_2104453456934794894[34] = 0;
   out_2104453456934794894[35] = 0;
   out_2104453456934794894[36] = 0;
   out_2104453456934794894[37] = 0;
   out_2104453456934794894[38] = 0;
   out_2104453456934794894[39] = 0;
   out_2104453456934794894[40] = 0;
   out_2104453456934794894[41] = 0;
   out_2104453456934794894[42] = 0;
   out_2104453456934794894[43] = 0;
   out_2104453456934794894[44] = 1;
   out_2104453456934794894[45] = 0;
   out_2104453456934794894[46] = 0;
   out_2104453456934794894[47] = 0;
   out_2104453456934794894[48] = 0;
   out_2104453456934794894[49] = 0;
   out_2104453456934794894[50] = 0;
   out_2104453456934794894[51] = 0;
   out_2104453456934794894[52] = 0;
   out_2104453456934794894[53] = 0;
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

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_354310023345513326) {
  err_fun(nom_x, delta_x, out_354310023345513326);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_1844677192131749047) {
  inv_err_fun(nom_x, true_x, out_1844677192131749047);
}
void pose_H_mod_fun(double *state, double *out_803274849067133910) {
  H_mod_fun(state, out_803274849067133910);
}
void pose_f_fun(double *state, double dt, double *out_1058845708616673013) {
  f_fun(state,  dt, out_1058845708616673013);
}
void pose_F_fun(double *state, double dt, double *out_6637133899774644224) {
  F_fun(state,  dt, out_6637133899774644224);
}
void pose_h_4(double *state, double *unused, double *out_930779811893977904) {
  h_4(state, unused, out_930779811893977904);
}
void pose_H_4(double *state, double *unused, double *out_1858787399404689635) {
  H_4(state, unused, out_1858787399404689635);
}
void pose_h_10(double *state, double *unused, double *out_6196868944639979348) {
  h_10(state, unused, out_6196868944639979348);
}
void pose_H_10(double *state, double *unused, double *out_4232608186599470764) {
  H_10(state, unused, out_4232608186599470764);
}
void pose_h_13(double *state, double *unused, double *out_180148709017634934) {
  h_13(state, unused, out_180148709017634934);
}
void pose_H_13(double *state, double *unused, double *out_1353486425927643166) {
  H_13(state, unused, out_1353486425927643166);
}
void pose_h_14(double *state, double *unused, double *out_8530894024565618123) {
  h_14(state, unused, out_8530894024565618123);
}
void pose_H_14(double *state, double *unused, double *out_2104453456934794894) {
  H_14(state, unused, out_2104453456934794894);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
