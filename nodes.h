#pragma once

#include "vm_definitions.h"

class IntegerNode : public VmNode {
public:
    IntegerNode(int value);

    ~IntegerNode() override = default;

public:
    VmNodeType GetNodeType() const override;

    std::string Value() const override;

    void Negate() override;

    int RealValue() const;

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
    int _value;
};
