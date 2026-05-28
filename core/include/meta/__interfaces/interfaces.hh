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

#ifndef _$_HX_CORE_M9INTERFACE
#define _$_HX_CORE_M9INTERFACE

#include "ostream_support.hh"
#include "type_traits.hh"
#include "casting.hh"

#include <include/meta/traits.hh>
#include <include/meta/type_properties.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Interface {
    template <typename self>
    concept RangeCompliant = requires(self $_1738523894814_8910, self $_1738523894814_7822) {
        { ++$_1738523894814_8910 } -> std::Meta::is_convertible_to<self>;
        { $_1738523894814_8910 < $_1738523894814_7822 } -> std ::Meta ::is_convertible_to<bool>;
    };
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M9INTERFACE