#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "definitions.h"

std::vector<std::string> split(const std::string& str, char delimeter = ' ') {
    std::vector<std::string> result;
    bool foundDelimeter = true;

    for (auto c : str) {
        if (c == delimeter) {
            foundDelimeter = true;
        } else {
            if (foundDelimeter) {
                result.push_back("");
                foundDelimeter = false;
            }

            result.back().push_back(c);
        }
    }

    return result;
}

struct Instruction {
    std::string type;
    std::vector<std::string> arguments;

    Instruction& fromString(const std::string& instruction) {
        std::vector<std::string> args = split(instruction, '\t');

        type = args[0];
        arguments.resize(args.size() - 1);

        for (int i = 0; i < arguments.size(); ++i) {
            arguments[i] = args[i + 1];
        }

        return *this;
    }
};

std::unordered_map<std::string, int> marks;
std::vector<Instruction> instructions;

void readInstructions() {
    std::ifstream stream(outputFile);
    std::string str;

    while (getline(stream, str)) {
        size_t colon = str.find(':');

        if (colon != std::string::npos) {
            if (colon == 0) {
                throw new std::runtime_error("The name of mark is empty.");
            }

            marks[str.substr(0, colon)] = instructions.size();
            continue;
        }

        instructions.push_back(std::move(Instruction().fromString(str)));
    }

    std::cout << "MARKS:\n";
    for (const auto& [mark, number] : marks) {
        std::cout << mark << " " << number << "\n";
    }

    std::cout << "INSTRUCTIONS:\n";
    for (const auto& instruction : instructions) {
        std::cout << instruction.type << " ";

        for (const auto& arg : instruction.arguments) {
            std::cout << arg << " ";
        }

        std::cout << "\n";
    }
}
