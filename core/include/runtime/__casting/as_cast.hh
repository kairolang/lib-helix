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
///------------------------------------------------------------------------------------ Kairo ---///
/// \file cast.hh                                                                                ///
///                                                                                              ///
/// Provides `as` casting semantics in Kairo, enabling safe and versatile casting operations.    ///
/// These include standard type casting (`as_cast`), const casting (`as_const`), and unsafe      ///
/// reinterpretation casting (`as_unsafe`). This framework is designed to integrate seamlessly   ///
/// with Kairo's runtime and type system, supporting dynamic, static, and user-defined casting   ///
/// mechanisms where appropriate.                                                                ///
///                                                                                              ///
/// ### Purpose                                                                                  ///
/// - Facilitates the use of Kairo-specific `as` casting constructs.                             ///
/// - Offers a type-safe interface for performing various cast operations, adhering to Kairo's   ///
///   runtime semantics and ensuring compatibility with custom user-defined types.               ///
///                                                                                              ///
/// ### Features                                                                                 ///
/// #### `as_cast`                                                                               ///
/// - The general-purpose casting function, supporting:                                          ///
///   - Const casting.                                                                           ///
///   - Pointer casting (dynamic and static based on type constraints).                          ///
///   - Reference casting.                                                                       ///
///   - Kairo-specific `operator$cast` for user-defined casting.                                 ///
///   - Default static casting as a fallback.                                                    ///
///                                                                                              ///
/// #### `as_const`                                                                              ///
/// - Ensures that a value is treated as const, regardless of its current qualifiers.            ///
/// - Can be used with both mutable and immutable inputs.                                        ///
///                                                                                              ///
/// #### `as_unsafe`                                                                             ///
/// - Enables unsafe reinterpretation of types using `reinterpret_cast`.                         ///
/// - Primarily intended for low-level operations where type safety is explicitly disregarded.   ///
///                                                                                              ///
/// ### Examples                                                                                 ///
/// ```kairo                                                                                     ///
/// let x: i32 = 42;                                                                             ///
/// let y: i32* = x as i32*;             // Uses `as_cast` for pointer casting.                  ///
/// let z: const i32* = x as const i32*; // Uses `as_const` for const pointer casting.           ///
/// let u: usize = y as unsafe usize;    // Uses `as_unsafe` for reinterpretation.               ///
/// ```                                                                                          ///
///                                                                                              ///
/// ### Notes                                                                                    ///
/// - The `as_cast` function intelligently selects the appropriate casting method based on type  ///
///   traits and user-defined interfaces, ensuring both safety and flexibility.                  ///
/// - Unsafe casting (`as_unsafe`) should be used sparingly and only when absolutely necessary.  ///
///                                                                                              ///
/// ### Limitations                                                                              ///
/// - While `as_cast` supports dynamic and static casting for pointers, it relies on type        ///
///   information and user-defined constraints to determine the safest approach.                 ///
/// - The `as_unsafe` function bypasses all safety checks, and misuse can lead to ub.            ///
///                                                                                              ///
/// ### Implementation Details                                                                   ///
/// - `as_cast` leverages interfaces like `SupportsPointerCast` and `Castable` to ensure         ///
///   compatibility with Kairo-specific features.                                                ///
/// - `as_const` and `as_unsafe` provide implementations for const and unsafe casting.           ///
///                                                                                              ///
///-------------------------------------------------------------------------------- lib-helix ---///

#ifndef _$_HX_CORE_M7AS_CAST
#define _$_HX_CORE_M7AS_CAST

#include <include/config/config.hh>

#include <include/meta/meta.hh>
#include <include/runtime/__casting/const_cast.hh>
#include <include/runtime/__casting/unsafe_cast.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// Performs a cast from type `Up` to type `Ty`, selecting the most appropriate casting mechanism.
/// \tparam Ty The target type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return The value cast to the target type.
template <typename Ty, typename Up>
Ty as_cast(Up &value);

/// Performs a cast from type `Up` to type `Ty`, selecting the most appropriate casting mechanism.
/// \tparam Ty The target type.
/// \tparam Up The source type.
/// \param value The value to be cast.
/// \return The value cast to the target type, removing const qualifiers.
template <typename Ty, typename Up>
Ty as_cast(const Up &value);

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7AS_CAST