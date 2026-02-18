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

#ifndef _$_HX_CORE_M8LITERALS
#define _$_HX_CORE_M8LITERALS

#include <include/config/config.hh>

#include "num_data.hh"
#include "primitives.hh"

H_NAMESPACE_BEGIN

constexpr u8 operator""_u8(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned char>::max)),
                 "Literal value too large for u8",
                 "warning")) {
    return static_cast<unsigned char>(v);
}
constexpr i8 operator""_i8(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) &&
                     ((v > __NumData<signed char>::max) || (v < __NumData<signed char>::min)),
                 "Literal value too large for i8",
                 "warning")) {
    return static_cast<signed char>(v);
}

constexpr u16 operator""_u16(const unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned short>::max)),
                 "Literal value too large for u16",
                 "warning")) {
    return static_cast<unsigned short>(v);
}
constexpr i16 operator""_i16(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) &&
                     ((v > __NumData<signed short>::max) || (v < __NumData<signed short>::min)),
                 "Literal value too large for i16",
                 "warning")) {
    return static_cast<signed short>(v);
}

constexpr u32 operator""_u32(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned int>::max)),
                 "Literal value too large for u32",
                 "warning")) {
    return static_cast<unsigned int>(v);
}
constexpr i32 operator""_i32(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) &&
                     ((v > __NumData<signed int>::max) || (v < __NumData<signed int>::min)),
                 "Literal value too large for i32",
                 "warning")) {
    return static_cast<signed int>(v);
}

constexpr u64 operator""_u64(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > __NumData<unsigned long long>::max)),
                 "Literal value too large for u32",
                 "warning")) {
    return v;
}

constexpr i64 operator""_i64(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v > __NumData<signed long long>::max) ||
                                               (v < __NumData<signed long long>::min)),
                 "Literal value too large for i32",
                 "warning")) {
    return static_cast<signed long long>(v);
}

inline u128 operator""_u128(const char *str) noexcept {
    u128 result(0);          // Integer part
    u128 fraction(0);        // Fractional part (before exponent)
    u128 fraction_place(1);  // Denominator for fractional part
    int  base         = 10;  // Default to decimal
    bool has_fraction = false;
    int  exponent     = 0;  // Exponent value
    bool exp_negative = false;

    // Check for prefixes
    if (*str == '0') {
        ++str;
        if (*str == 'x' || *str == 'X') {
            base = 16;
            ++str;
        } else if (*str == 'b' || *str == 'B') {
            base = 2;
            ++str;
        }
    }

    // Parse integer part
    while (*str && *str != '.' && *str != 'e' && *str != 'E') {
        if (base == 16 && (*str >= 'a' && *str <= 'f')) {
            result = result * u128(base) + u128(*str - 'a' + 10);
        } else if (base == 16 && (*str >= 'A' && *str <= 'F')) {
            result = result * u128(base) + u128(*str - 'A' + 10);
        } else if (*str >= '0' && *str <= '9' && (*str - '0' < base)) {
            result = result * u128(base) + u128(*str - '0');
        }
        ++str;
    }

    // Parse fractional part (if any)
    if (*str == '.') {
        has_fraction = true;
        ++str;  // Skip the decimal point
        while (*str && *str != 'e' && *str != 'E') {
            if (base == 16 && (*str >= 'a' && *str <= 'f')) {
                fraction       = fraction * u128(base) + u128(*str - 'a' + 10);
                fraction_place = fraction_place * u128(base);
            } else if (base == 16 && (*str >= 'A' && *str <= 'F')) {
                fraction       = fraction * u128(base) + u128(*str - 'A' + 10);
                fraction_place = fraction_place * u128(base);
            } else if (*str >= '0' && *str <= '9' && (*str - '0' < base)) {
                fraction       = fraction * u128(base) + u128(*str - '0');
                fraction_place = fraction_place * u128(base);
            }
            ++str;
        }
    }

    // Parse exponent (if any)
    if (*str == 'e' || *str == 'E') {
        ++str;  // Skip 'e' or 'E'
        if (*str == '-') {
            exp_negative = true;
            ++str;
        } else if (*str == '+') {
            ++str;  // Skip optional '+'
        }
        while (*str) {
            exponent = exponent * 10 + (*str - '0');
            ++str;
        }
    }

    // Combine integer and fractional parts (for base 10 only, as hex/binary don’t use exponents
    // here)
    if (base == 10 && has_fraction) {
        result = result * fraction_place + fraction;  // Temporarily scale up
    }

    // Apply exponent (multiply or divide by powers of base)
    if (exponent > 0) {
        u128 multiplier(1);
        for (int i = 0; i < exponent; ++i) {
            multiplier = multiplier * u128(base);
        }
        result = (base == 10 && has_fraction) ? result / fraction_place * multiplier
                                              : result * multiplier;
        if (exp_negative) {
            result = result / multiplier;
        }
    }

    return result;  // Return truncated integer part
}

inline i128 operator""_i128(const char *str) noexcept {
    bool is_negative = (*str == '-');
    if (is_negative)
        ++str;  // Skip the '-' sign if present

    i128 result(0);          // Integer part
    i128 fraction(0);        // Fractional part (before exponent)
    i128 fraction_place(1);  // Denominator for fractional part
    int  base         = 10;  // Default to decimal
    bool has_fraction = false;
    int  exponent     = 0;  // Exponent value
    bool exp_negative = false;

    // Check for prefixes
    if (*str == '0') {
        ++str;
        if (*str == 'x' || *str == 'X') {
            base = 16;
            ++str;
        } else if (*str == 'b' || *str == 'B') {
            base = 2;
            ++str;
        }
    }

    // Parse integer part
    while (*str && *str != '.' && *str != 'e' && *str != 'E') {
        if (base == 16 && (*str >= 'a' && *str <= 'f')) {
            result = result * i128(base) + i128(*str - 'a' + 10);
        } else if (base == 16 && (*str >= 'A' && *str <= 'F')) {
            result = result * i128(base) + i128(*str - 'A' + 10);
        } else if (*str >= '0' && *str <= '9' && (*str - '0' < base)) {
            result = result * i128(base) + i128(*str - '0');
        }
        ++str;
    }

    // Parse fractional part (if any)
    if (*str == '.') {
        has_fraction = true;
        ++str;  // Skip the decimal point
        while (*str && *str != 'e' && *str != 'E') {
            if (base == 16 && (*str >= 'a' && *str <= 'f')) {
                fraction       = fraction * i128(base) + i128(*str - 'a' + 10);
                fraction_place = fraction_place * i128(base);
            } else if (base == 16 && (*str >= 'A' && *str <= 'F')) {
                fraction       = fraction * i128(base) + i128(*str - 'A' + 10);
                fraction_place = fraction_place * i128(base);
            } else if (*str >= '0' && *str <= '9' && (*str - '0' < base)) {
                fraction       = fraction * i128(base) + i128(*str - '0');
                fraction_place = fraction_place * i128(base);
            }
            ++str;
        }
    }

    // Parse exponent (if any)
    if (*str == 'e' || *str == 'E') {
        ++str;  // Skip 'e' or 'E'
        if (*str == '-') {
            exp_negative = true;
            ++str;
        } else if (*str == '+') {
            ++str;  // Skip optional '+'
        }
        while (*str) {
            exponent = exponent * 10 + (*str - '0');
            ++str;
        }
    }

    // Combine integer and fractional parts (for base 10 only, as hex/binary don’t use exponents
    // here)
    if (base == 10 && has_fraction) {
        result = result * fraction_place + fraction;  // Temporarily scale up
    }

    // Apply exponent (multiply or divide by powers of base)
    if (exponent > 0) {
        i128 multiplier(1);
        for (int i = 0; i < exponent; ++i) {
            multiplier = multiplier * i128(base);
        }
        result = (base == 10 && has_fraction) ? result / fraction_place * multiplier
                                              : result * multiplier;
        if (exp_negative) {
            result = result / multiplier;
        }
    }

    return is_negative ? -result : result;  // Apply sign and return truncated integer part
}

constexpr f32 operator""_f32(long double v) noexcept { return static_cast<float>(v); }

constexpr f64 operator""_f64(long double v) noexcept { return static_cast<double>(v); }

constexpr f80 operator""_f80(long double v) noexcept { return v; }

constexpr usize operator""_usize(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) &&
                     ((v > kairo::__NumData<usize>::max) || (v < kairo::__NumData<usize>::min)),
                 "Literal value too large for usize",
                 "warning")) {
    return static_cast<usize>(v);
}

constexpr isize operator""_isize(unsigned long long v) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(v)) && ((v < 0) || (v > kairo::__NumData<isize>::max)),
                 "Literal value too large for u32",
                 "warning")) {
    return static_cast<isize>(v);
}

// FIXME: add llvm level support for this
// constexpr f128 operator"" _f128(unsigned long long v) noexcept {
//     return static_cast<signed long long>(v);
// }

H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M8LITERALS
