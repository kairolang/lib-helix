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
