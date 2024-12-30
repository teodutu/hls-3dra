#include "MEM_CTRL.h"
#include <hls_stream.h>
#include <numeric>

void MEM_CTRL(MEM_CTRL_IO &io) {
    #pragma HLS PIPELINE

    // Transfer data from memory inputs to PEs
    io.mem_in1_to_PEs = io.mem_in1_from_mem;
    io.mem_in2_to_PEs = io.mem_in2_from_mem;

    // Determine if all store requests from PEs are valid
    // Set ready signals for PE store inputs
    // Concatenate store data from PEs to form the store_out.bits
    ap_uint<MEM_WIDTH> concatenated_store_data = 0;
    bool all_PEs_mem_store_valid = true;

    for (int i = 0; i < MEM_WIDTH / DATA_WIDTH; i++) {
        #pragma HLS UNROLL
        all_PEs_mem_store_valid &= io.v_store_from_PEs[i].valid;
        io.v_store_from_PEs[i].ready = io.store_out.ready && all_PEs_mem_store_valid;
        concatenated_store_data((i + 1) * DATA_WIDTH - 1, i * DATA_WIDTH) = io.v_store_from_PEs[i].bits;
    }
    io.store_out.bits = concatenated_store_data;

    // Set the valid signal for store_out
    io.store_out.valid = all_PEs_mem_store_valid;
}
