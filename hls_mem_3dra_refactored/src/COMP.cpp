#include "COMP.h"
#include <hls_stream.h>
#include <array>

void COMP(COMP_IO &io) {
    #pragma HLS INLINE off
    #pragma HLS PIPELINE

    // Determine the operation type
    bool is_load = (io.opcode == OPCODE_LOAD);
    bool is_store = (io.opcode == OPCODE_STORE);
    bool is_used = (io.opcode != OPCODE_NIL);

    bool b_from_mem_in1 = io.in_1.valid && is_load && (io.in_1.bits[4] == 0) && io.mem_in1.valid;
    bool b_from_mem_in2 = is_load && io.mem_in2.valid;

    ap_uint<MEM_ADDR_WIDTH> addr_from_mem = io.in_1.bits(3, 0);
    
    // Memory slices
    static ap_uint<DATA_WIDTH> mem1_slices[MEM_WIDTH / DATA_WIDTH] ;
    static ap_uint<DATA_WIDTH> mem2_slices[MEM_WIDTH / DATA_WIDTH] ;

    for (int i = 0; i < MEM_WIDTH / DATA_WIDTH; i++) {
        #pragma HLS UNROLL
        mem1_slices[i] = io.mem_in1.bits((i + 1) * DATA_WIDTH - 1, i * DATA_WIDTH);
        mem2_slices[i] = io.mem_in2.bits((i + 1) * DATA_WIDTH - 1, i * DATA_WIDTH);
    }

    // Ready signals
    io.in_1.ready = is_store ? io.store_out.ready : (is_load ? (b_from_mem_in1 || b_from_mem_in2) : io.output.ready);
    io.in_2.ready = is_store ? io.store_out.ready : io.output.ready;
    io.in_pred.ready = is_store ? io.store_out.ready : io.output.ready;
    io.mem_in1.ready = is_load ? io.output.ready && io.in_1.valid : true;
    io.mem_in2.ready = is_load ? io.output.ready && io.in_1.valid : true;  // TODO: in2.valid ?

    // Default store output
    io.store_out.bits.addr = 0;
    io.store_out.bits.data = 0;
    io.store_out.bits.is_store = is_store;
    io.store_out.valid = false;

    // Default output
    io.output.bits = 0;
    io.output.valid = false;

    // ALU Operations
    if (io.output.ready && !is_load && !is_store) {
        switch (io.opcode) {
            case OPCODE_ADD:
                io.output.bits = io.in_1.bits + io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_SUB:
                io.output.bits = io.in_1.bits - io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_MUL:
                io.output.bits = io.in_1.bits * io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_DIV:
                io.output.bits = io.in_1.bits / io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_LS:
                io.output.bits = io.in_1.bits << io.in_2.bits(4, 0);
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_RS:
                io.output.bits = io.in_1.bits >> io.in_2.bits(4, 0);
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_AND:
                io.output.bits = io.in_1.bits & io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_OR:
                io.output.bits = io.in_1.bits | io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_XOR:
                io.output.bits = io.in_1.bits ^ io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_CMP:
                io.output.bits = io.in_1.bits == io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_CLT:
                io.output.bits = io.in_1.bits < io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_CLTE:
                io.output.bits = io.in_1.bits <= io.in_2.bits;
                io.output.valid = io.in_1.valid && io.in_2.valid;
                break;
            case OPCODE_SELECT:
                io.output.bits = io.in_pred.bits ? io.in_2.bits : io.in_1.bits;
                io.output.valid = io.in_2.valid && io.in_1.valid;
            default:
                io.output.bits = 0;
                io.output.valid = false;
                break;
        }
    } else if (io.output.ready && is_load) {
        io.output.valid = io.in_1.valid && (b_from_mem_in1 || b_from_mem_in2);
        io.output.bits = b_from_mem_in1 ? mem1_slices[addr_from_mem] : mem2_slices[addr_from_mem];
    } else if (is_store) {
        io.store_out.bits.addr = io.in_1.bits;
        io.store_out.bits.data = io.in_2.bits;
        io.store_out.valid = io.in_1.valid && io.in_2.valid;
        // if (io.is_loopend && io.in_1.valid && io.in_2.valid)
        //     io.loop_ended = true;
    } else if (io.opcode == OPCODE_DUMMY) {
        io.store_out.bits.addr = io.in_1.bits;
        io.store_out.bits.data = 0;
        io.store_out.valid = true;
    }
}
