#include "nodes.h"

#include <memory>
#include <stdexcept>

#include "vm_definitions.h"

IntegerNode::IntegerNode(int value) : _value(value) {}

VmNodeType IntegerNode::GetNodeType() const { return NODE_TYPE_INTEGER; }

std::string IntegerNode::Value() const { return std::to_string(_value); }

void IntegerNode::Negate() { _value = -_value; }

int IntegerNode::RealValue() const { return _value; }

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
