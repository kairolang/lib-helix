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

#ifndef _$_HX_CORE_M11TYPE_TRAITS
#define _$_HX_CORE_M11TYPE_TRAITS

#include <include/config/config.hh>

#include <include/meta/traits.hh>
#include <include/meta/type_properties.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Interface {
template <typename T>
concept ClassType = Meta::is_class<T>;

template <class T>
concept ConstType = std::Meta::is_const<T>;

template <typename T>
concept ReferenceableType = std::Meta::is_referenceable<T>;

template <class T>
concept RValueReference = std::Meta::is_rval_reference<T>;

template <class T>
concept LValueReference = std::Meta::is_rval_reference<T>;

template <class T>
concept ReferenceType = std::Meta::is_reference<T>;

template <typename T>
concept MoveConstructible = std::Meta::can_move_noexcept<T>;

template <typename T, typename Arg>
concept NothrowAssignable = std::Meta::can_assign_noexcept<T, Arg>;

template <class T>
concept CopyConstructible = std::Meta::can_copy_construct<T>;
}  // namespace Interface

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M11TYPE_TRAITS
