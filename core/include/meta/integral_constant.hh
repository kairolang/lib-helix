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

#ifndef _$_HX_CORE_M17INTEGRAL_CONSTANT
#define _$_HX_CORE_M17INTEGRAL_CONSTANT

#include <include/config/config.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {
    template <class T, T v>
    struct integral_constant {
        using value_type               = T;
        using type                     = integral_constant;
        static constexpr const T value = v;
        constexpr                operator value_type() const { return value; }  // NOLINT
        constexpr value_type     operator()() const { return value; }
    };
}  // namespace _types

using true_t  = Meta::_types::integral_constant<bool, true>;
using false_t = Meta::_types::integral_constant<bool, false>;
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M17INTEGRAL_CONSTANT
