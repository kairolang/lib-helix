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

#ifndef _$_HX_CORE_M13STACK_BOUNDS
#define _$_HX_CORE_M13STACK_BOUNDS

#include <include/config/config.hh>

#include <include/types/builtins/builtins.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
inline usize stack_size(usize *size = nullptr) {
    static size_t stack_size = 0;
    static bool  called     = false;

    if (called) {
        return (size != nullptr) ? (*size = usize(stack_size)) : usize(stack_size);
    }

#if defined(__linux__) || defined(__APPLE__)
#if defined(__APPLE__)
    stack_size = pthread_get_stacksize_np(pthread_self());
#elif defined(__linux__)
    pthread_attr_t attr;

    pthread_getattr_np(pthread_self(), &attr);
    pthread_attr_getstacksize(&attr, &stack_size);
    pthread_attr_destroy(&attr);
#endif
#elif defined(_WIN32)
    NT_TIB *tib = reinterpret_cast<NT_TIB *>(NtCurrentTeb());
    stack_size  = usize(reinterpret_cast<uintptr_t>(tib->StackBase) -
                       reinterpret_cast<uintptr_t>(tib->StackLimit));
#endif
    called = true;
    return (size != nullptr) ? (*size = usize(stack_size)) : usize(stack_size);
}

inline void *stack_start(void **start = nullptr) {
    static void *st_addr = nullptr;
    static bool  called  = false;

    if (called) {
        return (start != nullptr) ? (*start = st_addr) : (st_addr);
    }

#if defined(__linux__) || defined(__APPLE__)
    static libcxx::size_t st_size = 0;
    pthread_attr_t        attr;

#if defined(__linux__)
    pthread_getattr_np(pthread_self(), &attr);
#endif

    pthread_attr_getstack(&attr, &st_addr, &st_size);
    pthread_attr_destroy(&attr);
#elif defined(_WIN32)
    NT_TIB *tib = reinterpret_cast<NT_TIB *>(NtCurrentTeb());
    st_addr     = tib->StackLimit;
#endif
    called = true;
    return (start != nullptr) ? (*start = st_addr) : (st_addr);
}

inline void *stack_end(void **end = nullptr) {
    static void *st_end = nullptr;
    static bool  called = true;

    if (called) {
        return (end != nullptr) ? (*end = st_end) : (st_end);
    }

#if defined(_WIN32)
    NT_TIB *tib = reinterpret_cast<NT_TIB *>(NtCurrentTeb());
    st_end      = tib->StackBase;
#elif defined(__linux__) || defined(__APPLE__)
    st_end = static_cast<void *>(static_cast<char *>(stack_start()) +
                                 static_cast<unsigned long long>(stack_size()));  // NOLINT
#endif
    called = true;
    return (end != nullptr) ? (*end = st_end) : (st_end);
}

inline libcxx::pair<void *, void *> stack_bounds(void **start = nullptr, void **end = nullptr) {
    libcxx::pair<void *, void *> pair;

    static void *s_st = stack_start();
    static void *s_ed = stack_end();

    pair.first  = (start != nullptr) ? (*start = s_st) : (s_st);
    pair.second = (end != nullptr) ? (*end = s_ed) : s_ed;

    return pair;
}

inline bool in_stack(const void *ptr) {
    static auto [stack_start, stack_end] = stack_bounds();
    return (ptr >= stack_start && ptr < stack_end);
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M13STACK_BOUNDS
