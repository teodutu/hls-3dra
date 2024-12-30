#include "config.h"
#include <ap_int.h>

ap_int<32> ALU(ap_int<32> i1, ap_int<32> i2, ap_int<32> constant, ap_int<6> opcode,
				ap_int<1>& mem_req, ap_int<1>&mem_read_write, ap_int<32>&address, ap_int<32>& data);
