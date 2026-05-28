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

#ifndef _$_HX_CORE_M12CONVERSIONS
#define _$_HX_CORE_M12CONVERSIONS

#include <include/config/config.hh>

#include <include/meta/meta.hh>
#include <include/runtime/__memory/forwarding.hh>


H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
template <typename T>
    requires(!std::Meta::is_lval_reference<T>)
constexpr std::Meta::ref_as_ptr<T> as_pointer(T &&ref) noexcept {
    return &ref;
}

template <typename T>
constexpr T &&as_reference(T *ptr) noexcept {
    return *ptr;
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M12CONVERSIONS
