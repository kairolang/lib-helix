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

#ifndef _$_HX_CORE_M6TRAITS
#define _$_HX_CORE_M6TRAITS

#include <include/config/config.hh>
#include <utility>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {
    template <class>
    constexpr bool is_const_impl = false;

    template <class T>
    constexpr bool is_const_impl<const T> = true;
}  // namespace _types

template <class Up, class T>
constexpr bool is_convertible = __is_convertible_to(Up, T);

#ifdef __clang__  // clang has an intrinsic for this
template <class T, class Up>
constexpr bool same_as = __is_same(T, Up) && __is_same(Up, T);

#else  // use a custom implementation
template <class, class>
constexpr bool same_as = false;

template <class T>
constexpr bool same_as<T, T> = true;
#endif

template <class Up, class T>
concept is_convertible_to = is_convertible<Up, T> && requires { static_cast<T>(libcxx::declval<Up>()); };

template <class Up, class T>
concept is_same_as = same_as<Up, T>;

template <class T>
concept is_const = _types::is_const_impl<T>;

template <class B, class D>
concept is_derived_of = __is_base_of(B, D);

template <class T>
concept is_class = __is_class(T);

}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M6TRAITS
