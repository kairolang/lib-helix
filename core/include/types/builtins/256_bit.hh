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

#ifndef _$_HX_CORE_M7256_BIT
#define _$_HX_CORE_M7256_BIT

#include <include/config/config.hh>

#include "bitset.hh"

H_NAMESPACE_BEGIN

// constexpr u256 operator"" _u256(const char* str, unsigned long len) noexcept {
//     return u256::from_cstr(str, len);
// }

// constexpr i256 operator"" _i256(const char* str, unsigned long len) noexcept {
//     bool negative = false;
//     unsigned long start = 0;
//     if(len > 0 && str[0] == '-') {
//         negative = true;
//         start = 1;
//     }
//     u256 abs_val = u256::from_cstr(str + start, len - start);
//     i256 signed_val = u256_to_i256(abs_val);
//     // We avoid ambiguity in unary minus by subtracting from zero.
//     return negative ? (i256() - signed_val) : signed_val;
// }

// // Overload for u256 when the literal fits in an unsigned long long.
// constexpr u256 operator"" _u256(unsigned long long v) noexcept {
//     return u256(
//         static_cast<u64>(0),
//         static_cast<u64>(0),
//         static_cast<u64>(0),
//         static_cast<u64>(v)
//     );
// }

// // Overload for i256 when the literal fits in an unsigned long long.
// constexpr i256 operator"" _i256(unsigned long long v) noexcept {
//     return i256(
//         static_cast<i64>(0),
//         static_cast<i64>(0),
//         static_cast<i64>(0),
//         static_cast<i64>(v)
//     );
// }

H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M7256_BIT
