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

#ifndef _$_HX_CORE_M12TYPE_ERASURE
#define _$_HX_CORE_M12TYPE_ERASURE

#include <include/config/config.hh>

#include "erasure_base.hh"
#include "erasure_impl.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// \fn erase_type
///
/// Erases the type of an object `obj` and returns a `TypeErasure` wrapper.
///
/// ### Purpose
/// The `erase_type` function creates a type-erased wrapper for an object, allowing
/// it to be stored and manipulated without exposing its concrete type. This is useful
/// for working with heterogeneous collections or dynamic polymorphism.
///
/// ### Parameters
/// - `obj`: A pointer to the object to be type-erased.
///
/// ### Returns
/// A `TypeErasure` pointer that wraps the object, providing type-erased access.
///
/// ### Example
/// ```cpp
/// MyType *obj = new MyType();
/// TypeErasure *erased = erase_type(obj);
/// ```
template <typename T>
[[nodiscard]] TypeErasure *erase_type(T *obj) {
    return new TypeErasureImpl<T>(obj);  // NOLINT(cppcoreguidelines-owning-memory)
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M12TYPE_ERASURE
