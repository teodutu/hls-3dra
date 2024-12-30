#include "3dra.h"

void TDRA(ap_int<5> *i1_src, ap_int<5> *i2_src, ap_int<5> *opcode, ap_int<32> *out)
{
    static ap_int<32> in1 = 3;
    static ap_int<32> in2 = 4;
    ap_int<32> pe1;
    ap_int<32> pe2;

#pragma HLS PIPELINE

    pe1 = PE1(in1, in2, i1_src[0], i2_src[0], opcode[0]);
    pe2 = PE2(in1, in2, i1_src[1], i2_src[1], opcode[1]);

    in1 = pe1;
    in2 = pe2;

    out[0] = pe1;
    out[1] = pe2;
}
