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

constexpr size_t ARRAY_SIZE_LIMIT = 100'000'000;

std::unordered_map<std::string, VmInstructionType> strToInstruction = {
    {"push", TYPE_PUSH},     {"pop", TYPE_POP},       {"print", TYPE_PRINT},
    {"add", TYPE_ADD},       {"sub", TYPE_SUB},       {"mul", TYPE_MUL},
    {"div", TYPE_DIV},       {"mod", TYPE_MOD},       {"compLT", TYPE_COMPLT},
    {"compGT", TYPE_COMPGT}, {"compGE", TYPE_COMPGE}, {"compLE", TYPE_COMPLE},
    {"compNE", TYPE_COMPNE}, {"compEQ", TYPE_COMPEQ}, {"jz", TYPE_JZ},
    {"jmp", TYPE_JMP},       {"neg", TYPE_NEG},       {"call", TYPE_CALL},
    {"return", TYPE_RETURN}, {"array", TYPE_ARRAY},   {"access", TYPE_ACCESS},
    {"length", TYPE_LENGTH},
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

void RescueArray(Frame& frame, std::shared_ptr<VmNode> node) {
    /*std::cout << "here\n";*/
    /**/
    /*for (const auto& o : frame.objects) {*/
    /*    std::cout << o->GetNodeType() << " " << o->Value() << "\n";*/
    /*}*/
    /**/
    /*std::cout << "======\n";*/

    if (node->GetNodeType() == NODE_TYPE_ARRAY) {
        std::shared_ptr<ArrayNode> casted =
            std::static_pointer_cast<ArrayNode>(node);

        for (int i = 0; i < casted->Size(); i++) {
            std::shared_ptr<VmNode> toSave = casted->Get(i).lock();
            frame.objects.push_back(toSave);

            // If the value is also array, rescue it recursively.
            RescueArray(frame, toSave);
        }
    }
}

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
                    frame.objects.push_back(std::make_shared<IntegerNode>(arg));
                    _values.push_back(frame.objects.back());
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
                if (instruction.arguments.size() < 1) {
                    throw std::runtime_error("pop needs at least 1 argument");
                }

                if (_values.empty()) {
                    throw std::runtime_error(
                        "value stack is empty, nothing to pop");
                }

                if (instruction.arguments.size() == 1) {
                    const std::string& arg = instruction.arguments[0];

                    frame.variables[arg] = _values.back();
                    _values.pop_back();
                } else {
                    // We are setting the value to array cell.
                    const std::string& arg = instruction.arguments[1];

                    std::shared_ptr<IntegerNode> index =
                        std::static_pointer_cast<IntegerNode>(
                            _values.back().lock());
                    _values.pop_back();

                    std::shared_ptr<VmNode> value = _values.back().lock();
                    _values.pop_back();

                    std::shared_ptr<ArrayNode> arrayNode =
                        std::static_pointer_cast<ArrayNode>(
                            frame.variables[arg].lock());

                    arrayNode->Set(index->RealValue(), value);
                }

                break;
            }
            case TYPE_PRINT: {
                if (_values.empty()) {
                    throw std::runtime_error(
                        "value stack is empty, nothing to print");
                }

                std::cout << _values.back().lock()->Value() << "\n";
                _values.pop_back();

                break;
            }
            case TYPE_ADD: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for add");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> result = *lhs.get() + *rhs.get();

                frame.objects.push_back(result);
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_SUB: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for sub");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> result = *lhs.get() - *rhs.get();

                frame.objects.push_back(result);
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_MUL: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for mul");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> result = *lhs.get() * *rhs.get();

                frame.objects.push_back(result);
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_DIV: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for div");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> result = *lhs.get() / *rhs.get();

                frame.objects.push_back(result);
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_MOD: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for mod");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> result = *lhs.get() % *rhs.get();

                frame.objects.push_back(result);
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_NEG: {
                if (_values.empty()) {
                    throw std::runtime_error("value stack is empty for neg");
                }

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                lhs->Negate();

                break;
            }
            case TYPE_COMPEQ: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compeq");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() == *rhs.get());

                frame.objects.push_back(std::make_shared<IntegerNode>(result));
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_COMPGE: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compge");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() >= *rhs.get());

                frame.objects.push_back(std::make_shared<IntegerNode>(result));
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_COMPGT: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compgt");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() > *rhs.get());

                frame.objects.push_back(std::make_shared<IntegerNode>(result));
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_COMPLE: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for comple");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() <= *rhs.get());

                frame.objects.push_back(std::make_shared<IntegerNode>(result));
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_COMPLT: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for complt");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() < *rhs.get());

                frame.objects.push_back(std::make_shared<IntegerNode>(result));
                _values.push_back(frame.objects.back());

                break;
            }
            case TYPE_COMPNE: {
                if (_values.size() < 2) {
                    throw std::runtime_error(
                        "value stack does not contain 2 variables for compne");
                }

                std::shared_ptr<VmNode> rhs = _values.back().lock();
                _values.pop_back();

                std::shared_ptr<VmNode> lhs = _values.back().lock();
                _values.pop_back();

                int result = static_cast<int>(*lhs.get() != *rhs.get());

                frame.objects.push_back(std::make_shared<IntegerNode>(result));
                _values.push_back(frame.objects.back());

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

                if (_values.back().lock()->GetNodeType() != NODE_TYPE_INTEGER) {
                    throw std::runtime_error(
                        "jz cannot check because top of the stack is not "
                        "integer");
                }

                // jz jumps if the top of the stack is 0
                if (std::static_pointer_cast<IntegerNode>(_values.back().lock())
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
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error("return should have one argument");
                }

                // If returnAddress is -1, then we are returning from the
                // entrypoint. Therefore, end the program.
                if (frame.returnAddress == -1) {
                    return;
                }

                int amountOfReturned = stoi(instruction.arguments[0]);

                if (_values.size() < amountOfReturned) {
                    throw std::runtime_error(
                        "amount of returned values is greater than the stack "
                        "size");
                }

                // Objects that were returned should not be deallocated.
                // Therefore, we push them at the previous frame.
                auto& previousFrame = _frames[_frames.size() - 2];

                for (int i = 0; i < amountOfReturned; ++i) {
                    std::shared_ptr<VmNode> locked =
                        _values[_values.size() - 1 - i].lock();

                    previousFrame.objects.push_back(locked);

                    // If this node is an array, we should rescue all objects
                    // inside array.
                    RescueArray(previousFrame, locked);
                }

                // Substitute 1, because of ++currentInstruction at the end
                // of the cycle.
                currentInstruction = frame.returnAddress - 1;
                _frames.pop_back();

                break;
            }
            case TYPE_ARRAY: {
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error("array needs 1 argument");
                }

                if (_values.empty()) {
                    throw std::runtime_error(
                        "value stack is empty, no size for creating array");
                }

                const std::string& arg = instruction.arguments[0];
                std::shared_ptr<VmNode> arraySizeNode = _values.back().lock();

                _values.pop_back();

                if (arraySizeNode->GetNodeType() != NODE_TYPE_INTEGER) {
                    throw std::runtime_error(
                        "provided array size is not integer");
                }

                BigInteger arraySize =
                    std::static_pointer_cast<IntegerNode>(arraySizeNode)
                        ->RealValue();

                if (arraySize > BigInteger(ARRAY_SIZE_LIMIT)) {
                    throw std::runtime_error("provided array size is too big");
                }

                int integerSize = stoi(arraySize.Value());

                frame.objects.push_back(
                    std::make_shared<ArrayNode>(integerSize, frame));

                frame.variables[arg] = frame.objects.back();

                break;
            }
            case TYPE_ACCESS: {
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error("access needs 1 argument");
                }

                if (_values.empty()) {
                    throw std::runtime_error(
                        "value stack is empty, no index for accessing");
                }

                const std::string& arg = instruction.arguments[0];
                std::shared_ptr<VmNode> arrayIndexNode = _values.back().lock();

                _values.pop_back();

                if (arrayIndexNode->GetNodeType() != NODE_TYPE_INTEGER) {
                    throw std::runtime_error(
                        "provided array index is not integer");
                }

                BigInteger arrayIndex =
                    std::static_pointer_cast<IntegerNode>(arrayIndexNode)
                        ->RealValue();

                std::shared_ptr<ArrayNode> arrayNode =
                    std::static_pointer_cast<ArrayNode>(
                        frame.variables[arg].lock());

                _values.push_back(arrayNode->Get(arrayIndex));

                break;
            }
            case TYPE_LENGTH: {
                if (instruction.arguments.size() != 1) {
                    throw std::runtime_error("length needs 1 argument");
                }

                const std::string& arg = instruction.arguments[0];

                std::shared_ptr<VmNode> node = frame.variables[arg].lock();

                if (node->GetNodeType() != NODE_TYPE_ARRAY) {
                    throw std::runtime_error(
                        "cannot get length of non-array type");
                }

                std::shared_ptr<ArrayNode> arrayNode =
                    std::static_pointer_cast<ArrayNode>(node);

                frame.objects.push_back(
                    std::make_shared<IntegerNode>(arrayNode->Size()));

                _values.push_back(frame.objects.back());

                break;
            }
            default: {
                throw std::runtime_error("caught unknown instruction: " +
                                         std::to_string(instruction.type));
            }
        }

        ++currentInstruction;

        /*for (const auto& [name, value] : frame.variables) {*/
        /*    std::cout << name << " " << value.use_count() << "\n";*/
        /*}*/
        /*std::cout << "==========\n";*/
    }
}
