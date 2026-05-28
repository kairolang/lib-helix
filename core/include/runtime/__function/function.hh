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

#ifndef _$_HX_CORE_M8FUNCTION
#define _$_HX_CORE_M8FUNCTION

#include <include/runtime/__function/function_impl.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// \typedef Function
///
/// A type alias for the `$function` class, providing a more concise and readable syntax for
/// defining function objects with specific signatures. Also allows for usage within the language
/// without function semantics.
template <typename Rt, typename... Tp>
using Function = $function<Rt(Tp...)>;

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M8FUNCTION
