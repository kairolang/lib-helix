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
