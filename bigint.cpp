#include "bigint.h"

#include <algorithm>
#include <stdexcept>

BigInteger::BigInteger() : _value("0"), _is_negative(false) {}

BigInteger::BigInteger(int value)
    : _value(std::to_string(value)), _is_negative(value < 0) {}

BigInteger::BigInteger(const std::string& value, bool is_negative)
    : _value(value), _is_negative(is_negative) {}

BigInteger::BigInteger(std::string&& value, bool is_negative)
    : _value(std::move(value)), _is_negative(is_negative) {}

BigInteger::BigInteger(const BigInteger& other)
    : _value(other._value), _is_negative(other._is_negative) {}

BigInteger::BigInteger(BigInteger&& other)
    : _value(std::move(other._value)),
      _is_negative(std::move(other._is_negative)) {}

BigInteger BigInteger::operator=(BigInteger other) {
    std::swap(this->_value, other._value);
    std::swap(this->_is_negative, other._is_negative);
    return *this;
}

BigInteger BigInteger::operator+(const BigInteger& other) const {
    if (_is_negative == other._is_negative) {
        std::string result;
        int remain = 0;

        for (int i = 0; i < std::max(_value.size(), other._value.size()); ++i) {
            int lhs = 0;
            int rhs = 0;

            if (i < _value.size()) {
                lhs = _value[_value.size() - 1 - i] - '0';
            }

            if (i < other._value.size()) {
                rhs = other._value[other._value.size() - 1 - i] - '0';
            }

            int temp = lhs + rhs + remain;

            result.push_back(static_cast<char>(temp % 10 + '0'));
            remain = temp / 10;
        }

        if (remain > 0) {
            result.push_back(static_cast<char>(remain + '0'));
        }

        std::reverse(result.begin(), result.end());

        return BigInteger(result, _is_negative);
    }

    if (_is_negative) {
        return other - BigInteger(_value, false);
    }

    return *this - BigInteger(other._value, false);
}

BigInteger BigInteger::operator-(const BigInteger& other) const {
    if (other._is_negative) {
        return BigInteger(other._value, false) + *this;
    }

    if (_is_negative) {
        return *this + BigInteger(other._value, true);
    }

    if (*this < other) {
        BigInteger result = other - *this;
        result.Negate();

        return result;
    }

    std::string result;
    int taken = 0;

    for (int i = 0; i < _value.size(); ++i) {
        int x = _value[_value.size() - 1 - i] - '0' - taken;
        int y = 0;

        if (i < other._value.size()) {
            y = other._value[other._value.size() - 1 - i] - '0';
        }

        int temp = x - y;

        if (temp < 0) {
            taken = 1;
            temp += 10;
        } else {
            taken = 0;
        }

        result.push_back(static_cast<char>(temp + '0'));
    }

    while (!result.empty() && result.back() == '0') {
        result.pop_back();
    }

    if (result.empty()) {
        result = "0";
    }

    std::reverse(result.begin(), result.end());

    return BigInteger(result, false);
}

std::string BigInteger::operator*(int k) const {
    if (!(0 <= k && k < 10)) {
        throw std::runtime_error("multiplying biginteger by incorrect k");
    }

    std::string result;
    int remain = 0;

    for (int i = _value.size() - 1; i >= 0; --i) {
        int x = _value[i] - '0';
        int temp = x * k + remain;

        result.push_back(static_cast<char>(temp % 10 + '0'));
        remain = temp / 10;
    }

    if (remain > 0) {
        result.push_back(static_cast<char>(remain + '0'));
    }

    std::reverse(result.begin(), result.end());

    return result;
}

BigInteger BigInteger::operator*(const BigInteger& other) const {
    bool is_negative = _is_negative ^ other._is_negative;
    BigInteger sum;

    for (int i = 0; i < other._value.size(); ++i) {
        int x = other._value[other._value.size() - 1 - i] - '0';
        std::string temp = *this * x;

        for (int j = 0; j < i; ++j) {
            temp.push_back('0');
        }

        sum = sum + BigInteger(temp, false);
    }

    if (is_negative) {
        sum.Negate();
    }

    return sum;
}

BigInteger BigInteger::operator/(const BigInteger& other) const {
    if (_is_negative == other._is_negative) {
        BigInteger divisor = BigInteger(other._value, false);
        std::string buffer;
        std::string result;

        for (int i = 0; i < _value.size(); ++i) {
            buffer.push_back(_value[i]);

            BigInteger temp(buffer, false);

            if (temp < divisor) {
                if (!result.empty()) {
                    result.push_back('0');
                }

                continue;
            }

            int toAdd = 1;

            for (int k = 1; k <= 10; ++k) {
                if (temp < BigInteger(divisor * k, false)) {
                    toAdd = k - 1;
                    break;
                }
            }

            result.push_back(toAdd + '0');
            buffer = (temp - BigInteger(divisor * toAdd, false))._value;

            if (buffer == "0") {
                buffer.clear();
            }
        }

        return BigInteger(result, false);
    }

    BigInteger temp =
        BigInteger(_value, false) / BigInteger(other._value, false) + 1;

    temp.Negate();

    return temp;
}

BigInteger BigInteger::operator%(const BigInteger& other) const {
    return *this - (*this / other) * other;
}

bool BigInteger::operator<(const BigInteger& other) const {
    if (*this == other) {
        return false;
    }

    if (_is_negative != other._is_negative) {
        return _is_negative;
    }

    if (_is_negative) {
        return BigInteger(_value, false) > BigInteger(other._value, false);
    }

    if (_value.size() == other._value.size()) {
        for (int i = 0; i < _value.size(); ++i) {
            int x = _value[i] - '0';
            int y = other._value[i] - '0';

            if (x != y) {
                return x < y;
            }
        }
    }

    return _value.size() < other._value.size();
}

bool BigInteger::operator>(const BigInteger& other) const {
    if (*this == other) {
        return false;
    }

    if (_is_negative != other._is_negative) {
        return !_is_negative;
    }

    if (_is_negative) {
        return BigInteger(_value, false) < BigInteger(other._value, false);
    }

    if (_value.size() == other._value.size()) {
        for (int i = 0; i < _value.size(); ++i) {
            int x = _value[i] - '0';
            int y = other._value[i] - '0';

            if (x != y) {
                return x > y;
            }
        }
    }

    return _value.size() > other._value.size();
}

bool BigInteger::operator<=(const BigInteger& other) const {
    return (*this == other) || (*this < other);
}

bool BigInteger::operator>=(const BigInteger& other) const {
    return (*this == other) || (*this > other);
}

bool BigInteger::operator!=(const BigInteger& other) const {
    return !(*this == other);
}

bool BigInteger::operator==(const BigInteger& other) const {
    return this->_value == other._value;
}

std::string BigInteger::Value() const {
    return (_is_negative ? "-" : "") + _value;
}

void BigInteger::Negate() { _is_negative ^= 1; }
