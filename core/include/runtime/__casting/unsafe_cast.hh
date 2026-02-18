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

#ifndef _$_HX_CORE_M11UNSAFE_CAST
#define _$_HX_CORE_M11UNSAFE_CAST

#include <include/config/config.hh>

#include <include/meta/meta.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename Ty, typename Up>
Ty as_unsafe(Up value);

template <typename Ty, typename Up>
const Ty as_unsafe(const Up value)  // NOLINT
    requires(std::Meta::is_const<Up> || std::Meta::is_const<Ty>);

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M11UNSAFE_CAST
