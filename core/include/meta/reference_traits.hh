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

#ifndef _$_HX_CORE_M16REFERENCE_TRAITS
#define _$_HX_CORE_M16REFERENCE_TRAITS

#include <include/config/config.hh>

#include "integral_constant.hh"
#include "traits.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {
    struct is_referenceable_helper {
        template <typename T>
        static T &test(int);  // Prefers this overload if T is referenceable
        template <typename T>
        static false_t test(...);  // Fallback for non-referenceable types
    };

    template <typename T,
              bool =
                  (!Meta::same_as<decltype(_types::is_referenceable_helper::test<T>(0)), false_t>)>
    struct add_rvalue_reference {
        using type = T;
    };

    template <typename T>
    struct add_rvalue_reference<T, true> {
        using type = T &&;
    };

    template <typename T,
              bool =
                  (!Meta::same_as<decltype(_types::is_referenceable_helper::test<T>(0)), false_t>)>
    struct add_lvalue_reference {
        using type = T;
    };

    template <typename T>
    struct add_lvalue_reference<T &&, true> {
        using type = T &;
    };

    template <typename T>
    struct add_lvalue_reference<T, true> {
        using type = T &;
    };

    template <class T>
    struct is_lvalue_reference : public false_t {};

    template <class T>
    struct is_lvalue_reference<T &> : public true_t {};

    template <class T>
    struct is_rvalue_reference : public false_t {};

    template <class T>
    struct is_rvalue_reference<T &&> : public true_t {};

    template <class T>
    struct is_reference : public false_t {};

    template <class T>
    struct is_reference<T &> : public true_t {};

    template <class T>
    struct is_reference<T &&> : public true_t {};

    template <class T>
    struct is_pointer : public false_t {};

    template <class T>
    struct is_pointer<T *> : public true_t {};

    template <class T>
    struct is_pointer<T **> : public true_t {};

}  // namespace _types

template <typename T>
using as_rvalue_reference = typename _types::add_rvalue_reference<T>::type;

template <typename T>
using as_lvalue_reference = typename _types::add_lvalue_reference<T>::type;

template <typename T>  // FIXME( add typename is msvc requires it )
concept is_lval_reference = _types::is_lvalue_reference<T>::value;

template <typename T>
concept is_rval_reference = _types::is_rvalue_reference<T>::value;

template <typename T>
concept is_reference = _types::is_reference<T>::value;

template <typename T>
concept is_pointer = _types::is_pointer<T>::value;

template <typename T>
concept is_referenceable =
    !Meta::same_as<decltype(_types::is_referenceable_helper::test<T>(0)), false_t>;
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M16REFERENCE_TRAITS
