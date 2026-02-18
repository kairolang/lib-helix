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

#ifndef _$_HX_CORE_M12ERASURE_IMPL
#define _$_HX_CORE_M12ERASURE_IMPL

#include <include/config/config.hh>

#include <include/runtime/__error/error.hh>
#include <include/runtime/__memory/memory.hh>
#include <include/types/type_erasure/erasure_base.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename T>
inline TypeErasureImpl<T>::TypeErasureImpl(const TypeErasureImpl &other)
    : object(new T(*other.object))
    , type(other.type)
    , is_destroyed(other.is_destroyed) {}

template <typename T>
inline TypeErasureImpl<T> &TypeErasureImpl<T>::operator=(const TypeErasureImpl &other) {
    if (this != &other) {
        if (object != nullptr) {  // NOLINT(readability-delete-null-pointer)
            delete object;
        }
        object       = new T(*other.object);  // NOLINT(cppcoreguidelines-owning-memory)
        is_destroyed = other.is_destroyed;
        type         = other.type;
    }
    return *this;
}

template <typename T>
inline TypeErasureImpl<T>::TypeErasureImpl(TypeErasureImpl &&other) noexcept
    : object(other.object)
    , type(other.type)
    , is_destroyed(other.is_destroyed) {
    other.object       = nullptr;
    other.is_destroyed = true;
}

template <typename T>
inline TypeErasureImpl<T> &TypeErasureImpl<T>::operator=(TypeErasureImpl &&other) noexcept {
    if (this != &other) {
        if (object != nullptr) {  // NOLINT(readability-delete-null-pointer)
            delete object;
        }
        object             = other.object;
        is_destroyed       = other.is_destroyed;
        other.object       = nullptr;
        other.is_destroyed = true;
        type               = other.type;
    }
    return *this;
}

template <typename T>
inline TypeErasureImpl<T>::TypeErasureImpl(T *obj)
    : object(obj)
    , type(&typeid(T)) {}

template <typename T>
inline void TypeErasureImpl<T>::destroy() {
    if (!is_destroyed && object != nullptr) {
        delete object;
        object       = nullptr;
        is_destroyed = true;
    }
}

template <typename T>
inline void *TypeErasureImpl<T>::operator*() {
    return object;
}

template <typename T>
inline const libcxx::type_info *TypeErasureImpl<T>::type_info() const {
    return type;
}

template <typename T>
inline TypeErasure *TypeErasureImpl<T>::clone() const {
    if (object == nullptr) {
        throw std::Error::RuntimeError(L"Cannot clone a null object.");
    }
    return new TypeErasureImpl<T>(new T(*object));  // NOLINT(cppcoreguidelines-owning-memory)
}

template <typename T>
inline TypeErasureImpl<T>::~TypeErasureImpl() {
    object = nullptr;  // Note: destroy() should be called explicitly to delete object
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M12ERASURE_IMPL
