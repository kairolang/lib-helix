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

#ifndef _$_HX_CORE_M12ERASURE_BASE
#define _$_HX_CORE_M12ERASURE_BASE

#include <include/config/config.hh>

#include <include/c++/libc++.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

/// \class TypeErasure
///
/// `TypeErasure` is an abstract base class that provides a mechanism for type erasure.
/// This class serves as the foundation for managing objects of arbitrary types without
/// exposing their concrete implementation details. It is particularly useful for
/// generic systems requiring heterogeneous collections, dynamic polymorphism, or
/// runtime type inspection.
///
/// ### Design Details
/// - **Abstract Interface**: `TypeErasure` defines an interface with pure virtual
///   methods that subclasses must implement. These methods handle object destruction,
///   type retrieval, cloning, and access.
/// - **Dynamic Lifetime Management**: It ensures safe destruction and cleanup of
///   objects, allowing proper lifecycle management within a type-erased context.
/// - **Runtime Type Information**: The `type_info` method provides RTTI for the
///   stored object, enabling dynamic type checks.
///
/// ### Usage
/// `TypeErasure` is not meant to be used directly. Instead, it is subclassed to wrap
/// specific object types. For example, `TypeErasureImpl<T>` is a concrete implementation
/// that can store objects of type `T`.
///
/// ### Responsibilities
/// - Provide an interface for type-erased storage and manipulation.
/// - Abstract away the underlying type of stored objects while maintaining access
///   to their functionality.
///
/// ### Interactions with FrameContext
/// `TypeErasure` is a key component of the `FrameContext` class. It allows `FrameContext`
/// to store and manage objects of arbitrary types while ensuring proper cleanup and
/// exception handling.
///
/// ### Key Methods
/// - `destroy()`: Safely destroys the stored object.
/// - `type_info()`: Retrieves runtime type information of the stored object.
/// - `clone()`: Creates a deep copy of the type-erased object.
///
/// ### Example
/// \code{.cpp}
/// // creating a typeerasure instance
/// TypeErasure *erased = new TypeErasureImpl<MyType>(new MyType());
///
/// // accessing type information
/// const libcxx::type_info *info = erased->type_info();
///
/// // destroying the object
/// erased->destroy();
/// delete erased;
/// \endcode
class TypeErasure {
  public:
    constexpr virtual ~TypeErasure() = default;

    constexpr TypeErasure()                                   = default;
    constexpr TypeErasure(const TypeErasure &)                = default;
    constexpr TypeErasure &operator=(const TypeErasure &)     = default;
    constexpr TypeErasure(TypeErasure &&) noexcept            = default;
    constexpr TypeErasure &operator=(TypeErasure &&) noexcept = default;

    constexpr virtual void                                   destroy()         = 0;
    constexpr virtual void                                  *operator*()       = 0;
    [[nodiscard]] constexpr virtual const libcxx::type_info *type_info() const = 0;
    [[nodiscard]] constexpr virtual TypeErasure             *clone() const     = 0;
};

/// \class TypeErasureImpl
///
/// \warning This class does not free the memory of the object it holds, it is the
///          responsibility of the caller to free the memory.
///
/// `TypeErasureImpl` is a templated implementation of the `TypeErasure` interface. It
/// provides type-erased storage and management for objects of type `T`. This class
/// encapsulates the object, allowing it to be manipulated without exposing its type.
///
/// ### Design Details
/// - **Type-Specific Storage**: `TypeErasureImpl` wraps an object of type `T`,
///   providing type-erased functionality.
/// - **Dynamic Behavior**: Implements methods for destruction, cloning, and access,
///   ensuring the stored object can be safely managed within a type-erased context.
/// - **Integration with FrameContext**: This class is used by `FrameContext` to
///   manage objects dynamically, supporting panic handling and exception throwing.
///
/// ### Responsibilities
/// - Manage the lifetime of an object of type `T`.
/// - Provide a mechanism to retrieve runtime type information for `T`.
/// - Enable cloning of the type-erased wrapper.
///
/// ### Implementation Notes
/// - The `destroy()` method ensures the wrapped object is properly deleted.
/// - The `clone()` method creates a deep copy of the wrapped object, maintaining
///   type safety within the type-erased system.
///
/// ### Interactions
/// `TypeErasureImpl` is used internally by `FrameContext` and is not typically
/// exposed to end users. It enables `FrameContext` to work with heterogeneous types
/// while maintaining strong exception safety guarantees.
///
/// \warning This class is NOT memory safe.
///
/// ### Example
/// \code{.cpp}
/// // wrapping an object of type mytype
/// MyType *myObj = new MyType();
/// TypeErasureImpl<MyType> erased(myObj);
///
/// // accessing runtime type information
/// const libcxx::type_info *info = erased.type_info();
///
/// // safely destroying the object
/// erased.destroy();
/// \endcode
template <typename T>
class TypeErasureImpl : public TypeErasure {
  private:
    T                       *object;
    const libcxx::type_info *type;
    bool                     is_destroyed = false;  // Guard against double deletion

  public:
    // Constructors
    TypeErasureImpl(const TypeErasureImpl &other);
    explicit TypeErasureImpl(T *obj);

    // Assignment Operators
    TypeErasureImpl &operator=(const TypeErasureImpl &other);
    TypeErasureImpl &operator=(TypeErasureImpl &&other) noexcept;

    // Move Constructor
    TypeErasureImpl(TypeErasureImpl &&other) noexcept;

    // Destructor
    ~TypeErasureImpl() override;

    // Override Methods
    void                                   destroy() override;
    void                                  *operator*() override;
    [[nodiscard]] const libcxx::type_info *type_info() const override;
    [[nodiscard]] TypeErasure             *clone() const override;
};

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M12ERASURE_BASE
