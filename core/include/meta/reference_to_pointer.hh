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

#ifndef _$_HX_CORE_M20REFERENCE_TO_POINTER
#define _$_HX_CORE_M20REFERENCE_TO_POINTER

#include <include/config/config.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Meta {
namespace _types {

    template <typename T>
    struct reference_to_pointer {
        using type = T;
    };

    template <typename T>
    struct reference_to_pointer<T &> {
        using type = T *;
    };

    template <typename T>
    struct reference_to_pointer<T &&> {
        using type = T *;
    };
}  // namespace _types

template <typename T>
using ref_as_ptr = typename _types::reference_to_pointer<T>::type;
}  // namespace Meta

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M20REFERENCE_TO_POINTER
