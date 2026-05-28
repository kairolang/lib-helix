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

#ifndef _$_HX_CORE_M4ENDL
#define _$_HX_CORE_M4ENDL

#include <include/config/config.hh>

#include <include/types/string/string.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

class endl {
  public:
    endl &operator=(const endl &) = delete;
    endl &operator=(endl &&)      = delete;
    endl(const endl &)            = delete;
    endl(endl &&)                 = delete;

    endl()  = default;
    ~endl() = default;

    explicit endl(string end)
        : end_l(std::Memory::move(end)) {}

    explicit endl(const wchar_t *end)
        : end_l((end != nullptr) ? end : L"\n") {}

    explicit endl(wchar_t end)
        : end_l(end, 1) {}

  private:
    string end_l = L"\n";
};

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M4ENDL
