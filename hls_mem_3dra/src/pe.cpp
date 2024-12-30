#include "pe.h"
#include <iostream>
using namespace std;

ap_int<32> PE(ap_int<32>* in_channel, ap_int<1>& mem_req, ap_int<1>&mem_read_write, ap_int<32>&address,
        ap_int<32>& data, ap_int<1>& mem_read_valid, ap_int<MEM_WIDTH>& mem_in1)
{
    ap_int<32> i1, i2;
    static ap_int<32> res1;
    constexpr unsigned int log2_pe_count = log2(PE_COUNT);
	#pragma HLS interface mode=ap_ctrl_hs port=return

    static hls::stream<ap_int<32>, 4> i1_stream;
    static hls::stream<ap_int<32>, 4> i2_stream;

    ap_int<1> i1_used = mem_in1[0];
    ap_int<log2_pe_count> i1_src = mem_in1(log2_pe_count, 1);
    ap_int<1> i1_const_used = mem_in1[log2_pe_count + 1];

    int offset2 = DATA_WIDTH;
    ap_int<DATA_WIDTH> i1_const_value = mem_in1(offset2 + DATA_WIDTH - 1, offset2);

    int offset3 = offset2 + DATA_WIDTH;
    ap_int<1> i2_used = mem_in1[offset3];
    ap_int<log2_pe_count> i2_src = mem_in1(offset3 + log2_pe_count, offset3 + 1);
    ap_int<1> i2_const_used = mem_in1[offset3 + log2_pe_count + 1];

    int offset4 = offset3 + DATA_WIDTH;
    ap_int<DATA_WIDTH> i2_const_value = mem_in1(offset4 + DATA_WIDTH - 1, offset4);

    int offset5 = offset4 + DATA_WIDTH;
    ap_int<1> p_used = mem_in1[offset5];
    ap_int<log2_pe_count> p_src = mem_in1(offset5 + log2_pe_count, offset5 + 1);
    ap_int<2> p_const = mem_in1(offset5 + log2_pe_count + 2, offset5 + log2_pe_count + 1);

    int offset6 = offset5 + DATA_WIDTH;
    ap_int<1> output_valid = mem_in1[offset6];
    ap_int<OPCODE_WIDTH - 1> opcode = mem_in1(offset6 + OPCODE_WIDTH, offset6 + 1);

    int offset7 = offset6 + DATA_WIDTH;
    ap_int<DATA_WIDTH> initial_output = mem_in1(offset7 + DATA_WIDTH  - 1, offset7);

    ap_int<32> constant = 1;  // TODO: use consts from above

    for (int i = 0; i < PE_COUNT; i++) {
        if (i1_src == i){
            // i1_stream.write(in_channel[i]);  //write to fifo
            // i1 = i1_stream.read();       //read to ALU
            i1_stream << in_channel[i];
            i1_stream >> i1;
            break;
        }
    }

    for (int j = 0; j < PE_COUNT; j++) {
        if (i2_src == j){
            // i2_stream.write(in_channel[j]);
            // i2 = i2_stream.read();
            i2_stream << in_channel[j];
            i2_stream >> i2;
            break;
        }
    }

    cout << "i1: " << i1 << " i2: " << i2 << " opcode: " << opcode <<" ";

    if (mem_read_valid == 1){
        res1 = data;
    } else {
        res1 = ALU(i1, i2, constant, opcode, mem_req, mem_read_write, address, data);
    }


    return res1;
}
