#include "nodes.h"

#include <memory>
#include <stdexcept>

#include "bigint.h"
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

ArrayNode::ArrayNode(size_t size, Frame& frame)
    : _value(std::vector<std::weak_ptr<VmNode>>(size)) {
    for (size_t i = 0; i < size; ++i) {
        frame.objects.push_back(std::make_shared<IntegerNode>(0));
        _value[i] = frame.objects.back();
    }
}

VmNodeType ArrayNode::GetNodeType() const { return NODE_TYPE_ARRAY; };

std::string ArrayNode::Value() const {
    std::string result;

    for (size_t i = 0; i < _value.size(); ++i) {
        if (i == _value.size() - 1) {
            result += _value[i].lock()->Value();
        } else {
            result += _value[i].lock()->Value() + ", ";
        }
    }

    return "[ " + result + " ]";
}

size_t ArrayNode::Size() const { return _value.size(); }

const std::weak_ptr<VmNode>& ArrayNode::Get(size_t index) const {
    return _value[index];
}

void ArrayNode::Set(size_t index, std::weak_ptr<VmNode> value) {
    _value[index] = value;
}

const std::weak_ptr<VmNode>& ArrayNode::Get(const BigInteger& index) const {
    return Get(ConvertBigIntegerToSizeT(index));
}

void ArrayNode::Set(const BigInteger& index, std::weak_ptr<VmNode> value) {
    Set(ConvertBigIntegerToSizeT(index), value);
}

size_t ArrayNode::ConvertBigIntegerToSizeT(const BigInteger& value) const {
    if (value >= BigInteger(Size())) {
        throw std::runtime_error("index out of range while accessing array");
    }

    return stoi(value.Value());
};
