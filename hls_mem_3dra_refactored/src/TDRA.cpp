#include "TDRA.h"
#include <hls_stream.h>

void TDRA(TDRA_IO &io) {
    //#pragma HLS INLINE off
    //#pragma HLS PIPELINE

    // Instantiate PE and MEM_CTRL modules
    MEM_CTRL_IO mem_ctrl_io;
    MEM_CTRL(mem_ctrl_io);

    PE_IO v_PEs[N_PEs];
    #pragma HLS ARRAY_PARTITION variable=v_PEs complete

    // Connect memory inputs to MEM_CTRL
    mem_ctrl_io.mem_in1_from_mem = io.mem_in1;
    mem_ctrl_io.mem_in2_from_mem = io.mem_in2;

    // Create control signals for PEs
    bool all_PEs_mem_in1_ready = true;
    bool all_PEs_mem_in2_ready = true;
    bool all_PEs_mem_store_valid = true;
    bool loop_ended = false;

    ap_uint<1> reg_used_offsets[MEM_WIDTH / DATA_WIDTH] = {0};
    ap_uint<1> is_programmed = 0;

    // Constants (offsets)
    const int offset2 = DATA_WIDTH;
    const int offset6 = DATA_WIDTH * 5;

    // Extracting opcode and constant
    ap_uint<OPCODE_WIDTH> opcode = io.mem_in1.bits(offset6 + OPCODE_WIDTH - 1, offset6);
    ap_uint<DATA_WIDTH> constt = io.mem_in1.bits(offset2 + DATA_WIDTH - 1, offset2);

    // Logic equivalent to `when` in Chisel
    if (io.rst.valid && opcode == OPCODE_STORE) {
        printf("reg_used_offsets: %d\n", (int)constt.to_uint());
        reg_used_offsets[constt.to_uint()] = 1;
        is_programmed = 1;
    }

    int idx = 0;
    for (auto& store_in : mem_ctrl_io.v_store_from_PEs) {
        #pragma HLS UNROLL
        store_in.bits = 0;
        store_in.valid = !io.rst.valid;

        if (!reg_used_offsets[idx] && is_programmed) {
            store_in.valid = true;
        }

        idx++;
    }

    // Initialize all PEs
    for (int i = 0; i < N_PEs; i++) {
        #pragma HLS UNROLL
        PE_IO &pe_io = v_PEs[i];
        // PE_params &params = pe_params[i];

        pe_io.rst = io.rst;
        pe_io.pe_idx = i;
        pe_io.loop_ended = false;

        for (int j = 0; j < N_PEs; j++) {
            #pragma HLS UNROLL
            v_PEs[j].v_data_in[i].bits = pe_io.data_out.bits;
            v_PEs[j].v_data_in[i].valid = pe_io.data_out.valid;

            pe_io.data_out.ready &= v_PEs[j].v_data_in[i].ready;
        }

        // Connect memory inputs from MEM_CTRL to PEs
        pe_io.mem_in1.bits = mem_ctrl_io.mem_in1_to_PEs.bits;
        pe_io.mem_in1.valid = mem_ctrl_io.mem_in1_to_PEs.valid && all_PEs_mem_in1_ready;
        pe_io.mem_in2.bits = mem_ctrl_io.mem_in2_to_PEs.bits;
        pe_io.mem_in2.valid = mem_ctrl_io.mem_in2_to_PEs.valid && all_PEs_mem_in2_ready;

        pe_io.store_out.ready = mem_ctrl_io.store_out.ready;

        // Process the PE
        PE(pe_io);

        mem_ctrl_io.v_store_from_PEs[pe_io.store_out.bits.addr].bits = pe_io.store_out.bits.data;
        mem_ctrl_io.v_store_from_PEs[pe_io.store_out.bits.addr].valid =
            pe_io.store_out.bits.is_store ? pe_io.store_out.valid : true;

        // Aggregate ready and valid signals
        all_PEs_mem_in1_ready &= pe_io.mem_in1.ready;
        all_PEs_mem_in2_ready &= pe_io.mem_in2.ready;
        all_PEs_mem_store_valid &= pe_io.store_out.valid;
        io.done |= pe_io.loop_ended;
    }

    // Connect outputs back to MEM_CTRL
    mem_ctrl_io.mem_in1_to_PEs.ready = all_PEs_mem_in1_ready;
    mem_ctrl_io.mem_in2_to_PEs.ready = all_PEs_mem_in2_ready;

    // Output memory results
    io.mem_out = mem_ctrl_io.store_out;
}
