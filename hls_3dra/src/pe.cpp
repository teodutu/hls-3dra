#include "pe.h"
#include <iostream>
using namespace std;


ap_int<32> PE(ap_int<32>* in_channel, ap_int<5>& i1_src, ap_int<5>& i2_src, ap_int<5>& opcode){

    ap_int<32> i1, i2;
    static ap_int<32> res1;
	#pragma HLS interface mode=ap_ctrl_hs port=return

    static hls::stream<ap_int<32>, 4> i1_stream;
    static hls::stream<ap_int<32>, 4> i2_stream;

    for(int i = 0; i < PE_COUNT; i++) {
        if(i1_src == i){
            i1_stream.write(in_channel[i]);  //write to fifo
            i1 = i1_stream.read();       //read to ALU
            break;
        }
    }

    for(int j = 0; j < PE_COUNT; j++) {
        if(i2_src == j){
            i2_stream.write(in_channel[j]);
            i2 = i2_stream.read();
            break;
        }
    }

    // for(int i = 0; i < PE_COUNT; i++) {
    //     if(i1_src == i){
    //         i1_stream.write(in_channel[i]);  //write to fifo
    //         i1 = i1_stream.read();       //read to ALU
    //         break;
    //     }
    // }

    // for(int j = 0; j < PE_COUNT; j++) {
    //     if(i2_src == j){
    //         i2_stream.write(in_channel[j]);
    //         i2 = i2_stream.read();
    //         break;
    //     }
    // }

    cout << "i1: " << i1 << " i2: " << i2 << " opcode: " << opcode <<" ";

    res1=ALU(i1, i2, opcode);
    return res1;
}
