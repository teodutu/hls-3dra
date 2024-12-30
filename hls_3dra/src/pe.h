#include "config.h"
#include "comp.h"
#include <hls_stream.h>

ap_int<32> PE(ap_int<32>*in_channel,ap_int<5>& i1_src, ap_int<5>& i2_src, ap_int<5>& opcode);