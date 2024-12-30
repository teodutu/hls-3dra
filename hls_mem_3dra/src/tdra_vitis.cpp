#include "3dra.h"

extern "C"
{
    void tdra_vitis(ap_int<MEM_WIDTH> *mem_in1, ap_int<32> *out, int size)
    {

    #pragma HLS INTERFACE m_axi port = mem_in1 offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
    // #pragma HLS INTERFACE s_axilite port = in_src1 bundle = control
    // #pragma HLS INTERFACE s_axilite port = in_src2 bundle = control
    // #pragma HLS INTERFACE s_axilite port = opcode bundle = control
    // #pragma HLS INTERFACE s_axilite port = out bundle = control
    // #pragma HLS INTERFACE s_axilite port = size bundle = control
    // #pragma HLS INTERFACE s_axilite port = return bundle = control

        ap_int<MEM_WIDTH> mem_in1_local[PE_COUNT];
        ap_int<32> out_local[PE_COUNT];
        static ap_int<32> in_channel[PE_COUNT];

        //no memory so this is used as a initial value to do computation 
        in_channel[0] = 3;
        in_channel[1] = 4;

        for (int i = 0; i < size; i += PE_COUNT) {    
            #pragma HLS pipeline
            for (int j = 0; j < PE_COUNT; ++j) {
                #pragma HLS unroll
                mem_in1_local[j] = mem_in1[i + 1];
            }

            TDRA(in_channel, out_local, mem_in1_local);

            for (int j = 0; j < PE_COUNT; ++j) {
                #pragma HLS unroll
                in_channel[i + j] = out_local[j];
                out[i + j] = out_local[j];
            }
        }
    }
}

// static void read_input(ap_int<32>* in, hls::stream<ap_int<32>>& inStream, int size) {
// // Auto-pipeline is going to apply pipeline to this loop
// mem_rd:
//     for (int i = 0; i < size; i++) {
//     #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
//         inStream << in[i];
//     }
// }

// static void write_result(ap_int<32>* out, hls::stream<ap_int<32>>& outStream, int size) {
// // Auto-pipeline is going to apply pipeline to this loop
// mem_wr:
//     for (int i = 0; i < size; i++) {
// #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
//         out[i] = outStream.read();
//     }
// }

// static void compute_3dra(hls::stream<ap_int<32>>& inStream1, hls::stream<ap_int<32>>& inStream2, hls::stream<ap_int<32>>& outStream, int size)
// {

//     ap_int<32> pe1;
//     ap_int<32> pe2;

//     execute:
//     for (int i = 0; i < size; i++) {
//     #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
//         pe1=inStream1.read();
//         pe2=inStream2.read();
//         pe1 = PE(pe1, pe2, pe1, 0,1,0);
//         pe2 = PE(pe1, pe2, pe2, 0,1,0);

//         outStream << pe1 + pe2;
//     }

// }

// extern "C" {
// /*
//     Vector Addition Kernel Implementation using dataflow
//     Arguments:
//         in1   (input)  --> Input Vector 1
//         in2   (input)  --> Input Vector 2
//         out  (output) --> Output Vector
//         size (input)  --> Size of Vector in Integer
//    */
// void tdra_vitis(ap_int<32>* in1, ap_int<32>* in2, ap_int<32>* out, int size) {
//     static hls::stream<ap_int<32>> inStream1("input_stream_1");
//     static hls::stream<ap_int<32>> inStream2("input_stream_2");
//     static hls::stream<ap_int<32>> outStream("output_stream");

// #pragma HLS INTERFACE m_axi port = in1 bundle = gmem0
// #pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
// #pragma HLS INTERFACE m_axi port = out bundle = gmem0

// #pragma HLS dataflow
//     // dataflow pragma instruct compiler to run following three APIs in parallel
//     read_input(in1, inStream1, size);
//     read_input(in2, inStream2, size);
//     compute_3dra(inStream1, inStream2, outStream, size);
//     write_result(out, outStream, size);
// }
// }
