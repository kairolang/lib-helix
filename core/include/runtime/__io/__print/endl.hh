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
