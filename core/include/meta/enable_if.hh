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

#ifndef _$_HX_CORE_M9ENABLE_IF
#define _$_HX_CORE_M9ENABLE_IF

#include <include/config/config.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
template <bool, class T = void>
struct enable_if {};

template <class T>
struct enable_if<true, T> {
    using type = T;
};
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M9ENABLE_IF
