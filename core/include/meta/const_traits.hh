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

#ifndef _$_HX_CORE_M12CONST_TRAITS
#define _$_HX_CORE_M12CONST_TRAITS

#include <include/config/config.hh>

#include "remove_reference.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {

    template <typename T>
    struct remove_const {
        using type = T;
    };

    template <typename T>
    struct remove_const<const T> {
        using type = T;
    };

    template <typename T>
    struct remove_const_volatile {
        using type = T;
    };

    template <typename T>
    struct remove_const_volatile<const T> {
        using type = T;
    };

    template <typename T>
    struct remove_const_volatile<volatile T> {
        using type = T;
    };

    template <typename T>
    struct remove_const_volatile<const volatile T> {
        using type = T;
    };

    template <typename T>
    struct remove_cvref {
        using type = typename remove_const_volatile<typename remove_reference<T>::type>::type;
    };

    template <class T>
    struct add_const {
        using type = const T;
    };

    template <class T>
    struct add_const_volatile {
        using type = const T;
    };

    template <class T>
    struct add_cvref {
        using type = const T;
    };
}  // namespace _types

template <class T>
using const_removed = typename _types::remove_const<T>::type;

template <typename T>
using const_volatile_removed = typename _types::remove_const_volatile<T>::type;

template <typename T>
using cvref_removed = typename _types::remove_cvref<T>::type;

template <class T>
using as_const = typename _types::add_const<T>::type;

template <class T>
using as_const_volatile = typename _types::add_const_volatile<T>::type;

template <class T>
using as_cvref = typename _types::add_cvref<T>::type;
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M12CONST_TRAITS
