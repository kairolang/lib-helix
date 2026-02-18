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

#ifndef _$_HX_CORE_M13ADDRESS_TYPE
#define _$_HX_CORE_M13ADDRESS_TYPE

#include <include/config/config.hh>

#include "heap_bounds.hh"
#include "stack_bounds.hh"
#include "rot_bounds.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
enum class AddressType : u8 {
    ROTData,  // (.rodata)
    Stack,    // (stack)
    Heap,     // (heap)
    Unknown   // (in cases like an other program or some other shared memory configuration)
};

inline AddressType address_type(const void *ptr) {
    static auto [stack_st, stack_ed] = stack_bounds();
    static auto heap_base            = reinterpret_cast<uintptr_t>(heap_start());
    static auto stack_low            = reinterpret_cast<uintptr_t>(stack_st);
    static auto stack_high           = reinterpret_cast<uintptr_t>(stack_ed);
    auto        addr                 = reinterpret_cast<uintptr_t>(ptr);

    if (addr >= heap_base) {
        return AddressType::Heap;
    }

    if ((addr - stack_low) < (stack_high - stack_low)) {
        return AddressType::Stack;
    }

    if (in_rotdata(ptr)) {
        return AddressType::ROTData;
    }

    return AddressType::Unknown;
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M13ADDRESS_TYPE
