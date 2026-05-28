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

#ifndef _$_HX_CORE_M8QUESTION
#define _$_HX_CORE_M8QUESTION

#include <include/config/config.hh>

#include <include/types/question/question_fwd.hh>
#include <include/types/question/question_impl.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// \typedef Questionable
///
/// A type alias for the `$question` class, providing a more concise and readable syntax for
/// working with quantum types (`?`). This alias also allows developers to use the `Questionable`
/// type in either C++ or Kairo code. exposing the `Questionable` type for interoperability.
template <typename T>
using Questionable = $question<T>;

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M8QUESTION
