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

#ifndef _$_HX_CORE_M12HEAP_BOUNDS
#define _$_HX_CORE_M12HEAP_BOUNDS

#include <include/config/config.hh>

#include <include/c++/libc++.hh>

#ifdef _MSC_VER
#include <psapi.h>
#endif

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
inline void *heap_start(void **start = nullptr) {
    static void *hp_start = nullptr;
    static bool  called   = false;
    if (called) {
        return (start != nullptr) ? (*start = hp_start) : (hp_start);
    }
#if defined(__linux__) || defined(__APPLE__)
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif
    hp_start = sbrk(0);
#if defined(__clang__) || defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
#elif defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc, sizeof(pmc));
    hp_start = (void*)pmc.PrivateUsage;
#endif
    called = true;
    return (start != nullptr) ? (*start = hp_start) : (hp_start);
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M12HEAP_BOUNDS
