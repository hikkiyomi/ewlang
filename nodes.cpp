#include "nodes.h"

#include <memory>
#include <stdexcept>

#include "vm_definitions.h"

IntegerNode::IntegerNode(int value) : _value(BigInteger(value)) {}

IntegerNode::IntegerNode(const std::string& value)
    : _value(BigInteger(value, value[0] == '-')) {}

IntegerNode::IntegerNode(BigInteger value) : _value(std::move(value)) {}

VmNodeType IntegerNode::GetNodeType() const { return NODE_TYPE_INTEGER; }

std::string IntegerNode::Value() const { return _value.Value(); }

void IntegerNode::Negate() { _value.Negate(); }

BigInteger IntegerNode::RealValue() const { return _value; }

std::shared_ptr<VmNode> IntegerNode::operator+(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return std::make_shared<IntegerNode>(this->RealValue() +
                                         casted.RealValue());
}

std::shared_ptr<VmNode> IntegerNode::operator-(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return std::make_shared<IntegerNode>(this->RealValue() -
                                         casted.RealValue());
}

std::shared_ptr<VmNode> IntegerNode::operator*(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return std::make_shared<IntegerNode>(this->RealValue() *
                                         casted.RealValue());
}

std::shared_ptr<VmNode> IntegerNode::operator/(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return std::make_shared<IntegerNode>(this->RealValue() /
                                         casted.RealValue());
}

std::shared_ptr<VmNode> IntegerNode::operator%(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return std::make_shared<IntegerNode>(this->RealValue() %
                                         casted.RealValue());
}

bool IntegerNode::operator<(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return this->RealValue() < casted.RealValue();
}

bool IntegerNode::operator>(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return this->RealValue() > casted.RealValue();
}

bool IntegerNode::operator<=(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return this->RealValue() <= casted.RealValue();
}

bool IntegerNode::operator>=(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return this->RealValue() >= casted.RealValue();
}

bool IntegerNode::operator!=(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return this->RealValue() != casted.RealValue();
}

bool IntegerNode::operator==(const VmNode& other) const {
    if (this->GetNodeType() != other.GetNodeType()) {
        throw std::runtime_error("summing integer and non-integer");
    }

    const IntegerNode& casted = static_cast<const IntegerNode&>(other);

    return this->RealValue() == casted.RealValue();
}
