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

#ifndef _$_HX_CORE_M5PRINT
#define _$_HX_CORE_M5PRINT

#include <include/config/config.hh>

#include <include/meta/meta.hh>
#include <include/types/string/string.hh>
#include <include/runtime/__io/__print/endl.hh>
#include <include/runtime/__memory/memory.hh>
#include <include/meta/type_properties.hh>
#include <include/meta/const_traits.hh>

H_NAMESPACE_BEGIN

// `string` holds UTF-8 bytes and print writes them verbatim, so no locale is
// involved; only the Windows console needs telling that the bytes are UTF-8.
inline void enable_utf8_output() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

template <typename... Args>
void print(Args &&...t) {
    if constexpr (sizeof...(t) == 0) {
        libcxx::fputc('\n', stdout);
        return;
    }

    (
        [&] {
            const string str = std::to_string(t);
            libcxx::fwrite(str.raw(), 1, str.size(), stdout);
        }(),
        ...);

    if constexpr (sizeof...(t) > 0) {
        using LastArg = libcxx::tuple_element_t<sizeof...(t) - 1, tuple<Args...>>;
        if constexpr (!std::Meta::same_as<std::Meta::const_volatile_removed<LastArg>, std::endl>) {
            libcxx::fputc('\n', stdout);
        }
    }
}

H_STD_NAMESPACE_BEGIN
using ::H_NAMESPACE::print;

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5PRINT
