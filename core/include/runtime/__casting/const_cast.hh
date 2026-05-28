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

#ifndef _$_HX_CORE_M10CONST_CAST
#define _$_HX_CORE_M10CONST_CAST

#include <include/config/config.hh>

#include <include/meta/meta.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename Ty, typename Up>
const Ty &as_const(Up &value);

template <typename Ty, typename Up>
const Ty &as_const(const Up &value);

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M10CONST_CAST
