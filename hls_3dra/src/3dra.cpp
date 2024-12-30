#include "3dra.h"

void TDRA(ap_int<32> *in_channel, ap_int<5> *i1_src, ap_int<5> *i2_src, ap_int<5> *opcode, ap_int<32> *out)
{
    // static ap_int<32> in1 = 3;
    // static ap_int<32> in2 = 4;
    ap_int<32> pe_res[PE_COUNT];

    #pragma HLS PIPELINE
    for(int i = 0; i < PE_COUNT; i++) {
        #pragma HLS unroll
        pe_res[i]=PE(in_channel, i1_src[i], i2_src[i], opcode[i] );
        out[i] = pe_res[i];
    }

}
