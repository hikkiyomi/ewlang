#pragma once

#include <stdexcept>

#include "bigint.h"
#include "vm_definitions.h"

class IntegerNode : public VmNode {
public:
    IntegerNode(int value);

    IntegerNode(const std::string& value);

    IntegerNode(BigInteger value);

    ~IntegerNode() override = default;

public:
    VmNodeType GetNodeType() const override;

    std::string Value() const override;

    void Negate() override;

    BigInteger RealValue() const;

public:
    std::shared_ptr<VmNode> operator+(const VmNode& other) const override;

    std::shared_ptr<VmNode> operator-(const VmNode& other) const override;

    std::shared_ptr<VmNode> operator*(const VmNode& other) const override;

    std::shared_ptr<VmNode> operator/(const VmNode& other) const override;

    std::shared_ptr<VmNode> operator%(const VmNode& other) const override;

public:
    bool operator<(const VmNode& other) const override;

    bool operator>(const VmNode& other) const override;

    bool operator<=(const VmNode& other) const override;

    bool operator>=(const VmNode& other) const override;

    bool operator!=(const VmNode& other) const override;

    bool operator==(const VmNode& other) const override;

private:
    BigInteger _value;
};

class ArrayNode : public VmNode {
public:
    ArrayNode(size_t size);

    ~ArrayNode() override = default;

public:
    void Negate() override {
        throw std::runtime_error("bad operation with array");
    };

public:
    std::shared_ptr<VmNode> operator+(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    std::shared_ptr<VmNode> operator-(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    std::shared_ptr<VmNode> operator*(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    std::shared_ptr<VmNode> operator/(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    std::shared_ptr<VmNode> operator%(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

public:
    bool operator<(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    bool operator>(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    bool operator<=(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    bool operator>=(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    bool operator!=(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

    bool operator==(const VmNode& other) const override {
        throw std::runtime_error("bad operation with array");
    }

public:
    VmNodeType GetNodeType() const override;

    std::string Value() const override;

    size_t Size() const;

public:
    const std::shared_ptr<VmNode>& Get(const BigInteger& index) const;
    void Set(const BigInteger& index, std::shared_ptr<VmNode> value);

    const std::shared_ptr<VmNode>& Get(size_t index) const;
    void Set(size_t index, std::shared_ptr<VmNode> value);

private:
    size_t ConvertBigIntegerToSizeT(const BigInteger& value) const;

private:
    std::vector<std::shared_ptr<VmNode>> _value;
};
