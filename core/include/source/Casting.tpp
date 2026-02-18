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

#ifndef _$_HX_CORE_M7CASTING_TPP
#define _$_HX_CORE_M7CASTING_TPP

#include <include/c++/libc++.hh>
#include <include/config/config.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>


H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename Ty, typename Up>
Ty as_cast(Up &value) {
    if constexpr (std::Meta::is_const<std::Meta::reference_removed<Up>> &&
                  std::Meta::same_as<std::Meta::const_removed<Up>, Ty>) {
        return const_cast<Ty>(value);
    }

    if constexpr (std::Meta::is_pointer<Ty>) {
        if constexpr (std::Interface::SupportsPointerCast<Up, Ty>) {
            return dynamic_cast<Ty>(value);
        }

        return static_cast<Ty>(value);
    }

    if constexpr (std::Interface::Castable<Up, Ty>) {
        return value.operator$cast(static_cast<Ty *>(nullptr));
    }

    // Code below is not reachable, but we need to keep it for the sake of the compiler
    _KAIRO_SUPPRESS_UNREACHABLE_WARN_PUSH
    if constexpr (std::Meta::is_const<Ty>) {
        return const_cast<Ty>(value);
    }

    return static_cast<Ty>(value);
    _KAIRO_SUPPRESS_UNREACHABLE_WARN_POP
}

template <typename Ty, typename Up>
Ty as_cast(const Up &value) {
    if constexpr (std::Meta::same_as<Up, Ty>) {
        return static_cast<Ty>(value);
    }

    _KAIRO_SUPPRESS_UNREACHABLE_WARN_PUSH
    return as_cast<Ty>(const_cast<Up &>(value));
    _KAIRO_SUPPRESS_UNREACHABLE_WARN_POP
}

template <typename Ty, typename Up>
const Ty &as_const(Up &value) {
    return const_cast<const Ty &>(value);
}

template <typename Ty, typename Up>
const Ty &as_const(const Up &value) {
    return static_cast<const Ty &>(value);
}

template <typename Ty, typename Up>
Ty as_unsafe(Up value) {
    return reinterpret_cast<Ty>(value);  // NOLINT
}

template <typename Ty, typename Up>
const Ty as_unsafe(const Up value)  // NOLINT
    requires(std::Meta::is_const<Up> || std::Meta::is_const<Ty>)
{
    return reinterpret_cast<const Ty>(value);  // NOLINT
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7CASTING