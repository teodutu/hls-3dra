#ifndef MEM_CTRL_H
#define MEM_CTRL_H

#include "General_Params.h"
#include "Decoupled.h"
#include <ap_int.h>
#include <array>

struct MEM_CTRL_IO {
    // Data from the memory to be read by PEs
    Decoupled<ap_uint<MEM_WIDTH>> mem_in1_from_mem;
    Decoupled<ap_uint<MEM_WIDTH>> mem_in2_from_mem;

    // Data from the PEs to be written to the memory
    std::array<Decoupled<ap_uint<DATA_WIDTH>>, MEM_WIDTH / DATA_WIDTH> v_store_from_PEs;

    // Data to PEs
    Decoupled<ap_uint<MEM_WIDTH>> mem_in1_to_PEs;
    Decoupled<ap_uint<MEM_WIDTH>> mem_in2_to_PEs;

    // Request to the memory
    Decoupled<ap_uint<MEM_WIDTH>> store_out;
};

void MEM_CTRL(MEM_CTRL_IO &io);

#endif // MEM_CTRL_H
