#pragma once

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

enum VmNodeType {
    NODE_TYPE_INTEGER,
    NODE_TYPE_ARRAY,
};

class VmNode {
public:
    virtual ~VmNode() = default;

public:
    virtual VmNodeType GetNodeType() const = 0;
    virtual std::string Value() const = 0;
    virtual std::shared_ptr<VmNode> Negate() = 0;

public:
    virtual std::shared_ptr<VmNode> operator+(const VmNode& other) const = 0;
    virtual std::shared_ptr<VmNode> operator-(const VmNode& other) const = 0;
    virtual std::shared_ptr<VmNode> operator*(const VmNode& other) const = 0;
    virtual std::shared_ptr<VmNode> operator/(const VmNode& other) const = 0;
    virtual std::shared_ptr<VmNode> operator%(const VmNode& other) const = 0;

public:
    virtual bool operator<(const VmNode& other) const = 0;
    virtual bool operator>(const VmNode& other) const = 0;
    virtual bool operator<=(const VmNode& other) const = 0;
    virtual bool operator>=(const VmNode& other) const = 0;
    virtual bool operator!=(const VmNode& other) const = 0;
    virtual bool operator==(const VmNode& other) const = 0;
};

enum VmInstructionType {
    TYPE_PUSH = 0,
    TYPE_POP,
    TYPE_PRINT,
    TYPE_ADD,
    TYPE_SUB,
    TYPE_MUL,
    TYPE_DIV,
    TYPE_MOD,
    TYPE_COMPLT,
    TYPE_COMPGT,
    TYPE_COMPGE,
    TYPE_COMPLE,
    TYPE_COMPNE,
    TYPE_COMPEQ,
    TYPE_JZ,
    TYPE_JMP,
    TYPE_NEG,
    TYPE_CALL,
    TYPE_RETURN,
    TYPE_ARRAY,
    TYPE_ACCESS,
    TYPE_LENGTH,
    TYPE_BIN_AND,
    TYPE_BIN_OR,
};

struct Instruction {
    VmInstructionType type;
    std::vector<std::string> arguments;

    Instruction& fromString(const std::string& instruction);
};

struct Frame {
    // Variables are containing only weak pointer to objects on current frame.
    // This will break all cyclic dependencies and ARC will work correctly.
    std::unordered_map<std::string, std::weak_ptr<VmNode>> variables;

    std::vector<std::shared_ptr<VmNode>> objects;

    int returnAddress = -1;
};

class VirtualMachine {
public:
    void Run();

private:
    void ReadInstructions();
    void Optimize();
    void Execute();

private:
    std::unordered_map<std::string, int> _marks;
    std::vector<Instruction> _instructions;
    std::vector<Frame> _frames;
    std::vector<std::weak_ptr<VmNode>> _values;
};
