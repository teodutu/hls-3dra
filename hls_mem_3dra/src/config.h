#ifndef TDRA_CONFIG_H_
#define TDRA_CONFIG_H_

#define PE_COUNT            2
#define MEMORY_SIZE         16
#define MEM_BANK            1
#define MEM_CTR_CHANNEL     2  //how many PEs are group together to interface with one bank
#define DATA_WIDTH          8
#define MEM_WIDTH           64
#define OPCODE_WIDTH        8

constexpr unsigned int log2(unsigned int x)
{
    return x < 2 ? x : 1 + log2(x >> 1);
}

#endif  // TDRA_CONFIG_H_
