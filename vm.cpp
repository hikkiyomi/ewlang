#include <cctype>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "definitions.h"
#include "nodes.h"
#include "vm_definitions.h"

std::unordered_map<std::string, VmInstructionType> strToInstruction = {
    {"push", TYPE_PUSH},     {"pop", TYPE_POP},       {"print", TYPE_PRINT},
    {"add", TYPE_ADD},       {"sub", TYPE_SUB},       {"mul", TYPE_MUL},
    {"div", TYPE_DIV},       {"mod", TYPE_MOD},       {"compLT", TYPE_COMPLT},
    {"compGT", TYPE_COMPGT}, {"compGE", TYPE_COMPGE}, {"compLE", TYPE_COMPLE},
    {"compNE", TYPE_COMPNE}, {"compEQ", TYPE_COMPEQ}, {"jz", TYPE_JZ},
    {"jmp", TYPE_JMP},       {"neg", TYPE_NEG},       {"call", TYPE_CALL},
    {"return", TYPE_RETURN},
};

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

Instruction& Instruction::fromString(const std::string& instruction) {
    std::vector<std::string>&& args = split(instruction, '\t');

    type = strToInstruction[args[0]];
    arguments.resize(args.size() - 1);

    for (int i = 0; i < arguments.size(); ++i) {
        arguments[i] = args[i + 1];
    }

    return *this;
};

void VirtualMachine::Run() {
    ReadInstructions();
    Execute();
}

void VirtualMachine::ReadInstructions() {
    std::ifstream stream(outputFile);
    std::string str;

    while (getline(stream, str)) {
        size_t colon = str.find(':');

        if (colon != std::string::npos) {
            if (colon == 0) {
                throw std::runtime_error("The name of mark is empty.");
            }

            _marks[str.substr(0, colon)] = _instructions.size();
            continue;
        }

        _instructions.push_back(std::move(Instruction().fromString(str)));
    }

    // DEBUGGING INFO
    /*std::cout << "MARKS:\n";*/
    /*for (const auto& [mark, number] : _marks) {*/
    /*    std::cout << mark << " " << number << "\n";*/
    /*}*/
    /**/
    /*std::cout << "INSTRUCTIONS:\n";*/
    /*for (const auto& instruction : _instructions) {*/
    /*    std::cout << instruction.type << " ";*/
    /**/
    /*    for (const auto& arg : instruction.arguments) {*/
    /*        std::cout << arg << " ";*/
    /*    }*/
    /**/
    /*    std::cout << "\n";*/
    /*}*/
    /**/
    /*std::cout << "END OF DEBUGGING INFO\n===============\n";*/
}

bool IsNumber(const std::string& str) {
    for (auto c : str) {
        if (!std::isdigit(c)) {
            return false;
        }
    }

    return true;
}

void VirtualMachine::Execute() {
    int currentInstruction = _marks["entrypoint"];

    _frames.push_back(std::move(Frame()));

    while (currentInstruction < _instructions.size()) {
        const auto& instruction = _instructions[currentInstruction];
        auto& frame = _frames.back();

        switch (instruction.type) {
            case TYPE_PUSH: {
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error(
                        "push should have exactly one argument");
                }

                const std::string& arg = instruction.arguments[0];

                if (IsNumber(arg)) {
                    _values.push_back(std::make_shared<IntegerNode>(arg));
                } else {
                    if (frame.variables.find(arg) == frame.variables.end()) {
                        throw std::runtime_error("unknown variable: " + arg);
                    } else {
                        _values.push_back(frame.variables.at(arg));
                    }
                }

                break;
            }
            case TYPE_POP: {
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error("pop needs 1 argument");
                }

                if (_values.empty()) {
                    throw std::runtime_error(
                        "value stack is empty, nothing to pop");
                }

                const std::string& arg = instruction.arguments[0];

                frame.variables[arg] = _values.back();
                _values.pop_back();

                break;
            }
            case TYPE_PRINT: {
                if (_values.empty()) {
                    throw std::runtime_error(
                        "value stack is empty, nothing to print");
                }

                std::cout << _values.back()->Value() << "\n";
                _values.pop_back();

                break;
            }
            case TYPE_ADD: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for add");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                _values.push_back(*lhs.get() + *rhs.get());

                break;
            }
            case TYPE_SUB: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for sub");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                _values.push_back(*lhs.get() - *rhs.get());

                break;
            }
            case TYPE_MUL: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for mul");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                _values.push_back(*lhs.get() * *rhs.get());

                break;
            }
            case TYPE_DIV: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for div");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                _values.push_back(*lhs.get() / *rhs.get());

                break;
            }
            case TYPE_MOD: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for mod");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                _values.push_back(*lhs.get() % *rhs.get());

                break;
            }
            case TYPE_NEG: {
                if (_values.empty()) {
                    throw std::runtime_error("value stack is empty for neg");
                }

                std::shared_ptr<VmNode> lhs = _values.back();
                lhs->Negate();

                break;
            }
            case TYPE_COMPEQ: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compeq");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() == *rhs.get());

                _values.push_back(std::make_shared<IntegerNode>(result));

                break;
            }
            case TYPE_COMPGE: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compge");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() >= *rhs.get());

                _values.push_back(std::make_shared<IntegerNode>(result));

                break;
            }
            case TYPE_COMPGT: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compgt");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() > *rhs.get());

                _values.push_back(std::make_shared<IntegerNode>(result));

                break;
            }
            case TYPE_COMPLE: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for comple");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() <= *rhs.get());

                _values.push_back(std::make_shared<IntegerNode>(result));

                break;
            }
            case TYPE_COMPLT: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for complt");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() < *rhs.get());

                _values.push_back(std::make_shared<IntegerNode>(result));

                break;
            }
            case TYPE_COMPNE: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compne");
                }

                std::shared_ptr<VmNode> rhs = _values.back();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() != *rhs.get());

                _values.push_back(std::make_shared<IntegerNode>(result));

                break;
            }
            case TYPE_JMP: {
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error(
                        "jmp should have exactly one argument");
                }

                // Substitute 1, because of ++currentInstruction at the end of
                // the cycle.
                currentInstruction = _marks[instruction.arguments[0]] - 1;

                break;
            }
            case TYPE_JZ: {
                if (_values.empty()) {
                    throw std::runtime_error("value stack is empty for jz");
                }

                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error(
                        "jz should have exactly one argument");
                }

                if (_values.back()->GetNodeType() != NODE_TYPE_INTEGER) {
                    throw std::runtime_error(
                        "jz cannot check because top of the stack is not "
                        "integer");
                }

                // jz jumps if the top of the stack is 0
                if (std::static_pointer_cast<IntegerNode>(_values.back())
                        ->RealValue()
                        .Value() == "0") {
                    // Substitute 1, because of ++currentInstruction at the end
                    // of the cycle.
                    currentInstruction = _marks[instruction.arguments[0]] - 1;
                }

                _values.pop_back();

                break;
            }
            case TYPE_CALL: {
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error("call should have one argument");
                }

                int jumpTo = _marks[instruction.arguments[0]];
                int returnTo = currentInstruction + 1;

                _frames.push_back(std::move(Frame()));
                _frames.back().returnAddress = returnTo;

                // Substitute 1, because of ++currentInstruction at the end
                // of the cycle.
                currentInstruction = jumpTo - 1;

                break;
            }
            case TYPE_RETURN: {
                // If returnAddress is -1, then we are returning from the
                // entrypoint. Therefore, end the program.
                if (frame.returnAddress == -1) {
                    return;
                }

                // Substitute 1, because of ++currentInstruction at the end
                // of the cycle.
                currentInstruction = frame.returnAddress - 1;
                _frames.pop_back();

                break;
            }
            default: {
                throw std::runtime_error("caught unknown instruction: " +
                                         std::to_string(instruction.type));
            }
        }

        ++currentInstruction;
    }
}
