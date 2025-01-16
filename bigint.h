#pragma once

#include <string>

class BigInteger {
public:
    BigInteger();

    BigInteger(int value);

    BigInteger(const std::string& value, bool is_negative);

    BigInteger(std::string&& value, bool is_negative);

    BigInteger(const BigInteger& other);

    BigInteger(BigInteger&& other);

public:
    BigInteger operator=(BigInteger other);

public:
    BigInteger operator+(const BigInteger& other) const;

    BigInteger operator-(const BigInteger& other) const;

    BigInteger operator*(const BigInteger& other) const;

    BigInteger operator/(const BigInteger& other) const;

    BigInteger operator%(const BigInteger& other) const;

public:
    bool operator<(const BigInteger& other) const;

    bool operator>(const BigInteger& other) const;

    bool operator<=(const BigInteger& other) const;

    bool operator>=(const BigInteger& other) const;

    bool operator!=(const BigInteger& other) const;

    bool operator==(const BigInteger& other) const;

public:
    std::string Value() const;

    void Negate();

private:
    // For 0 <= k < 10
    std::string operator*(int k) const;

private:
    std::string _value;
    bool _is_negative;
};
