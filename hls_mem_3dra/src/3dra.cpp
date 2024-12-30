#include "3dra.h"

void TDRA(ap_int<32> * in_channel, ap_int<32> *out, ap_int<MEM_WIDTH> *mem_in1)
{
    // static ap_int<32> in1 = 3;
    // static ap_int<32> in2 = 4;
    ap_int<32> pe_res[PE_COUNT];

    static ap_int<32> mem_address[PE_COUNT]; //32 bits address, 32 bits data, 1 bit read/write
    static ap_int<32> data[PE_COUNT]; 
    static ap_int<1> mem_read_write[PE_COUNT]; 
    static ap_int<1> mem_req_valid[PE_COUNT];

    static ap_int<1> read_done[PE_COUNT]; 

    for (int i = 0; i < PE_COUNT; i++) {
        #pragma HLS PIPELINE
        data[i] = i;
    }
        // data[i] = input_data[i];

#pragma HLS PIPELINE

    for (int i = 0; i < PE_COUNT; i++) {
        #pragma HLS pipeline
        pe_res[i] = PE(in_channel, mem_req_valid[i], mem_read_write[i], mem_address[i],
            data[i], read_done[i], mem_in1[i]);
        out[i] = pe_res[i];
    }

    //Each Memory controller controls MEM_CTR_CHANNEL number of PEs
    for (int k = 0; k < PE_COUNT / MEM_CTR_CHANNEL; k += MEM_CTR_CHANNEL) {
        #pragma HLS unroll
        Memory_Controller(mem_req_valid, mem_address, mem_read_write, data, read_done, k);
    }
}
