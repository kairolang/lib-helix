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

#ifndef _$_HX_CORE_M15OSTREAM_SUPPORT
#define _$_HX_CORE_M15OSTREAM_SUPPORT

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/traits.hh>
#include <include/meta/type_properties.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Interface {
template <typename T>
concept SupportsOStream = requires(LIBCXX_NAMESPACE::ostream &os, T a) {
    { os << a } -> std::Meta::is_convertible_to<LIBCXX_NAMESPACE::ostream &>;
};
}  // namespace Interface

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M15OSTREAM_SUPPORT
