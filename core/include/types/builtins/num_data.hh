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

#ifndef _$_HX_CORE_M8NUM_DATA
#define _$_HX_CORE_M8NUM_DATA

#include <include/config/config.hh>

H_NAMESPACE_BEGIN

// -----------------------------------------------------------------------------
// __NumData - Specialization-Dependent Numeric Limits
// -----------------------------------------------------------------------------
template <typename T>
struct __NumData {
    static constexpr const T    digits    = 0;
    static constexpr const T    max       = 0;
    static constexpr const T    min       = 0;
    static constexpr const bool is_signed = false;
    static constexpr const bool is_radix  = false;
};

template <>
struct __NumData<unsigned char> {
    static constexpr const unsigned char digits    = 3;
    static constexpr const unsigned char max       = 255;
    static constexpr const unsigned char min       = 0;
    static constexpr const bool          is_signed = false;
    static constexpr const bool          is_radix  = false;
};

template <>
struct __NumData<signed char> {
    static constexpr const signed char digits    = 3;
    static constexpr const signed char max       = 127;
    static constexpr const signed char min       = -127;
    static constexpr const bool        is_signed = true;
    static constexpr const bool        is_radix  = false;
};

template <>
struct __NumData<unsigned short> {
    static constexpr const unsigned short digits    = 5;
    static constexpr const unsigned short max       = 65535;
    static constexpr const unsigned short min       = 0;
    static constexpr const bool           is_signed = false;
    static constexpr const bool           is_radix  = false;
};

template <>
struct __NumData<signed short> {
    static constexpr const signed short digits    = 5;
    static constexpr const signed short max       = 32767;
    static constexpr const signed short min       = -32767;
    static constexpr const bool         is_signed = true;
    static constexpr const bool         is_radix  = false;
};

template <>
struct __NumData<unsigned int> {
    static constexpr const unsigned int digits    = 10;
    static constexpr const unsigned int max       = 4294967295;
    static constexpr const unsigned int min       = 0;
    static constexpr const bool         is_signed = false;
    static constexpr const bool         is_radix  = false;
};

template <>
struct __NumData<signed int> {
    static constexpr const signed int digits    = 10;
    static constexpr const signed int max       = 2147483647;
    static constexpr const signed int min       = -2147483647;
    static constexpr const bool       is_signed = true;
    static constexpr const bool       is_radix  = false;
};

template <>
struct __NumData<unsigned long long> {
    static constexpr const unsigned long long digits    = 20;
    static constexpr const unsigned long long max       = 18446744073709551615ULL;
    static constexpr const unsigned long long min       = 0;
    static constexpr const bool               is_signed = false;
    static constexpr const bool               is_radix  = false;
};

template <>
struct __NumData<signed long long> {
    static constexpr const signed long long digits    = 20;
    static constexpr const signed long long max       = 9223372036854775807LL;
    static constexpr const signed long long min       = -9223372036854775807LL;
    static constexpr const bool             is_signed = true;
    static constexpr const bool             is_radix  = false;
};

H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M8NUM_DATA
