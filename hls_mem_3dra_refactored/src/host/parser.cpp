#include "parser.h"

// Opcode mapping
std::unordered_map<std::string, int> opcode_map = {
    {"ADD", OPCODE_ADD}, {"SUB", OPCODE_SUB}, {"MUL", OPCODE_MUL}, {"DIV", OPCODE_DIV},
    {"LS", OPCODE_LS}, {"RS", OPCODE_RS}, {"AND", OPCODE_AND}, {"OR", OPCODE_OR},
    {"XOR", OPCODE_XOR}, {"SELECT", OPCODE_SELECT}, {"CMP", OPCODE_CMP}, {"CLT", OPCODE_CLT},
    {"CLTE", OPCODE_CLTE}, {"LOAD", OPCODE_LOAD}, {"STORE", OPCODE_STORE},
    {"DUMMY", OPCODE_DUMMY}, {"LOADB", OPCODE_LOAD}, {"STOREB", OPCODE_STORE},
    {"CMERGE", OPCODE_SELECT}, {"MOVC", OPCODE_DUMMY}
};

// Function to parse a single Node element
PEConfig parse_node(const pugi::xml_node& node) {
    PEConfig config = {};
    config.idx = node.attribute("idx").as_int();
    config.src1_used = false;
    config.src2_used = false;
    config.pred_used = false;
    config.src1_const = 0;
    config.src2_const = 0;
    config.pred_const = 0;

    // Get opcode
    std::string op = node.child("OP").child_value();
    if (opcode_map.find(op) != opcode_map.end()) {
        config.opcode = opcode_map[op];
    } else {
        config.opcode = OPCODE_NIL;
    }

    // Parse inputs
    int input_count = 0;
    for (const auto& input : node.child("Inputs").children("Input")) {
        int input_idx = input.attribute("idx").as_int();
        if (input_count == 0) {
            config.src1 = input_idx;
            config.src1_used = true;
        } else if (input_count == 1) {
            config.src2 = input_idx;
            config.src2_used = true;
        }
        input_count++;
    }

    // Parse outputs
    for (const auto& output : node.child("Outputs").children("Output")) {
        int output_idx = output.attribute("idx").as_int();
        (void)output_idx; // Use this if needed for additional logic
    }

    // Parse constants
    if (node.attribute("CONST")) {
        config.src1_const = node.attribute("CONST").as_int();
    }

    return config;
}

// Function to parse the XML and return a vector of PEConfig
std::vector<PEConfig> parse_dfg_file(const std::string& file_name) {
    std::vector<PEConfig> configs;

    // Load XML file
    pugi::xml_document doc;
    if (!doc.load_file(file_name.c_str())) {
        std::cerr << "Failed to load XML file: " << file_name << std::endl;
        return configs;
    }

    // Parse DFG Nodes
    for (const auto& node : doc.child("xml").child("DFG").children("Node")) {
        PEConfig config = parse_node(node);
        configs.push_back(config);
    }

    return configs;
}
