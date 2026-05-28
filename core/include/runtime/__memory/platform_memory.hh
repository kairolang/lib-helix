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

#ifndef _$_HX_CORE_M16PLATFORM_MEMORY
#define _$_HX_CORE_M16PLATFORM_MEMORY

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/types/builtins/builtins.hh>

H_NAMESPACE_BEGIN

template <typename T>
class $question;

H_STD_NAMESPACE_BEGIN

template <typename T>
using Questionable = $question<T>;

namespace Memory {
inline void *copy(void *dest, const void *src, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memcpy(dest, src, n);
}

inline void *move(void *dest, const void *src, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memmove(dest, src, n);
}

inline void *set(void *dest, int value, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memset(dest, value, n);
}

template <typename T>
inline std::Questionable<T *> find(const T *src, int value, size_t n) noexcept {
    void *result = LIBCXX_NAMESPACE::memchr(src, value, n);
    return result ? static_cast<T *>(result) : null;
}

inline int compare(const void *a, const void *b, size_t n) noexcept {
    return LIBCXX_NAMESPACE::memcmp(a, b, n);
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M16PLATFORM_MEMORY
