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

#ifndef _$_HX_CORE_M20STATE_MISMATCH_ERROR
#define _$_HX_CORE_M20STATE_MISMATCH_ERROR

#include <include/config/config.hh>

#include <include/types/string/basic.hh>

#include "error_base.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Error {
class StateMismatchError : public BaseError {
  public:
    StateMismatchError() = default;

    StateMismatchError(string msg)  // NOLINT(google-explicit-constructor)
        : msg(std::Memory::move(msg)) {}

    StateMismatchError(const StateMismatchError &other)     = default;
    StateMismatchError(StateMismatchError &&other) noexcept = default;

    StateMismatchError &operator=(const StateMismatchError &other)     = default;
    StateMismatchError &operator=(StateMismatchError &&other) noexcept = default;

    ~StateMismatchError() override = default;

    [[nodiscard]] string operator$panic() const override { return msg; }
    [[nodiscard]] string operator$cast(string * /*unused*/) const override { return msg; }

  private:
    string msg;
};
};  // namespace Error

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M20STATE_MISMATCH_ERROR
