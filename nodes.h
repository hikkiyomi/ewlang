#pragma once

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
