#include "TDRA_Wrapper.h"


extern "C" {
    void TDRA_Wrapper(Decoupled<ap_int<CONFIG_WIDTH>>* s_axi_in_A, Decoupled<ap_int<MEM_WIDTH>>* s_axi_in_B,
            Decoupled<ap_int<MEM_WIDTH>>* m_axi_out) {
        #pragma HLS INTERFACE m_axi port = s_axi_in_A offset = slave bundle = gmem
        #pragma HLS INTERFACE m_axi port = s_axi_in_B offset = slave bundle = gmem
        #pragma HLS INTERFACE m_axi port = m_axi_out offset = slave bundle = gmem

        // Decoupled<AXI_S> s_axi_in_A, s_axi_in_B, m_axi_out;

        // #pragma HLS INLINE off
        // #pragma HLS PIPELINE

        // Register for cycle count
        ap_uint<32> reg_cycle = 0;
        TDRA_IO tdra_io;

        tdra_io.done = false;

        m_axi_out->bits.data = tdra_io.mem_out.bits;
        tdra_io.mem_out.ready = m_axi_out->ready;
        // m_axi_out->bits.keep = s_axi_in_A->bits.keep & s_axi_in_B->bits.keep;
        // m_axi_out->bits.last = s_axi_in_A->bits.last && s_axi_in_B->bits.last;

        while (!tdra.io.done) {
            #pragma HLS PIPELINE
            tdra_io.rst.valid = true;
            tdra_io.rst.bits = reg_cycle < N_PEs;

            // Handle input stream A
            tdra_io.mem_in1.valid = s_axi_in_A[i].valid;
            tdra_io.mem_in1.bits = s_axi_in_A[i].bits;
            s_axi_in_A[i].ready = true;

            // Handle input stream B
            tdra_io.mem_in2.valid = s_axi_in_B->valid;
            tdra_io.mem_in2.bits = s_axi_in_B->bits;
            s_axi_in_B->ready = tdra_io.mem_in2.ready;

            // Handle output stream
            m_axi_out->valid = tdra_io.mem_out.valid;

            bool in_ready = m_axi_out->ready;
            s_axi_in_A->ready = s_axi_in_B->ready = in_ready;

            // 3DRA instance
            TDRA(tdra_io);

            reg_cycle++;
        }
    }
}
