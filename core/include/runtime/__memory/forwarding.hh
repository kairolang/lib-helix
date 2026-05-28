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
