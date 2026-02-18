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

#ifndef _$_HX_CORE_M11ALLOCATION
#define _$_HX_CORE_M11ALLOCATION

#include <include/config/config.hh>

#include "forwarding.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Legacy {
template <typename _Tp, typename... _Ty>
constexpr _Tp *_H_RESERVED$new(_Ty &&...t) {          // NOLINT
    return new _Tp(std::Memory::forward<_Ty>(t)...);  // NOLINT
}
}  // namespace Legacy

template <typename _Tp, typename... _Ty>
constexpr _Tp *create(_Ty &&...t) {          // NOLINT
    return new _Tp(std::Memory::forward<_Ty>(t)...);  // NOLINT
}

template <typename _Tp, typename... _Ty>
constexpr _Tp *create(void* dest, _Ty &&...t) {          // NOLINT
    return new (dest) _Tp(std::Memory::forward<_Ty>(t)...);  // NOLINT
}


// make a function called erase which calls c++ delete
template <typename _Tp>
constexpr void destroy(_Tp *ptr) {  // NOLINT
    delete ptr;  // NOLINT
}

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M11ALLOCATION
