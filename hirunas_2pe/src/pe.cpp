#include "pe.h"
#include <iostream>
using namespace std;


ap_int<32> PE1(ap_int<32> in1, ap_int<32> in2,ap_int<5>& i1_src, ap_int<5>& i2_src, ap_int<5>& opcode)
{

    ap_int<32> i1, i2;
    static ap_int<32> res1;
	#pragma HLS interface mode=ap_ctrl_hs port=return

    static hls::stream<ap_int<32>, 4> i1_stream;
    static hls::stream<ap_int<32>, 4> i2_stream;

    if (i1_src == 0){
        i1_stream << in1;
        i1_stream >> i1;

    }else if (i1_src == 1){
        i1_stream << in2;
        i1_stream >> i1;
    }

    if (i2_src == 0){
        i2_stream << in1;
        i2_stream >> i2;

    }else if (i2_src == 1){
        i2_stream << in2;
        i2_stream >> i2;
        
    }

    cout << "i1: " << i1 << " i2: " << i2 << " opcode: " << opcode <<" ";

    res1=ALU(i1, i2, opcode);
    return res1;
}

ap_int<32> PE2(ap_int<32> in1, ap_int<32> in2,ap_int<5>& i1_src, ap_int<5>& i2_src, ap_int<5>& opcode)
{

    ap_int<32> i1, i2;
    static ap_int<32> res2;
	#pragma HLS interface mode=ap_ctrl_hs port=return

    static hls::stream<ap_int<32>, 4> i1_stream;
    static hls::stream<ap_int<32>, 4> i2_stream;

    if (i1_src == 0){
        i1_stream << in1;
        i1_stream >> i1;

    }else if (i1_src == 1){
        i1_stream << in2;
        i1_stream >> i1;
    }

    if (i2_src == 0){
        i2_stream << in1;
        i2_stream >> i2;

    }else if (i2_src == 1){
        i2_stream << in2;
        i2_stream >> i2;
        
    }

    cout << "i1: " << i1 << " i2: " << i2 << " opcode: " << opcode <<" ";

    res2=ALU(i1, i2, opcode);
    return res2;
}
