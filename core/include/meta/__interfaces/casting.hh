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

#ifndef _$_HX_CORE_M7CASTING
#define _$_HX_CORE_M7CASTING

#include <include/config/config.hh>

#include <include/meta/traits.hh>
#include <include/meta/type_properties.hh>
#include <include/types/string/char_traits.hh>

#include "ostream_support.hh"

// ------- forward declarations ------- //
H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {
template <typename CharT, typename Traits = libcxx::char_traits<CharT>>
    requires CharTraits<Traits, CharT>
class basic;
}

H_STD_NAMESPACE_END
using string = std::String::basic<wchar_t>;
H_NAMESPACE_END
// ----- forward declarations end ----- //

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Interface {
template <typename T, typename U>
concept SupportsPointerCast = requires(T from) {
    dynamic_cast<U>(from);  // Dynamic cast requirement
};

template <typename T, typename U>
concept Castable = requires(T t, U *u) {
    { t.operator$cast(u) } -> std::Meta::is_same_as<U>;  // cast to the requested type
} || requires(T t, U *u) {
    { t.operator U() } -> std::Meta::is_same_as<U>;  // call the implicit cast
};

template <typename T>
concept ConvertibleToString = SupportsOStream<T> || Castable<T, string>;
}  // namespace Interface

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7CASTING
