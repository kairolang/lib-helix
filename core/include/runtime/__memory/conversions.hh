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

#define reference(T)      T&;
#define move_reference(T) T&&;

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M12CONVERSIONS
