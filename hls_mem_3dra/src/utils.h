#ifndef UTILS_H_
#define UTILS_H_

#include <cstddef>
#include <hls_stream.h>

template<typename T>
class Decoupled
{
public:
    // Public members for the ready-valid interface
    bool ready;
    bool valid;
    T data;

    // Constructor to initialize the ready-valid signals
    Decoupled() : ready(false), valid(false) {}

    // // Function to send data (Producer logic)
    // void send(const T& input_data)
    // {
    //     if (ready) {
    //         data = input_data;
    //         valid = true;
    //     } else {
    //         valid = false;
    //     }
    // }

    // // Function to receive data (Consumer logic)
    // bool receive(T& output_data)
    // {
    //     if (valid && ready) {
    //         output_data = data;
    //         valid = false;
    //         return true;
    //     }
    //     return false;
    // }

    // Function to check if both ready and valid are set
    bool fire()
    {
        return ready && valid;
    }
};

template<typename T, size_t QUEUE_SIZE>
class Queue {
public:
    hls::stream<T, QUEUE_SIZE> queue;

    // Producer enqueue interface
    bool enq_ready;          // Queue is ready to accept data
    bool enq_valid;          // Producer has valid data to enqueue

    // Consumer dequeue interface
    bool deq_ready;          // Consumer is ready to receive data
    bool deq_valid;          // Queue has valid data to be dequeued

    Queue() : enq_ready(true), enq_valid(false), deq_ready(false), deq_valid(false) {}

    // Update the ready-valid status for enqueue and dequeue
    void updateStatus() {
        enq_ready = !queue.full();
        deq_valid = !queue.empty();
    }
};

#endif  // UTILS_H_
