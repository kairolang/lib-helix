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

inline void enable_utf8_output() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    libcxx::setlocale(LC_ALL, ".UTF-8");
#else
    if (!libcxx::setlocale(LC_ALL, "en_US.UTF-8"))
        libcxx::setlocale(LC_ALL, "C.UTF-8");
#endif
}

template <typename... Args>
void print(Args &&...t) {
    if constexpr (sizeof...(t) == 0) {
        wprintf(L"\n");
        return;
    }

    ((wprintf(L"%ls",
              [&]() -> const string {
                  auto str = std::to_string(t);
                  return str;
              }()
                           .raw())),
     ...);

    if constexpr (sizeof...(t) > 0) {
        using LastArg = libcxx::tuple_element_t<sizeof...(t) - 1, tuple<Args...>>;
        if constexpr (!std::Meta::same_as<std::Meta::const_volatile_removed<LastArg>, std::endl>) {
            wprintf(L"\n");
        }
    }
}

H_STD_NAMESPACE_BEGIN
using ::H_NAMESPACE::print;

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5PRINT
