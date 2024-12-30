#include "iostream"
#include "memory.h"
using namespace std;

//Instantiate memory blocks
ap_int<32>Memory(ap_int<32>&address, ap_int<1>&read_write, ap_int<32>&in_data){
   
    static ap_int<32> out_data;
    #pragma HLS interface mode=ap_ctrl_hs port=return

    ap_int<32> SRAM[MEMORY_SIZE];
    #pragma HLS bind_storage variable = SRAM type = RAM_2P impl = bram
    
    //read = 1, write = 0
    if(read_write ==  1){
        out_data = SRAM[address];
    }else {
        SRAM[address] = in_data;
    } 
    return out_data;
}

// each memory controller control MEM_CTR_CHANNEL number of PEs
ap_int<1>Memory_Controller(ap_int<1> *mem_valid, ap_int<32>*address, ap_int<1>*read_write, ap_int<32>*data, ap_int<1>*done,
                    int offset){
    static ap_int<32> out_data;

    //sequential, serve one at a time
    for(int i=0; i<MEM_CTR_CHANNEL; i++){
        if(mem_valid[i+offset]==1){
            out_data = Memory(address[i+offset], read_write[i+offset], data[i+offset]);
            if(read_write[i+offset] == 1){
                data[i+offset] = out_data;
                done[i+offset] = 1;
            }else{
                done[i+offset] = 0;
            }
            
        }
    }

    return 0;
}


