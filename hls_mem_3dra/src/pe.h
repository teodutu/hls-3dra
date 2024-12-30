#include "config.h"
#include "comp.h"
#include <hls_stream.h>

ap_int<32> PE(ap_int<32>* in_channel, ap_int<1>& mem_req, ap_int<1>&mem_read_write, ap_int<32>&address,
        ap_int<32>& data, ap_int<1>& mem_read_valid, ap_int<MEM_WIDTH>& mem_in1);
