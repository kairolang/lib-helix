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