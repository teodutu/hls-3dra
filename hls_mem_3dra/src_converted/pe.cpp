#include <systemc.h>
#include <hls_stream.h>
#include <cstdint>
#include "config.h"       // Define the Opcode enums and constants
#include "Store_Req.h"    // Define Store_Req struct
#include "General_Params.h"  // General parameters for CGRA

struct PE_params {
    int i1_src_idx;
    int i2_src_idx;
    int p_src_idx;
    int i1_const_val;
    int i2_const_val;
    int p_const_val;
    Opcode opcode;
    int constant;
    int pe_idx;
    int fifo_size;
    bool used;
    General_Params g;

    // Constructor
    PE_params() : i1_src_idx(-1), i2_src_idx(-1), p_src_idx(-1),
                  i1_const_val(0), i2_const_val(0), p_const_val(0),
                  opcode(Opcode::ADD), constant(0), pe_idx(999), fifo_size(1), 
                  used(true), g() {}
};

SC_MODULE(PE) {
    // Input and Output Ports
    sc_in<bool> clk, rst;
    sc_in<sc_uint<32>> pe_idx;
    hls::stream<sc_uint<32>> v_data_in[MAX_PE];
    hls::stream<sc_uint<32>> mem_in1, mem_in2;
    hls::stream<sc_uint<32>> data_out;
    hls::stream<Store_Req> store_out;

    // Internal signals and variables
    bool r_i1_used, r_i2_used, r_p_used;
    sc_uint<32> r_i1_src, r_i2_src, r_p_src, r_i1_const, r_i2_const, r_p_const;
    sc_uint<32> r_output;
    bool r_output_valid;
    Opcode r_opcode;
    hls::stream<sc_uint<32>> q_i1, q_i2;
    hls::stream<sc_uint<2>> q_p;

    // Constructor
    PE(const PE_params& params) : p(params) {
        // Initialize internal registers and signals
        SC_METHOD(process);
        sensitive << clk.pos();
        async_reset_signal_is(rst, true);
    }

    void process() {
        if (rst.read() == true) {
            // Reset logic
            r_i1_used = r_i2_used = r_p_used = false;
            r_output_valid = false;
            // Reset streams and output signals
            while (!q_i1.empty()) q_i1.read();
            while (!q_i2.empty()) q_i2.read();
            while (!q_p.empty()) q_p.read();
            data_out.write(0);
            store_out.write({0, 0, false});
        } else {
            // Normal operation logic
            // Handle input queue readiness and dequeue operations
            handleInputData();

            // Execute operation based on opcode and inputs
            executeComputation();
            
            // Output result handling
            outputResults();
        }
    }

    void handleInputData() {
        for (int i = 0; i < p.g.n_PEs; ++i) {
            if (r_i1_used && i == r_i1_src && !q_i1.full() && v_data_in[i].valid()) {
                q_i1.write(v_data_in[i].read());
            }
            if (r_i2_used && i == r_i2_src && !q_i2.full() && v_data_in[i].valid()) {
                q_i2.write(v_data_in[i].read());
            }
            if (r_p_used && i == r_p_src && !q_p.full() && v_data_in[i].valid()) {
                q_p.write(v_data_in[i].read().range(1, 0));  // Write 2 bits
            }
        }
    }

    void executeComputation() {
        sc_uint<32> in_1 = (r_i1_const_used) ? r_i1_const : q_i1.read();
        sc_uint<32> in_2 = (r_i2_const_used) ? r_i2_const : q_i2.read();
        sc_uint<2> pred = q_p.read();

        if (r_opcode == Opcode::ADD) {
            r_output = in_1 + in_2;
        } else if (r_opcode == Opcode::SUB) {
            r_output = in_1 - in_2;
        } else if (r_opcode == Opcode::MUL) {
            r_output = in_1 * in_2;
        }
        // Extend this for other opcodes

        // Set output valid flag
        r_output_valid = true;
    }

    void outputResults() {
        if (data_out.ready() && r_output_valid) {
            data_out.write(r_output);
            r_output_valid = false;
        }
    }

private:
    PE_params p;
};
