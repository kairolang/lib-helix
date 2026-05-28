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
