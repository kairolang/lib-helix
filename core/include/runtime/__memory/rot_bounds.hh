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

#ifndef _$_HX_CORE_M11ROT_BOUNDS
#define _$_HX_CORE_M11ROT_BOUNDS

#include <include/config/config.hh>

#include <include/c++/libc++.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
inline bool in_rotdata(const void *ptr) {
#if defined(__linux__)
    /// this is because on linux mincore, excepts a unsigned char pointer
    /// but on mac and other unix systems it expects a char pointer
    unsigned
#endif
#if defined(__linux__) || defined(__APPLE__)
    char mincore_vec;  // NOLINT
    if (mincore(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) &
                                         ~static_cast<uintptr_t>(4095)),
                4096,
                &mincore_vec) == 0) {
        return (mincore_vec & 0x4) == 0;
    }
    return false;
#elif defined(_WIN32)
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(ptr, &mbi, sizeof(mbi))) {
        return !(mbi.Protect & PAGE_READWRITE);
    }
    return false;
#endif
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M11ROT_BOUNDS
