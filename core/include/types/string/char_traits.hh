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

#ifndef _$_HX_CORE_M11CHAR_TRAITS
#define _$_HX_CORE_M11CHAR_TRAITS

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/traits.hh>
#include <include/types/builtins/primitives.hh>
#include <type_traits>
#include <compare>
#include <cwchar>
#include <ios>

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

/// Traits for byte strings. libc++ (LLVM 19+) dropped the primary
/// `char_traits<T>` template, so `char_traits<unsigned char>` no longer exists;
/// `string` is a byte string and needs its own. Bytes compare unsigned, and
/// `to_int_type` stays in 0..255 so it can never collide with `eof()`.
struct byte_traits {
    using char_type  = unsigned char;
    using int_type   = int;
    using off_type   = libcxx::streamoff;
    using pos_type   = libcxx::streampos;
    using state_type = libcxx::mbstate_t;
    using comparison_category = libcxx::strong_ordering;

    static constexpr void assign(char_type &d, const char_type &s) noexcept { d = s; }
    static constexpr bool eq(char_type a, char_type b) noexcept { return a == b; }
    static constexpr bool lt(char_type a, char_type b) noexcept { return a < b; }

    static constexpr int compare(const char_type *a, const char_type *b, size_t n) noexcept {
        for (size_t i = 0; i < n; ++i) {
            if (a[i] != b[i]) {
                return a[i] < b[i] ? -1 : 1;
            }
        }
        return 0;
    }

    static constexpr size_t length(const char_type *s) noexcept {
        size_t n = 0;
        while (s[n] != 0) {
            ++n;
        }
        return n;
    }

    static constexpr const char_type *find(const char_type *s, size_t n, const char_type &c) noexcept {
        for (size_t i = 0; i < n; ++i) {
            if (s[i] == c) {
                return s + i;
            }
        }
        return nullptr;
    }

    static constexpr char_type *move(char_type *d, const char_type *s, size_t n) noexcept {
        if (n == 0) {
            return d;
        }
        if (libcxx::is_constant_evaluated()) {
            if (d < s) {
                for (size_t i = 0; i < n; ++i) d[i] = s[i];
            } else {
                for (size_t i = n; i > 0; --i) d[i - 1] = s[i - 1];
            }
            return d;
        }
        __builtin_memmove(d, s, n);
        return d;
    }

    static constexpr char_type *copy(char_type *d, const char_type *s, size_t n) noexcept {
        if (libcxx::is_constant_evaluated()) {
            for (size_t i = 0; i < n; ++i) d[i] = s[i];
            return d;
        }
        if (n != 0) {
            __builtin_memcpy(d, s, n);
        }
        return d;
    }

    static constexpr char_type *assign(char_type *d, size_t n, char_type c) noexcept {
        for (size_t i = 0; i < n; ++i) d[i] = c;
        return d;
    }

    static constexpr int_type  not_eof(int_type c) noexcept { return c == eof() ? 0 : c; }
    static constexpr char_type to_char_type(int_type c) noexcept { return static_cast<char_type>(c); }
    static constexpr int_type  to_int_type(char_type c) noexcept { return static_cast<int_type>(c); }
    static constexpr bool      eq_int_type(int_type a, int_type b) noexcept { return a == b; }
    static constexpr int_type  eof() noexcept { return -1; }
};

static_assert(CharTraits<byte_traits, unsigned char>);

template <typename CharT>
struct default_traits {
    using type = libcxx::char_traits<CharT>;
};

template <>
struct default_traits<unsigned char> {
    using type = byte_traits;
};

template <typename CharT>
using default_traits_t = typename default_traits<CharT>::type;
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M11CHAR_TRAITS
