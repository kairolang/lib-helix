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
[[noreturn]] static constexpr void crash(
    const T& error,
    const libcxx::source_location& location = libcxx::source_location::current()
) {
    if constexpr (Panic::Interface::Panicking<T>) {
        auto frame = std::Panic::Frame(error, location.file_name(), location.line());
        frame.show_trace = false;
        HX_FN_Vi_Q5_13_kairopanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv(&frame);
        throw error; // Ensure the program terminates after the panic handler is executed
    } else {
        throw error;
    }

    throw error;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5PANIC
