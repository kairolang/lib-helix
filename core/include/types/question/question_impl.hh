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

#ifndef _$_HX_CORE_M13QUESTION_IMPL
#define _$_HX_CORE_M13QUESTION_IMPL

#include <include/config/config.hh>

#include <include/runtime/__panic/frame.hh>  // THIS is the issue
#include <include/types/builtins/primitives.hh>

H_NAMESPACE_BEGIN

/// \class $question
///
/// Represents Kairo's `?` type, also known as the **quantum type**. This type can hold either:
/// - A valid value of type `T`,
/// - A null state, or
/// - An error represented by a `std::Panic::Frame`.
///
/// ### Unique Concept
/// The `$question` type is unique to Kairo and is a core component of Kairo's type system, coined
/// as the **quantum type** because its state  (value, null, or error) remains unknown until
/// explicitly checked or "collapsed." This makes it more versatile than traditional nullable or
/// optional types. It enables developers to work with quantum-like values (`?`) that
/// simplify null checks, error propagation, and value retrieval while ensuring type safety.
///
/// ### States
/// `$question` operates in one of three states:
/// - **Value**: Holds a valid object of type `T`.
/// - **Null**: Represents the absence of a value.
/// - **Error**: Encapsulates an error using a `std::Panic::Frame`.
///
/// ### Key Features
/// - **Type Safety**: Enforces constraints such as copy and move constructibility for valid types.
/// - **Error Containment**: Encodes errors using `std::Panic::Frame`, enabling
/// structured error
///                          handling.
/// - **State Inspection**: Provides methods to determine whether the instance is null, contains a
///                         value, or holds an error of a specific type.
/// - **Casting**: Supports safe casting of stored values and errors with explicit checks to prevent
///                type mismatches.
/// - **Integration**: Designed to work seamlessly with Kairo's type system and error-handling
///                    mechanisms.
///
/// ### Use Cases
/// `$question` is particularly useful in scenarios where:
/// - A value may or may not be present.
/// - Functions can fail, and errors must be propagated or inspected.
/// - Null states need to be represented explicitly without relying on sentinel values.
///
/// ### Construction
/// `$question` supports the following constructions:
/// - **Default (Null)**: `T? x;` or `T? x = null;` creates a null instance.
/// - **Value Initialization**: `T? x = value;` stores a valid value.
/// - **Error Initialization**: `T? x = std::Panic::Frame(...);` stores an error state.
///
/// ### Core Behaviors
/// - **State Queries**:
///   - `bool operator$question() const`: Returns `true` if the state is `Value`.
///   - `bool is_null() const`: Checks if the state is `Null`.
///   - `bool is_err() const`: Checks if the state is `Error`.
///   - `bool is_err(const type_info *type) const`: Checks if the error matches a specific type.
/// - **Value Access**:
///   - `T operator*()`: Retrieves the value, or throws if null or in an error state.
///   - `T operator$cast(T * /*unused*/) const`: Casts the value to `T`, or throws if null or in an
///   error state.
/// - **Error Inspection**:
///   - `bool operator$contains(const E & /* unused */) const`: Returns `true` if the error matches
///   the type `E`.
/// - **Equality**:
///   - `bool operator==(const std::null_t & /*unused*/) const`: Checks for null equality.
///   - `bool operator==(const E & /*unused*/) const`: Checks if the error matches a specific type.
///
/// ### Guarantees
/// 1. Default initialization creates a null state.
/// 2. Errors are always stored as `std::Panic::Frame` objects.
/// 3. Errors and values are managed with proper resource allocation and destruction to avoid leaks
///    or undefined behavior.
///
/// ### Related
/// - `std::Panic::Frame`: Represents the error state.
/// - `std::Panic::Interface::Panicking`: Concept for types supporting the `operator$panic()`
/// function.
/// - `std::Panic::Interface::PanickingInstance`: Concept for instances supporting
/// `operator$panic()`.
/// - `std::Panic::Interface::PanickingStatic`: Concept for types supporting static
/// `operator$panic()`.
///
/// ### Example Usage
/// ```kairo
/// fn process_data() -> int? {
///     panic std::Error::RuntimeError("Data processing failed.");
/// }
///
/// fn main() -> i32 {
///     let x: int? = 42;                   // Holds a valid value.
///     let y: int? = null;                 // Null state.
///
///     let z: int? = process_data();       // Holds an error.
///
///     if x? {
///         print(x as i32 /* or '*x' */);  // Collapse the value. if the value is invalid the
///                                         // error gets thrown and can be caught using 'try-catch'
///     } else if x $contains RuntimeError {
///         print("Error occurred.");       // Handle specific error types.
///     }
///
///     return 0;
/// }
/// ```
///
/// ### Notes
/// `$question` is designed for developers to handle nullable and error-prone states seamlessly. It
/// ensures type safety and enforces proper construction and destruction of resources.
template <class T>
class $question {
  private:
    enum class $State : char { Value, Null, Error };

    union $StorageT {
        mutable std::Panic::Frame error;
        mutable T                 value;

        constexpr $StorageT() noexcept
            : value() {}
        ~$StorageT() noexcept {}
    };

    mutable $State state = $State::Null;
    $StorageT      data;

    [[nodiscard]] bool is_null() const noexcept;
    [[nodiscard]] bool is_err() const noexcept;
    [[nodiscard]] bool is_err(const libcxx::type_info *type) const noexcept;

    void set_value(const T &value);
    void set_value(T &&value);
    void set_err(std::Panic::Frame &&error);
    void set_err(std::Panic::Frame &error);

    void delete_error() noexcept;
    void delete_value() noexcept;

  public:
    /// ------------------------------- Constructors (Null) -------------------------------
    $question() noexcept;
    $question(const std::null_t &) noexcept;
    $question(std::null_t &&) noexcept;

    /// ------------------------------- Constructors (Value) -------------------------------
    $question(const T &value);
    $question(T &&value);

    /// ------------------------------- Constructors (Error) -------------------------------
    $question(const std::Panic::Frame &error);
    $question(std::Panic::Frame &&error);

    /// --------------------------- Move Constructor & Assignment ---------------------------
    $question($question &&other) noexcept;

    $question &operator=($question &&other) noexcept;

    /// -------------------------- Copy Constructor & Assignment ----------------------------
    $question(const $question &other);
    $question &operator=(const $question &other);

    /// ------------------------------- Destructor -------------------------------
    ~$question();

    /// ------------------------------- Operators -------------------------------
    bool operator==(const std::null_t &) const noexcept;
    bool operator!=(const std::null_t &) const noexcept;

    template <typename E>
    bool operator==(const E &) const noexcept;

    template <typename E>
    bool operator!=(const E &other) const noexcept;

    template <typename E>
    bool operator$contains(const E &other) const noexcept;

    [[nodiscard]] bool operator$question() const noexcept;

    /// ------------------------------- Casting -------------------------------
    template <typename E>
        requires std::Panic::Interface::Panicking<E>
    E operator$cast(E * /*unused*/) const;
    T operator$cast(T * /*unused*/) const;

    [[nodiscard]] T &operator*();
    [[nodiscard]] operator T();
};

H_NAMESPACE_END

#endif  // _$_HX_CORE_M13QUESTION_IMPL
