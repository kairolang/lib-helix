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

#ifndef _$_HX_CORE_M5PANIC
#define _$_HX_CORE_M5PANIC

#include <source_location>
#include <include/config/config.hh>

#include <include/runtime/__panic/panic_fwd.hh>
#include <include/runtime/__panic/panic_config.hh>
#include <include/runtime/__panic/frame_context.hh>
#include <include/runtime/__panic/frame.hh>
#include <include/runtime/__panic/panic_interfaces.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename T>
static constexpr void crash(
    const T& error,
    const libcxx::source_location& location = libcxx::source_location::current()
) {
    if constexpr (Panic::Interface::Panicking<T>) {
        auto frame = std::Panic::Frame(error, location.file_name(), location.line());
        frame.show_trace = false;
        HX_FN_Vi_Q5_13_kairopanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv(&frame);
        throw error;
    } else {
        throw error;
    }

    throw error;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5PANIC
