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

#ifndef _$_HX_CORE_M11CHAR_TRAITS
#define _$_HX_CORE_M11CHAR_TRAITS

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/traits.hh>
#include <include/types/builtins/primitives.hh>
#include <type_traits>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {
template <typename Traits, typename CharT>
concept CharTraits = requires(CharT       &dest_char,
                              const CharT &src_char,
                              const CharT *source,
                              const CharT *target,
                              CharT       *dest_buffer,
                              size_t       count,
                              const CharT &value,
                              i32      code) {
    { Traits::assign(dest_char, src_char) } -> std::Meta::is_same_as<void>;
    { Traits::eq(src_char, dest_char) } -> std::Meta::is_same_as<bool>;
    { Traits::lt(src_char, dest_char) } -> std::Meta::is_same_as<bool>;

    { Traits::compare(source, target, count) } -> std::Meta::is_convertible_to<i32>;
    { Traits::length(source) } -> std::Meta::is_convertible_to<size_t>;
    { Traits::find(source, count, value) } -> std::Meta::is_same_as<const CharT *>;

    { Traits::move(dest_buffer, source, count) } -> std::Meta::is_same_as<CharT *>;
    { Traits::copy(dest_buffer, source, count) } -> std::Meta::is_same_as<CharT *>;
    { Traits::assign(dest_buffer, count, value) } -> std::Meta::is_same_as<CharT *>;

    { Traits::not_eof(code) } -> std::Meta::is_convertible_to<i32>;
    { Traits::to_char_type(code) } -> std::Meta::is_same_as<CharT>;
    { Traits::to_int_type(src_char) } -> std::Meta::is_convertible_to<i32>;
    { Traits::eq_int_type(code, code) } -> std::Meta::is_same_as<bool>;
    { Traits::eof() } -> std::Meta::is_convertible_to<i32>;
};
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M11CHAR_TRAITS
