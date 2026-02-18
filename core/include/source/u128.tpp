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

#ifndef _$_HX_CORE_M4U128
#define _$_HX_CORE_M4U128

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>

u128::u128()
    : high(0)
    , low(0) {}
u128::u128(u64 val)
    : high(0)
    , low(val) {}
u128::u128(u32 val)
    : high(0)
    , low(static_cast<u64>(val)) {}
u128::u128(u16 val)
    : high(0)
    , low(static_cast<u64>(val)) {}
u128::u128(u8 val)
    : high(0)
    , low(static_cast<u64>(val)) {}
u128::u128(i64 val)
    : high(0)
    , low(static_cast<u64>(val)) {}
u128::u128(i32 val)
    : high(0)
    , low(static_cast<u64>(static_cast<u32>(val))) {}
u128::u128(i16 val)
    : high(0)
    , low(static_cast<u64>(static_cast<u16>(val))) {}
u128::u128(i8 val)
    : high(0)
    , low(static_cast<u64>(static_cast<u8>(val))) {}
u128::u128(u64 high, u64 low)
    : high(high)
    , low(low) {}
u128::u128(const i128 &x)
    : high(x.high)
    , low(x.low) {}

u128 u128::operator+(const u128 &other) const {
    u64 sum_low  = low + other.low;
    u64 carry    = (sum_low < low) ? 1 : 0;
    u64 sum_high = high + other.high + carry;
    return {sum_high, sum_low};
}

u128 u128::operator-(const u128 &other) const {
    u64 diff_low  = low - other.low;
    u64 borrow    = (diff_low > low) ? 1 : 0;
    u64 diff_high = high - other.high - borrow;
    return {diff_high, diff_low};
}

u128 u128::operator*(const u128 &other) const {
    u128 p1       = mul_u64_to_u128(low, other.low);
    u128 p2       = mul_u64_to_u128(low, other.high);
    u128 p3       = mul_u64_to_u128(high, other.low);
    u64  high_sum = p1.high + p2.low + p3.low;
    return {high_sum, p1.low};
}

u128 u128::operator/(const u128 &divisor) const {
    if (divisor.high == 0 && divisor.low == 0)
        return u128(0);
    u128 quotient  = 0;
    u128 remainder = 0;
    for (int i = 127; i >= 0; --i) {
        u64 carry      = (remainder.low >> 63) & 1;
        remainder.low  = (remainder.low << 1) | ((high >> (i / 64)) & (1ULL << (i % 64)) ? 1 : 0);
        remainder.high = (remainder.high << 1) | carry;
        if (remainder >= divisor) {
            remainder = remainder - divisor;
            quotient  = quotient | (u128(1) << i);
        }
    }
    return quotient;
}

u128 u128::operator%(const u128 &divisor) const {
    if (divisor.high == 0 && divisor.low == 0)
        return u128(0);
    u128 remainder = 0;
    for (int i = 127; i >= 0; --i) {
        u64 carry      = (remainder.low >> 63) & 1;
        remainder.low  = (remainder.low << 1) | ((high >> (i / 64)) & (1ULL << (i % 64)) ? 1 : 0);
        remainder.high = (remainder.high << 1) | carry;
        if (remainder >= divisor) {
            remainder = remainder - divisor;
        }
    }
    return remainder;
}

u128 u128::operator&(const u128 &other) const { return {high & other.high, low & other.low}; }
u128 u128::operator|(const u128 &other) const { return {high | other.high, low | other.low}; }
u128 u128::operator^(const u128 &other) const { return {high ^ other.high, low ^ other.low}; }
u128 u128::operator~() const { return {~high, ~low}; }

u128 u128::operator<<(int shift) const {
    if (shift >= 128)
        return u128(0);
    if (shift >= 64)
        return {low << (shift - 64), 0};
    u64 new_low  = low << shift;
    u64 carry    = low >> (64 - shift);
    u64 new_high = (high << shift) | carry;
    return {new_high, new_low};
}

u128 u128::operator>>(int shift) const {
    if (shift >= 128)
        return u128(0);
    if (shift >= 64)
        return {0, high >> (shift - 64)};
    u64 new_high = high >> shift;
    u64 carry    = high << (64 - shift);
    u64 new_low  = (low >> shift) | carry;
    return {new_high, new_low};
}

bool u128::operator==(const u128 &other) const { return high == other.high && low == other.low; }
bool u128::operator!=(const u128 &other) const { return !(*this == other); }
bool u128::operator<(const u128 &other) const {
    return high < other.high || (high == other.high && low < other.low);
}
bool u128::operator>(const u128 &other) const { return other < *this; }
bool u128::operator<=(const u128 &other) const { return !(*this > other); }
bool u128::operator>=(const u128 &other) const { return !(*this < other); }

u128 &u128::operator=(const u128 &other) {
    high = other.high;
    low  = other.low;
    return *this;
}
u128 &u128::operator+=(const u128 &other) { return *this = *this + other; }
u128 &u128::operator-=(const u128 &other) { return *this = *this - other; }
u128 &u128::operator*=(const u128 &other) { return *this = *this * other; }
u128 &u128::operator/=(const u128 &other) { return *this = *this / other; }
u128 &u128::operator%=(const u128 &other) { return *this = *this % other; }
u128 &u128::operator&=(const u128 &other) { return *this = *this & other; }
u128 &u128::operator|=(const u128 &other) { return *this = *this | other; }
u128 &u128::operator^=(const u128 &other) { return *this = *this ^ other; }
u128 &u128::operator<<=(int shift) { return *this = *this << shift; }
u128 &u128::operator>>=(int shift) { return *this = *this >> shift; }

u128 &u128::operator++() { return *this += u128(1); }
u128  u128::operator++(int) {
    u128 temp = *this;
    ++*this;
    return temp;
}
u128 &u128::operator--() { return *this -= u128(1); }
u128  u128::operator--(int) {
    u128 temp = *this;
    --*this;
    return temp;
}

u128 u128::operator+() const { return *this; }
u128 u128::operator-() const { return u128(0) - *this; }

u128 u128::mul_u64_to_u128(u64 a, u64 b) {
    u32 a0    = a & 0xFFFFFFFF;
    u32 a1    = a >> 32;
    u32 b0    = b & 0xFFFFFFFF;
    u32 b1    = b >> 32;
    u64 p00   = static_cast<u64>(a0) * b0;
    u64 p01   = static_cast<u64>(a0) * b1;
    u64 p10   = static_cast<u64>(a1) * b0;
    u64 p11   = static_cast<u64>(a1) * b1;
    u64 low   = p00;
    u64 carry = 0;
    u64 sum1  = low + ((p01 & 0xFFFFFFFF) << 32);
    if (sum1 < low)
        carry += 1;
    low = sum1;
    carry += p01 >> 32;
    u64 sum2 = low + ((p10 & 0xFFFFFFFF) << 32);
    if (sum2 < low)
        carry += 1;
    low = sum2;
    carry += p10 >> 32;
    u64 high = p11 + carry;
    return {high, low};
}

#endif  // _$_HX_CORE_M4U128