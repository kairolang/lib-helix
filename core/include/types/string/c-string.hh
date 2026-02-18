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

#ifndef _$_HX_CORE_M8C_STRING
#define _$_HX_CORE_M8C_STRING

#include <include/config/config.hh>

#include <include/types/builtins/builtins.hh>
#include <include/types/question/question.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {
template <typename T>
constexpr inline T *concat(T *dest, const T *src) noexcept {
    return LIBCXX_NAMESPACE::wcscat(dest, src);
}

template <typename T>
constexpr inline T *concat_n(T *dest, const T *src, usize n) noexcept {
    return LIBCXX_NAMESPACE::wcsncat(dest, src, n);
}

template <typename T>
constexpr inline T *copy(T *dest, const T *src) noexcept {
    return LIBCXX_NAMESPACE::wcscpy(dest, src);
}

template <typename T>
constexpr inline T *copy_n(T *dest, const T *src, usize n) noexcept {
    return LIBCXX_NAMESPACE::wcsncpy(dest, src, n);
}

template <typename T>
constexpr inline std::Questionable<T *> find(const T *str, int c) noexcept {
    T *result = LIBCXX_NAMESPACE::wcschr(str, c);
    return result ? result : null;
}

template <typename T>
constexpr inline std::Questionable<T *> find_last(const T *str, int c) noexcept {
    T *result = LIBCXX_NAMESPACE::wcsrchr(str, c);
    return result ? result : null;
}

template <typename T>
constexpr inline std::Questionable<T *> find_any(const T *str, const T *accept) noexcept {
    T *result = LIBCXX_NAMESPACE::wcspbrk(str, accept);
    return result ? result : null;
}

template <typename T>
constexpr inline std::Questionable<T *> find_sub(const T *haystack, const T *needle) noexcept {
    T *result = LIBCXX_NAMESPACE::wcsstr(haystack, needle);
    return result ? result : null;
}

template <typename T>
constexpr inline vec<T *> split(T *str, const T *delim) noexcept {
    vec<T *> tokens;
    T       *token = LIBCXX_NAMESPACE::wcstok(str, delim, nullptr);
    while (token) {
        tokens.push_back(token);
        token = LIBCXX_NAMESPACE::wcstok(nullptr, delim, nullptr);
    }
    return tokens;
}

template <typename T>
constexpr inline usize length(const T *str) noexcept {
    return LIBCXX_NAMESPACE::wcslen(str);
}

template <typename T>
constexpr inline usize prefix_length(const T *str, const T *chars, bool exclude = false) noexcept {
    return exclude ? LIBCXX_NAMESPACE::wcscspn(str, chars) : LIBCXX_NAMESPACE::wcsspn(str, chars);
}

template <typename T>
constexpr inline int compare(const T *a, const T *b) noexcept {
    return LIBCXX_NAMESPACE::wcscmp(a, b);
}

template <typename T>
constexpr inline int compare_n(const T *a, const T *b, usize n) noexcept {
    return LIBCXX_NAMESPACE::wcsncmp(a, b, n);
}

template <typename T>
constexpr inline int compare_locale(const T *a, const T *b) noexcept {
    return LIBCXX_NAMESPACE::wcscoll(a, b);
}

template <typename T>
constexpr inline usize transform(T *dest, const T *src, usize n) noexcept {
    return LIBCXX_NAMESPACE::wcsxfrm(dest, src, n);
}

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

inline const char *error(int errnum) noexcept {
    return LIBCXX_NAMESPACE::strerror(errnum);  // No modern alternative for this
}

#ifdef _WIN32
#pragma warning(pop)
#endif
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M8C_STRING
