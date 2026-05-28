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

#ifndef _$_HX_CORE_M9GENERATOR
#define _$_HX_CORE_M9GENERATOR

#include <include/config/config.hh>

#include <include/runtime/__generator/generator_impl.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <LIBCXX_NAMESPACE::movable T>
using Generator = $generator<T>;

template <LIBCXX_NAMESPACE::movable T>
inline T next($generator<T> &gen) {
    auto iter = gen.begin();
    return *iter;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M9GENERATOR
