#ifndef COMP_H
#define COMP_H

#include "General_Params.h"
#include "Store_Req.h"
#include "Decoupled.h"
#include "Opcode.h"
#include <ap_int.h>
#include <hls_stream.h>

// Define the I/O interface for the COMP module
struct COMP_IO {
    Decoupled<ap_int<DATA_WIDTH>> in_1;
    bool in_1_used;

    Decoupled<ap_int<DATA_WIDTH>> in_2;
    bool in_2_used;

    Decoupled<ap_uint<2>> in_pred;
    bool in_pred_used;

    ap_uint<LOG_SIZE> pe_idx;
    ap_uint<32> cycle;

    ap_uint<OPCODE_WIDTH> opcode;

    Decoupled<ap_int<DATA_WIDTH>> output;
    Decoupled<ap_int<MEM_WIDTH>> mem_in1;
    Decoupled<ap_int<MEM_WIDTH>> mem_in2;

    Decoupled<Store_Req> store_out;

    // bool is_loopend;
    bool loop_ended;
};

COMP_IO[N_PEs] comp_ios;

void COMP(COMP_IO &io);

#endif // COMP_H
