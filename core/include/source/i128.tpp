///--- The Kairo Project ------------------------------------------------------------------------///
///                                                                                              ///
///   Part of the Kairo Project, under the Attribution 4.0 International license (CC BY 4.0).    ///
///   You are allowed to use, modify, redistribute, and create derivative works, even for        ///
///   commercial purposes, provided that you give appropriate credit, and indicate if changes    ///
///   were made.                                                                                 ///
///                                                                                              ///
///   For more information on the license terms and requirements, please visit:                  ///
///     https://creativecommons.org/licenses/by/4.0/                                             ///
///                                                                                              ///
///   SPDX-License-Identifier: CC-BY-4.0                                                         ///
///   Copyright (c) 2024 The Kairo Project (CC BY 4.0)                                           ///
///                                                                                              ///
///-------------------------------------------------------------------------------- lib-helix ---///

#ifndef _$_HX_CORE_M4I128_TPP
#define _$_HX_CORE_M4I128_TPP

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>

i128::i128()
    : high(0)
    , low(0) {}
i128::i128(u64 val)
    : high(0)
    , low(val) {}
i128::i128(u32 val)
    : high(0)
    , low(static_cast<u64>(val)) {}
i128::i128(u16 val)
    : high(0)
    , low(static_cast<u64>(val)) {}
i128::i128(u8 val)
    : high(0)
    , low(static_cast<u64>(val)) {}
i128::i128(i64 val)
    : high(val < 0 ? 0xFFFFFFFFFFFFFFFF : 0)
    , low(static_cast<u64>(val)) {}
i128::i128(i32 val)
    : high(val < 0 ? 0xFFFFFFFFFFFFFFFF : 0)
    , low(static_cast<u64>(static_cast<i64>(val))) {}
i128::i128(i16 val)
    : high(val < 0 ? 0xFFFFFFFFFFFFFFFF : 0)
    , low(static_cast<u64>(static_cast<i64>(val))) {}
i128::i128(i8 val)
    : high(val < 0 ? 0xFFFFFFFFFFFFFFFF : 0)
    , low(static_cast<u64>(static_cast<i64>(val))) {}
i128::i128(const u128 &x)
    : high(x.high)
    , low(x.low) {}
i128::i128(u64 high, u64 low)
    : high(high)
    , low(low) {}

bool i128::is_negative() const { return (high & 0x8000000000000000) != 0; }

i128 i128::operator+(const i128 &other) const {
    u64 sum_low  = low + other.low;
    u64 carry    = (sum_low < low) ? 1 : 0;
    u64 sum_high = high + other.high + carry;
    return {sum_high, sum_low};
}

i128 i128::operator-(const i128 &other) const {
    u64 diff_low  = low - other.low;
    u64 borrow    = (diff_low > low) ? 1 : 0;
    u64 diff_high = high - other.high - borrow;
    return {diff_high, diff_low};
}

i128 i128::operator*(const i128 &other) const {
    bool sign      = is_negative() != other.is_negative();
    i128 abs_this  = is_negative() ? -(*this) : *this;
    i128 abs_other = other.is_negative() ? -other : other;
    u128 u_result  = u128(abs_this) * u128(abs_other);
    i128 result    = i128(u_result);
    return sign ? -result : result;
}

i128 i128::operator/(const i128 &other) const {
    bool sign       = is_negative() != other.is_negative();
    i128 abs_this   = is_negative() ? -(*this) : *this;
    i128 abs_other  = other.is_negative() ? -other : other;
    u128 u_quotient = u128(abs_this) / u128(abs_other);
    i128 quotient   = i128(u_quotient);
    return sign ? -quotient : quotient;
}

i128 i128::operator%(const i128 &other) const {
    bool sign        = is_negative();
    i128 abs_this    = is_negative() ? -(*this) : *this;
    i128 abs_other   = other.is_negative() ? -other : other;
    u128 u_remainder = u128(abs_this) % u128(abs_other);
    i128 remainder   = i128(u_remainder);
    return sign ? -remainder : remainder;
}

i128 i128::operator&(const i128 &other) const { return {high & other.high, low & other.low}; }
i128 i128::operator|(const i128 &other) const { return {high | other.high, low | other.low}; }
i128 i128::operator^(const i128 &other) const { return {high ^ other.high, low ^ other.low}; }
i128 i128::operator~() const { return {~high, ~low}; }

i128 i128::operator<<(int shift) const {
    if (shift >= 128)
        return i128(0);
    if (shift >= 64)
        return {low << (shift - 64), 0};
    u64 new_low  = low << shift;
    u64 carry    = low >> (64 - shift);
    u64 new_high = (high << shift) | carry;
    return {new_high, new_low};
}

i128 i128::operator>>(int shift) const {
    if (shift >= 128)
        return is_negative() ? i128(-1) : i128(0);
    if (shift >= 64) {
        u64 new_low  = high >> (shift - 64);
        u64 new_high = is_negative() ? 0xFFFFFFFFFFFFFFFF : 0;
        return {new_high, new_low};
    }
    u64 new_high = high >> shift;
    u64 carry    = high << (64 - shift);
    u64 new_low  = (low >> shift) | carry;
    if (is_negative())
        new_high |= 0xFFFFFFFFFFFFFFFF << (64 - shift);
    return {new_high, new_low};
}

bool i128::operator==(const i128 &other) const { return high == other.high && low == other.low; }
bool i128::operator!=(const i128 &other) const { return !(*this == other); }
bool i128::operator<(const i128 &other) const {
    bool this_neg  = is_negative();
    bool other_neg = other.is_negative();
    if (this_neg != other_neg)
        return this_neg;
    return high < other.high || (high == other.high && low < other.low);
}
bool i128::operator>(const i128 &other) const { return other < *this; }
bool i128::operator<=(const i128 &other) const { return !(*this > other); }
bool i128::operator>=(const i128 &other) const { return !(*this < other); }

i128 &i128::operator=(const i128 &other) {
    high = other.high;
    low  = other.low;
    return *this;
}
i128 &i128::operator+=(const i128 &other) { return *this = *this + other; }
i128 &i128::operator-=(const i128 &other) { return *this = *this - other; }
i128 &i128::operator*=(const i128 &other) { return *this = *this * other; }
i128 &i128::operator/=(const i128 &other) { return *this = *this / other; }
i128 &i128::operator%=(const i128 &other) { return *this = *this % other; }
i128 &i128::operator&=(const i128 &other) { return *this = *this & other; }
i128 &i128::operator|=(const i128 &other) { return *this = *this | other; }
i128 &i128::operator^=(const i128 &other) { return *this = *this ^ other; }
i128 &i128::operator<<=(int shift) { return *this = *this << shift; }
i128 &i128::operator>>=(int shift) { return *this = *this >> shift; }

i128 &i128::operator++() { return *this += i128(1); }
i128  i128::operator++(int) {
    i128 temp = *this;
    ++*this;
    return temp;
}
i128 &i128::operator--() { return *this -= i128(1); }
i128  i128::operator--(int) {
    i128 temp = *this;
    --*this;
    return temp;
}

i128 i128::operator+() const { return *this; }
i128 i128::operator-() const { return i128(0) - *this; }

#endif  // _$_HX_CORE_M4I128