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

#ifndef _$_HX_CORE_M12PANIC_CONFIG
#define _$_HX_CORE_M12PANIC_CONFIG

/// \def _HX_MC_Q7_INTERNAL_CRASH_PANIC_M
///
/// A macro for triggering an immediate and unrecoverable panic within the Kairo runtime.
///
/// ### Purpose
/// This macro is designed to:
/// - Immediately construct a `Panic::Frame` object with the provided error, the current file, and
/// the line number.
/// - Directly invoke the panic operator (`operator$panic`) on the `Frame` object, propagating the
/// error and halting further execution.
///
/// ### Behavior
/// - Constructs a `Panic::Frame` using the provided error and contextual information (`__FILE__`,
/// `__LINE__`).
/// - Calls `operator$panic` on the constructed frame, invoking the Kairo panic handler and throwing
/// the managed error object.
///
/// ### Example Usage
/// ```cpp
/// _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(H_STD_NAMESPACE::Error::RuntimeError("Critical error
/// occurred."));
/// ```
/// This results in an immediate panic, halting execution and propagating the error.
///
/// ### Notes
/// - The macro does not return to the caller. It causes the program to terminate or transfer
/// control to a panic handler.
/// - Should be used in critical failure paths where recovery is not possible.
#ifndef _HX_MC_Q7_INTERNAL_CRASH_PANIC_M
#define _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(err) \
    ::kairo::std::Panic::Frame(err, __FILE__, __LINE__).operator$panic();
#endif

/// \def $panic
///
/// A macro for returning a `Panic::Frame` object from a function, encapsulating an error with
/// contextual details.
///
/// ### Purpose
/// This macro simplifies the creation and return of a `Panic::Frame` object, embedding the provided
/// error along with the current file and line number. It integrates seamlessly into Kairo's
/// panic-handling mechanism.
///
/// ### Behavior
/// - Constructs a `Panic::Frame` object using the provided error and contextual information
/// (`__FILE__`, `__LINE__`).
/// - Returns the constructed `Frame` object, enabling the caller to handle or propagate the error.
///
/// ### Example Usage
/// ```cpp
/// $panic(H_STD_NAMESPACE::Error::RuntimeError("Recoverable error occurred."));
/// ```
/// This creates a `Panic::Frame` with the given error and contextual details and returns it to the
/// caller.
///
/// ### Notes
/// - This macro is intended for use in functions where errors can be propagated or handled further
/// up the call stack.
/// - It does not invoke the panic operator directly, allowing the caller to determine the next
/// course of action.
#ifndef _HX_MC_Q7_PANIC_M
#define _HX_MC_Q7_PANIC_M(err) return ::kairo::std::Panic::Frame(err, __FILE__, __LINE__);
#endif

#endif  // _$_HX_CORE_M12PANIC_CONFIG
