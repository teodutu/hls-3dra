#include "comp.h"
#include <hls_stream.h>

ap_int<32> PE1(ap_int<32> in1, ap_int<32> in2,ap_int<5>& i1_src, ap_int<5>& i2_src, ap_int<5>& opcode);
ap_int<32> PE2(ap_int<32> in1, ap_int<32> in2,ap_int<5>& i1_src, ap_int<5>& i2_src, ap_int<5>& opcode);