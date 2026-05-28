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

#ifndef _$_HX_CORE_M9EXCHANGE
#define _$_HX_CORE_M9EXCHANGE

#include <include/config/config.hh>

#include <include/meta/meta.hh>
#include <include/runtime/__memory/forwarding.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
template <class T, class U = T>
constexpr T exchange(T &obj, U &&new_value) noexcept
    requires(std::Meta::can_move_noexcept<T> && std::Meta::can_assign_noexcept<T &, U>)
{
    T old_value = move(obj);
    obj         = forward<U>(new_value);
    return old_value;
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M9EXCHANGE
