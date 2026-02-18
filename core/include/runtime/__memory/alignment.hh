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

#ifndef _$_HX_CORE_M9ALIGNMENT
#define _$_HX_CORE_M9ALIGNMENT

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/types/builtins/builtins.hh>

#include "forwarding.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Memory {
///
/// \brief Dynamically allocates memory for a type T, respecting T's alignment,
///        and then constructs (placement-new) the object.
///
/// \tparam T   The object type to allocate and construct.
/// \tparam Args Constructor argument types.
/// \param  args Constructor arguments forwarded to T's constructor.
/// \return      Pointer to the newly constructed T.
/// \throws      std::bad_alloc if memory allocation fails.
///
template <typename T, typename... Args>
inline T *new_aligned(Args &&...args) {
    static_assert(alignof(T) <= __STDCPP_DEFAULT_NEW_ALIGNMENT__ || __cpp_aligned_new,
                  "Your compiler does not support over-aligned allocations.");

    void *raw_mem = nullptr;
#ifdef _MSC_VER
    raw_mem = _aligned_malloc(sizeof(T), alignof(T));

    if (!raw_mem) {
        throw libcxx::bad_alloc();
    }
#else
    raw_mem = ::operator new(sizeof(T), libcxx::align_val_t(alignof(T)));
#endif

    return ::new (raw_mem) T(libcxx::forward<Args>(args)...);
}

///
/// \brief Destroys and deallocates an object previously created by new_aligned.
///
/// \tparam T   The object type to destroy.
/// \param ptr  Pointer to the object to destroy and deallocate.
///
/// Calling delete_aligned on a pointer not allocated by new_aligned<T> is undefined.
///
template <typename T>
inline void delete_aligned(T *ptr) {
    if (!ptr) {
        return;
    }

    ptr->~T();
#ifdef _MSC_VER
    _aligned_free(ptr);
#else
    ::operator delete(ptr, libcxx::align_val_t(alignof(T)));
#endif
}
}  // namespace Memory

H_NAMESPACE_END
H_STD_NAMESPACE_END

#endif  // _$_HX_CORE_M9ALIGNMENT
