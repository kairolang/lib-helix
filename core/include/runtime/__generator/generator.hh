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
