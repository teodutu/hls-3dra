#ifndef PARSER_H_
#define PARSER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

#include <ap_int.h>

#include "pugixml.hpp"

#include "../General_Params.h"
#include "../Opcode.h"

struct PEConfig {
    ap_int<OPCODE_WIDTH> opcode;
    bool src1_used, src2_used, pred_used;
    ap_int<LOG_SIZE> idx, src1, src2, pred;
    ap_int<DATA_WIDTH> src1_const, src2_const, pred_const;
};

std::vector<PEConfig> parse_dfg_file(const std::string &filename);

#endif  // PARSER_H_
