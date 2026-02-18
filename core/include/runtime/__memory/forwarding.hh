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

#ifndef _$_HX_CORE_M11FORWARDING
#define _$_HX_CORE_M11FORWARDING

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/remove_reference.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
using libcxx::move;

template <typename T>
inline T &&forward(std::Meta::reference_removed<T> &t) noexcept {
    return static_cast<T &&>(t);
}

template <typename T>
inline T &&forward(std::Meta::reference_removed<T> &&t) noexcept {
    return static_cast<T &&>(t);
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M11FORWARDING
