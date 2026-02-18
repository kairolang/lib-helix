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

#ifndef _$_HX_CORE_M15TYPE_PROPERTIES
#define _$_HX_CORE_M15TYPE_PROPERTIES

#include <include/config/config.hh>
#include <type_traits>

#include "const_traits.hh"
#include "declval.hh"
#include "integral_constant.hh"
#include "reference_traits.hh"


H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {
    template <typename T>
    struct is_nothrow_move_constructible
        : Meta::_types::integral_constant<bool,
                                          __is_nothrow_constructible(
                                              T, typename _types::add_rvalue_reference<T>::type)> {
    };

    template <typename T, typename Arg>
    struct is_nothrow_assignable
        : Meta::_types::integral_constant<bool, __is_nothrow_assignable(T, Arg)> {};

    template <class T>
    struct is_copy_constructible
        : public Meta::_types::integral_constant<
              bool,
              __is_constructible(
                  T, typename add_lvalue_reference<typename add_const<T>::type>::type)> {};

    template <class T>
    struct is_function
        : public integral_constant<bool, !(is_reference<T>::value || is_const<const T>)> {};

    template <class>
    struct apply_destructible {
        using type = int;
    };

    template <class T>
    struct remove_all_extents_impl {
        using type = T;
    };
    template <class T>
    struct remove_all_extents_impl<T[]> {
        using type = typename remove_all_extents_impl<T>::type;
    };
    template <class T, size_t N>
    struct remove_all_extents_impl<T[N]> {
        using type = typename remove_all_extents_impl<T>::type;
    };

    template <class T>
    using all_extents_removed = typename _types::remove_all_extents_impl<T>::type;

#ifndef _MSC_VER

    template <typename T>
    struct wellformed_destructor {
        template <typename U>
        static std::Meta::true_t
            test(typename apply_destructible<decltype(std::Meta::declval<U &>().~U())>::type);

        template <typename U>
        static std::Meta::false_t test(...);

        static const bool value = decltype(test<T>(12))::value;
    };

#else

    template <typename T>
    struct wellformed_destructor {
        static constexpr bool value = libcxx::is_destructible_v<T>;
    };

#endif

    template <class T, bool>
    struct destructible;

    template <class T>
    struct destructible<T, false>
        : public integral_constant<bool, wellformed_destructor<all_extents_removed<T>>::value> {};

    template <class T>
    struct destructible<T, true> : public std::Meta::true_t {};

    template <class T, bool>
    struct destructible_false;

    template <class T>
    struct destructible_false<T, false> : public destructible<T, is_reference<T>::value> {};

    template <class T>
    struct destructible_false<T, true> : public std::Meta::false_t {};

    template <class T>
    struct is_destructible : public destructible_false<T, is_function<T>::value> {};

    template <class T>
    struct is_destructible<T[]> : public std::Meta::false_t {};

    template <>
    struct is_destructible<void> : public std::Meta::false_t {};
}  // namespace _types

template <class T>
using all_extents_removed = typename _types::all_extents_removed<T>;

template <typename T>  // FIXME( add typename if its not working on msvc )
concept can_move_noexcept =
    _types::is_nothrow_move_constructible<T>::value && libcxx::is_destructible_v<class Tp>;

template <typename T, typename Arg>
concept can_assign_noexcept = _types::is_nothrow_assignable<T, Arg>::value;

template <typename T>
concept can_copy_construct = _types::is_copy_constructible<T>::value;

template <typename T>
concept wellformed_destructor = _types::wellformed_destructor<T>::value;
template <class T>
concept is_function = _types::is_function<T>::value;
template <class T>
concept is_destructible = _types::is_destructible<T>::value;
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M15TYPE_PROPERTIES