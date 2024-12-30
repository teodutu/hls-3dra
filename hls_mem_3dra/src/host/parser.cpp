#include "parser.h"

// Mapping of string opcode names to their integer values.
static std::unordered_map<std::string, int> opcode_map = {
    {"ADD", OPCODE_ADD}, {"SUB", OPCODE_SUB}, {"MUL", OPCODE_MUL}, {"DIV", OPCODE_DIV},
    {"LS", OPCODE_LS}, {"RS", OPCODE_RS}, {"AND", OPCODE_AND}, {"OR", OPCODE_OR},
    {"XOR", OPCODE_XOR}, {"SELECT", OPCODE_SELECT}, {"CMP", OPCODE_CMP}, {"CLT", OPCODE_CLT},
    {"CLTE", OPCODE_CLTE}, {"LOAD", OPCODE_LOAD}, {"STORE", OPCODE_STORE}, {"CMERGE", OPCODE_SELECT}
};

// Helper function to check if a key exists in the map.
static bool opcode_exists(const std::string &opcode)
{
    return opcode_map.find(opcode) != opcode_map.end();
}

// Helper function to get an integer from the input line.
static int get_int_value(const std::string& token)
{
    return std::stoi(token.substr(token.find(":") + 1));
}

// Parsing function that processes the input file and returns a vector of PEConfig structs.
std::vector<PEConfig> parse_dfg_file(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    std::vector<PEConfig> configs;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return configs;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        PEConfig config = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        
        while (std::getline(iss, token, ',')) {
            if (token.find("IDX") != std::string::npos) {
                config.idx = get_int_value(token);
            }
            else if (token.find("OPCODE") != std::string::npos) {
                std::string opcode_str = token.substr(token.find(":") + 2);
                if (opcode_exists(opcode_str)) {
                    config.opcode = opcode_map[opcode_str];
                } else {
                    config.opcode = OPCODE_DUMMY;
                }
            }
            else if (token.find("SRC1") != std::string::npos) {
                config.src1_used = 1;
                config.src1 = get_int_value(token);
            }
            else if (token.find("CONST1") != std::string::npos) {
                config.src1_used = 0;
                config.src1_const = get_int_value(token);
            }
            else if (token.find("SRC2") != std::string::npos) {
                config.src2_used = 1;
                config.src2 = get_int_value(token);
            }
            else if (token.find("CONST2") != std::string::npos) {
                config.src2_used = 0;
                config.src2_const = get_int_value(token);
            }
            else if (token.find("SRC3") != std::string::npos) {
                config.src3_used = 1;
                config.src3 = get_int_value(token);
            }
            else if (token.find("CONST3") != std::string::npos) {
                config.src3_used = 0;
                config.src3_const = get_int_value(token);
            }
        }
        
        configs.push_back(config);
    }

    file.close();
    return configs;
}
