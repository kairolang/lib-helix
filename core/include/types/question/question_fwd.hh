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

#ifndef _$_HX_CORE_M12QUESTION_FWD
#define _$_HX_CORE_M12QUESTION_FWD

#include <include/config/config.hh>

H_NAMESPACE_BEGIN
template <class T>
class $question;
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

#endif  // _$_HX_CORE_M12QUESTION_FWD
