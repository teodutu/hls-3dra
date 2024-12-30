#include "PE.h"
#include <hls_stream.h>

void PE(PE_IO &io) {
    #pragma HLS INLINE off
    #pragma HLS PIPELINE

    ap_uint<32> cycle = 0;
    bool r_i1_used = false;
    ap_uint<LOG_SIZE> r_i1_src = 0;
    bool r_i1_const_used = false;
    ap_int<DATA_WIDTH> r_i1_const = 0;
    Queue<ap_int<DATA_WIDTH>, FIFO_SIZE> q_i1;

    bool r_i2_used = false;
    ap_uint<LOG_SIZE> r_i2_src = 0;
    bool r_i2_const_used = false;
    ap_int<DATA_WIDTH> r_i2_const = 0;
    Queue<ap_int<DATA_WIDTH>, FIFO_SIZE> q_i2;

    bool r_p_used = false;
    ap_uint<LOG_SIZE> r_p_src = 0;
    ap_uint<2> r_p_const = 2;
    Queue<ap_int<2>, FIFO_SIZE> q_p;

    ap_int<DATA_WIDTH> r_output = 0;
    bool r_output_valid = false;
    ap_uint<OPCODE_WIDTH> r_opcode = OPCODE_NIL;

    // Cycle counter
    if (io.rst.valid) {
        cycle = 0;
    } else {
        cycle++;
    }

    q_i1.tick();
    q_i2.tick();
    q_p.tick();

    q_i1.io_enq.bits = 0;
    q_i2.io_enq.bits = 0;
    q_p.io_enq.bits = 0;

    for (int i = 0; i < N_PEs; ++i) {
        #pragma HLS UNROLL
        Decoupled<ap_int<DATA_WIDTH>>& in_port = io.v_data_in[i];

        in_port.ready = !(r_i1_used && r_i1_src == i && !q_i1.io_enq.ready)
            && !(r_i2_used && r_i2_src == i && !q_i2.io_enq.ready)
            && !(r_p_used && r_p_src == i && !q_p.io_enq.ready);

        if (r_i1_used && (r_i1_src == i) && in_port.fire()) {
            q_i1.io_enq.valid = true;
            q_i1.io_enq.bits = in_port.bits;
        }
        if (r_i2_used && (r_i2_src == i) && in_port.fire()) {
            q_i2.io_enq.valid = true;
            q_i2.io_enq.bits = in_port.bits;
        }
        if (r_p_used && (r_p_src == i) && in_port.fire()) {
            q_p.io_enq.valid = true;
            q_p.io_enq.bits = in_port.bits;
        }
    }

    // Component instance
    COMP_IO comp_io;

    comp_io.is_loopend = io.is_loopend;
    comp_io.loop_ended = io.loop_ended;
    comp_io.pe_idx = io.pe_idx;
    comp_io.cycle = cycle;

    // Connect inputs to COMP
    comp_io.in_1.bits = r_i1_const_used ? r_i1_const : q_i1.io_deq.bits;
    comp_io.in_2.bits = r_i2_const_used ? r_i2_const : q_i2.io_deq.bits;
    comp_io.in_pred.bits = q_p.io_deq.bits;

    comp_io.in_1_used = r_i1_used || r_i1_const_used;
    comp_io.in_2_used = r_i2_used || r_i2_const_used;
    comp_io.in_pred_used = r_p_used;

    bool comp_in1_valid = r_i1_used ? q_i1.io_deq.valid : true;
    bool comp_in2_valid = r_i2_used ? q_i2.io_deq.valid : true;
    bool comp_inp_valid = r_p_used ? q_p.io_deq.valid && q_p.io_deq.bits == r_p_const : true;

    bool is_store = r_opcode == OPCODE_STORE;
    bool is_load = r_opcode == OPCODE_LOAD;

    bool comp_all_inputs_valid = (!io.rst.valid) && comp_in1_valid && comp_in2_valid && comp_inp_valid;      // sending valid signals only if all the operands are ready.
    comp_io.in_1.valid = comp_all_inputs_valid;
    comp_io.in_2.valid  = comp_all_inputs_valid;
    comp_io.in_pred.valid = comp_all_inputs_valid;

    comp_io.opcode = r_opcode;
    r_output = comp_io.output.fire() ? comp_io.output.bits : r_output;

    if (is_load && r_p_used && q_p.io_deq.valid && q_p.io_deq.bits != r_p_const && comp_in1_valid && comp_in2_valid) {
        q_i1.io_deq.ready = q_i1.io_deq.ready = q_p.io_deq.ready = r_output_valid = true;
        r_output = PRED_NULL;
    } else if (is_store && r_p_used && q_p.io_deq.valid && q_p.io_deq.bits != r_p_const && comp_in1_valid
            && comp_in2_valid && io.store_out.ready) {
        q_i1.io_deq.ready = q_i1.io_deq.ready = q_p.io_deq.ready = r_output_valid = true;
        r_output = PRED_NULL; 
    } else if ((r_opcode == OPCODE_CMP || r_opcode == OPCODE_CLT || r_opcode == OPCODE_CLTE)
            && q_p.io_deq.valid && q_p.io_deq.bits != r_p_const) {
        q_i1.io_deq.ready = q_i1.io_deq.ready = q_p.io_deq.ready = r_output_valid = true;
        r_output = PRED_NULL;        
    } else if (comp_io.output.fire()) {
        q_i1.io_deq.ready = q_i1.io_deq.ready = q_p.io_deq.ready = r_output_valid = true;
    } else if (comp_io.store_out.fire()) {
        q_i1.io_deq.ready = q_i1.io_deq.ready = q_p.io_deq.ready = true;
    } else {
        q_i1.io_deq.ready = q_i1.io_deq.ready = q_p.io_deq.ready = true;
    }

    comp_io.output.ready = (!r_output_valid || io.data_out.fire()) && !io.rst.valid;

    if (io.data_out.ready && r_output_valid) {
        io.data_out.bits = (1, r_output);
        io.data_out.valid = true;
        r_output_valid = comp_io.output.fire();
    } else {
        io.data_out.bits = r_output;
        io.data_out.valid = false;
    }

    if (io.rst.valid) {
        if (io.rst.bits == io.pe_idx && io.mem_in1.fire()) {
            // Initialize registers during reset
            for (auto& in_port : io.v_data_in) {
                #pragma HLS UNROLL
                in_port.ready = false;
            }
            io.mem_in1.ready             = true;          // To get the programming bits.
            io.mem_in2.ready             = false;
            io.data_out.valid            = false;
            io.store_out.valid           = false;
            io.store_out.bits.addr       = 0;
            io.store_out.bits.data       = 0;
            io.store_out.bits.is_store   = false;
            comp_io.mem_in1.valid        = false;
            comp_io.mem_in2.valid        = false;
            comp_io.store_out.ready      = false;
            comp_io.mem_in1.bits         = 0;
            comp_io.mem_in2.bits         = 0;
            comp_io.in_1.valid           = false;
            comp_io.in_2.valid           = false;
            comp_io.output.ready         = false;

            if (io.rst.bits == io.pe_idx && io.mem_in1.fire()) {
                // Programming logic when memory input is valid

                r_i1_used         = io.mem_in1.bits[0];
                r_i1_src          = io.mem_in1.bits(LOG_SIZE, 1);
                r_i1_const_used   = io.mem_in1.bits[LOG_SIZE + 1];

                // 2nd Entry: i1 constant value
                int offset2 = DATA_WIDTH;
                r_i1_const = io.mem_in1.bits(offset2 + DATA_WIDTH - 1, offset2);

                // 3rd Entry: i2 used, i2 src, i2 constant value used
                int offset3 = DATA_WIDTH * 2;
                r_i2_used = io.mem_in1.bits[offset3];
                r_i2_src = io.mem_in1.bits(offset3 + LOG_SIZE, offset3 + 1);
                r_i2_const_used = io.mem_in1.bits[offset3 + LOG_SIZE + 1];

                // 4th Entry: i2 constant value
                int offset4 = DATA_WIDTH * 3;
                r_i2_const = io.mem_in1.bits(offset4 + DATA_WIDTH - 1, offset4);

                // 5th Entry: p used, p src, p constant value
                int offset5 = DATA_WIDTH * 4;
                r_p_used = io.mem_in1.bits[offset5];
                r_p_src = io.mem_in1.bits(offset5 + LOG_SIZE, offset5 + 1);
                r_p_const = io.mem_in1.bits(offset5 + LOG_SIZE + 2, offset5 + LOG_SIZE + 1);

                // 6th Entry: Initial output value used, opcode
                int offset6 = DATA_WIDTH * 5;
                r_output_valid = io.mem_in1.bits[offset6];
                r_opcode = io.mem_in1.bits(offset6 + OPCODE_WIDTH, offset6 + 1);

                // 7th Entry: Initial output value
                int offset7 = DATA_WIDTH * 6;
                r_output = io.mem_in1.bits(offset7 + DATA_WIDTH - 1, offset7);
            }
        }
    } else if (io.rst.valid && io.pe_idx != io.rst.bits) {
        for (auto& in_port : io.v_data_in) {
            #pragma HLS UNROLL
            in_port.ready = false;
        }
        io.mem_in1.ready            = false;
        io.mem_in2.ready            = false;
        io.data_out.valid           = false;
        io.store_out.valid          = false;
        io.store_out.bits.addr      = 0;
        io.store_out.bits.data      = 0;
        io.store_out.bits.is_store  = false;
        comp_io.mem_in1.valid       = false;
        comp_io.mem_in2.valid       = false;
        comp_io.store_out.ready     = false;
        comp_io.mem_in1.bits        = 0;
        comp_io.mem_in2.bits        = 0;
    } else {
        comp_io.mem_in1             = io.mem_in1;  // connecting memory ports.
        comp_io.mem_in2             = io.mem_in2;  // connecting memory ports.
        io.store_out.valid          = r_opcode == OPCODE_STORE ? comp_io.store_out.valid : true;
        io.store_out                = comp_io.store_out;
    }

    // Execute the COMP module
    COMP(comp_io);
}
