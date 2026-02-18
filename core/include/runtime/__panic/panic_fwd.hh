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

#ifndef _$_HX_CORE_M9PANIC_FWD
#define _$_HX_CORE_M9PANIC_FWD

#include <include/config/config.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Panic {
class Frame;
};

H_STD_NAMESPACE_END

/// \fn HX_FN_Vi_Q5_13_kairopanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv
///
/// \brief Handles panic events triggered during runtime.
///
/// \details The `HX_FN_Vi_Q5_13_kairopanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv` function is
///          invoked when a panic is triggered. It performs necessary operations, such as invoking
///          registered panic hooks or logging the error context.
///
/// \param f A pointer to a `Panic::Frame` object that encapsulates the panic context.
/// \return This function does not return a value.
///
/// \note This function is a forward declaration and is typically invoked by Kairo's internal panic
///       mechanisms. It is not intended for direct use by developers.
///
/// \see Panic::Frame
/// \see ../../panic_handler.kro
///
/// \note The function name follows a specific naming convention to ensure consistency and
///       readability, the ABI scheme is as follows:
///         [_HX] - Kairo prefix.
///         [_FN] - Function entity.
///         [_Vi] - Visibility internal.
///         [_Q5_13_kairopanic_handler]:
///           [_Q]  - Qualified.
///           [5_]  - Length of the namespace (`kairo`).
///           [13_] - Length of the function name (`panic_handler`).
///         [_Q3_5_5_stdPanicFrame_C_PK]:
///           [_Q]  - Qualified parameter.
///           [3_]  - Length of the namespace (`std`).
///           [5_]  - Length of the nested namespace (`Panic`).
///           [5_]  - Length of the entity (`Frame`).
///           [_C]  - Const.
///           [_PK] - Pointer.
///         [_Rv]: Return prefix (`void`).
///
/// \example
/// \code{.kro}
/// // This sets the panic handler function for Kairo's panic system, this can be changed by the
/// //      user to provide custom panic handling.
/// // Add the following directive to the main file of your Kairo program:
/// //     the passed parameter can be any function pointer that satisfies the signature:
/// //     fn (*std::Panic::Frame) -> void
/// #[panic(&HX_FN_Vi_Q5_13_kairopanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv)]
/// \endcode
static void (*HX_FN_Vi_Q5_13_kairopanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv)(
    const std::Panic::Frame * /* f */);
H_NAMESPACE_END

#endif  // _$_HX_CORE_M9PANIC_FWD
