#ifndef PE_H
#define PE_H

#include "General_Params.h"
#include "Store_Req.h"
#include "Decoupled.h"
#include "Opcode.h"
#include "Pred.h"
#include "COMP.h"
#include <ap_int.h>
#include <hls_stream.h>
#include <array>

template<typename T, int SIZE>
class Queue {
public:
    // Enqueue interface (Producer -> Queue)
    struct EnqInterface {
        bool valid;    // Producer provides valid data
        T bits;        // Data to enqueue
        bool ready;    // Queue indicates it is ready to accept data
    } io_enq;

    // Dequeue interface (Queue -> Consumer)
    struct DeqInterface {
        bool valid;    // Queue provides valid data
        T bits;        // Data to dequeue
        bool ready;    // Consumer indicates it is ready to receive data
    } io_deq;

private:
    hls::stream<T, SIZE> fifo;  // Internal FIFO for storing data
    bool fifo_full;
    bool fifo_empty;

public:
    // Constructor
    Queue() : fifo_full(false), fifo_empty(true) {
        // Initialize enqueue interface
        io_enq.valid = false;
        io_enq.ready = true;

        // Initialize dequeue interface
        io_deq.valid = false;
        io_deq.ready = false;
    }

    // Queue logic (called every clock cycle)
    void tick() {
        // Update FIFO status
        fifo_full = fifo.full();
        fifo_empty = fifo.empty();

        // Handle enqueue
        if (io_enq.valid && io_enq.ready) {
            fifo.write(io_enq.bits); // Write data into the FIFO
        }

        // Handle dequeue
        if (io_deq.valid && io_deq.ready) {
            fifo.read(io_deq.bits); // Read data from the FIFO
        }

        // Update ready-valid handshake signals
        io_enq.ready = !fifo_full;   // Queue is ready if not full
        io_deq.valid = !fifo_empty; // Queue has valid data if not empty
    }
};

struct PE_IO {
    // Inputs
    Decoupled<ap_uint<LOG_SIZE>> rst;
    ap_uint<LOG_SIZE> pe_idx;

    std::array<Decoupled<ap_int<DATA_WIDTH>>, N_PEs> v_data_in;
    Decoupled<ap_int<MEM_WIDTH>> mem_in1;
    Decoupled<ap_int<MEM_WIDTH>> mem_in2;

    // Outputs
    Decoupled<ap_int<DATA_WIDTH>> data_out;
    Decoupled<Store_Req> store_out;

    bool is_loopend, loop_ended;
};

void PE(PE_IO &io);

#endif // PE_H
