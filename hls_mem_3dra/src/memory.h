#include "config.h"
#include <ap_int.h>

ap_int<32> Memory(ap_int<32> &address, ap_int<1> &read_write, ap_int<32> &in_data);

ap_int<1> Memory_Controller(ap_int<1> *mem_valid, ap_int<32> *address, ap_int<1> *read_write, ap_int<32>*data,
        ap_int<1>*done, int offset);
