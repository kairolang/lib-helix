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

#ifndef _$_HX_CORE_M10ERROR_BASE
#define _$_HX_CORE_M10ERROR_BASE

#include <include/config/config.hh>

#include <include/runtime/__memory/memory.hh>
#include <include/types/string/basic.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Error {
class BaseError /* with Panicking */ {
  public:
    BaseError() = default;

    BaseError(const BaseError &other)     = default;
    BaseError(BaseError &&other) noexcept = default;

    auto operator=(const BaseError &other) -> BaseError&    = default;
    auto operator=(BaseError &&other) noexcept -> BaseError& = default;

    virtual ~BaseError() = default;

    [[nodiscard]] virtual inline auto operator$panic() const -> string { return L"An error occurred."; }
    [[nodiscard]] virtual inline auto operator$cast(string * /*unused*/) const -> string = 0;  // string cast
};
};  // namespace Error

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M10ERROR_BASE
