/// --- The Kairo Project -------------------------------------------------- ///
///
///   Part of the Kairo Project, under the Apache License v2.0 with the
///   Kairo Runtime Library Exception.
///
///   See: https://www.kairolang.org/LICENSE.txt
///   SPDX-License-Identifier: Apache-2.0 WITH KAIRO-RUNTIME-EXCEPTION
///   Copyright (c) 2026 Dhruvan Kartik
///
/// ------------------------------------------------------------------------ ///

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
