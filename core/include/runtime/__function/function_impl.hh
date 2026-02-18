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

#ifndef _$_HX_CORE_M13FUNCTION_IMPL
#define _$_HX_CORE_M13FUNCTION_IMPL

#include <include/config/config.hh>

#include <include/meta/meta.hh>
#include <include/runtime/__memory/memory.hh>
#include <include/meta/remove_reference.hh>

H_NAMESPACE_BEGIN

/// \class $function
///
/// The `$function` class is a type-erased, dynamically allocated wrapper for callable entities.
/// It provides a uniform interface for invoking various callable objects, including function
/// pointers, lambdas, and functors, with the same signature.
template <typename Sig>
class $function;

/// \class $function
///
/// A core utility within the Kairo runtime, `$function` provides support for function-like
/// objects, enabling flexible and dynamic handling of callable entities. This feature is central to
/// implementing higher-order functions, callbacks, and functional programming paradigms in Kairo.
///
/// ### Overview
/// `$function` serves as a type-erased, dynamically allocated wrapper for various callable
/// entities, including:
/// - Lambdas.
/// - Function pointers.
/// - Functor objects (classes with `operator()`).
/// - Kairo-specific callable constructs, such as `fn` types.
///
/// This wrapper ensures consistent semantics and integration with Kairo's runtime, allowing
/// developers to store and invoke any callable with the same type signature.
///
/// ### Key Features
/// - **Dynamic Callable Wrapping**:
///   - Can wrap any callable conforming to the specified signature (`Rt(Tp...)`).
///   - Provides type erasure to handle various callable types uniformly.
/// - **Integration with Kairo's `fn` Syntax**:
///   - Supports Kairo-specific syntax for defining function types (`fn (...) -> ...`).
///   - Allows assigning function pointers, lambdas, or callable objects to `fn` variables:
///     ```kairo
///     let x: fn (i32) -> void = some_function;
///     let y: fn () -> void = fn () -> void { print("Hello"); };
///     class Functor {
///         fn op () ()[call_op] -> void;
///     }
///     let z: fn () -> void = Functor();
///     ```
/// - **Memory Management**:
///   - Manages the lifetime of the callable object.
///   - Provides explicit control via the `reset()` method to clear the current callable.
/// - **Copy and Move Semantics**:
///   - Supports both copy and move operations for flexible ownership management.
/// - **Boolean Conversion**:
///   - Can be evaluated in boolean contexts to check if it holds a callable.
///
/// ### Components
/// #### `$callable`
/// An abstract base struct that defines the interface for all wrapped callable types:
/// - `invoke`: Executes the callable with the provided arguments.
/// - `clone`: Creates a deep copy of the callable.
///
/// #### `Callable`
/// A templated implementation of `$callable` for a specific callable type. Manages the storage,
/// invocation, and copying of the wrapped callable.
///
/// ### Functionality
/// #### Construction
/// `$function` can be constructed from:
/// - A callable object (e.g., lambda or functor):
///   ```kairo
///   let f: fn (i32) -> void = [](i32 x) { print(x); };
///   ```
/// - A function pointer:
///   ```kairo
///   let f: fn () -> void = &some_function;
///   ```
///
/// #### Invocation
/// Once assigned, `$function` objects can be called like regular functions:
/// ```kairo
/// let f: fn (i32) -> void = [](i32 x) { print(x); };
/// f(42);  // Output: 42
/// ```
///
/// #### Resetting
/// The `reset()` method clears the current callable and releases any associated resources:
/// ```kairo
/// let f: fn () -> void = &some_function;
/// f.reset();
/// assert(f == null);
/// ```
///
/// ### Notes
/// - `$function` integrates tightly with Kairo's runtime and is part of its standard library.
/// - The implementation currently relies on `libc++` for alignment and utility functions.
/// - Future versions may refactor `$function` to remove external dependencies.
///
/// ### Example Usage
/// ```kairo
/// fn hello_world() -> void {
///     print("Hello, World!");
/// }
///
/// let f: fn () -> void = &hello_world;
/// f();  // Output: Hello, World!
///
/// let g: fn (i32) -> void = [](i32 x) { print(x); };
/// g(10);  // Output: 10
/// ```
///
/// ### Integration with Kairo
/// `$function` supports Kairo's `fn` type, enabling seamless integration with Kairo's language
/// constructs and runtime. This makes it a core building block for implementing higher-order
/// functions and functional-style programming in Kairo.
///
/// ### Related Concepts
/// - `fn` Types: The Kairo-specific function type syntax.
/// - Callable Objects: Classes with `operator()` can be used as functors with `$function`.
/// - Lambdas: Inline callable constructs supported by `$function`.
template <typename Rt, typename... Tp>
class $function<Rt(Tp...)> {
  private:
    /// \struct $callable
    ///
    /// The base class for all callable entities managed by `$function`. This abstract class
    /// provides the foundation for type erasure and runtime polymorphism, enabling `$function` to
    /// work with any callable conforming to the specified signature.
    ///
    /// ### Purpose
    /// `$callable` serves as the interface for:
    /// - Storing callable objects of arbitrary types.
    /// - Invoking those objects with a consistent interface.
    /// - Cloning callable objects to support copy semantics in `$function`.
    ///
    /// By defining the essential methods (`invoke` and `clone`), `$callable` allows `$function` to
    /// manage and invoke callable entities without knowing their exact types at compile time.
    ///
    /// ### Why It Exists
    /// `$callable` abstracts the behavior of callable objects, enabling `$function` to:
    /// - Store various callable types uniformly.
    /// - Avoid direct dependency on specific callable implementations.
    /// - Provide runtime flexibility with minimal overhead.
    ///
    /// ### Key Methods
    /// - **`invoke(Tp... args)`**:
    ///   Executes the callable with the provided arguments. Each derived class implements this
    ///   method to handle invocation for its specific callable type.
    /// - **`clone()`**:
    ///   Creates a deep copy of the callable object. This supports the copy constructor and
    ///   assignment operator in `$function`.
    ///
    /// ### Example
    /// The `$function` class uses `$callable` to store and manage callable entities:
    /// ```kairo
    /// $function<void(int)> f = [](int x) { print(x); };
    /// f(42);  // Internally calls `$callable::invoke`.
    /// ```
    struct $callable {
        constexpr $callable()                                 = default;
        constexpr $callable(const $callable &)                = default;
        constexpr $callable &operator=(const $callable &)     = default;
        constexpr $callable($callable &&) noexcept            = default;
        constexpr $callable &operator=($callable &&) noexcept = default;
        constexpr virtual ~$callable()                        = default;
        constexpr virtual Rt         invoke(Tp... args)       = 0;
        constexpr virtual $callable *clone() const            = 0;
    };

    template <typename T>
    struct Callable : $callable {
        alignas(alignof(libcxx::max_align_t)) T callable;

        constexpr Callable(typename std::Meta::reference_removed<T>
                               &&callable)  // NOLINT(google-explicit-constructor)
            : callable(std::Memory::forward<T>(callable)) {}

        constexpr Callable(const T &callable)  // NOLINT(google-explicit-constructor)
            : callable(callable) {}

        constexpr Rt invoke(Tp... args) override {
            return callable(std::Memory::forward<Tp>(args)...);
        }

        constexpr $callable *clone() const override {
            return std::Memory::new_aligned<Callable<T>>(callable);
        }
    };

    $callable *callable;

  public:
    constexpr $function()
        : callable(nullptr) {}

    constexpr $function($function &&other) noexcept
        : callable(other.callable) {
        other.callable = nullptr;
    }

    constexpr $function(const $function &other)
        : callable(other.callable ? other.callable->clone() : nullptr) {}

    template <typename T>
    constexpr $function(
        typename std::Meta::reference_removed<T> $call_o) {  // NOLINT(google-explicit-constructor)
        callable = new Callable<libcxx::decay_t<T>>(std::Memory::forward<T>($call_o));  // NOLINT
    }

    template <typename T>
    constexpr $function(T $call_o) {  // NOLINT(google-explicit-constructor)
        callable = new Callable<libcxx::decay_t<T>>(std::Memory::forward<T>($call_o));  // NOLINT
    }

    constexpr $function(Rt (*func)(Tp...))  // NOLINT(google-explicit-constructor)
        : callable(func ? new Callable<std::Meta::const_volatile_removed<Rt (*)(Tp...)>>(func)
                        : nullptr) {}

    constexpr ~$function() { reset(); }

    constexpr $function &operator=($function &&other) noexcept {
        if (this != &other) {
            reset();

            callable       = other.callable;
            other.callable = nullptr;
        }

        return *this;
    }

    constexpr $function &operator=(const $function &other) {
        if (this != &other) {
            reset();
            callable = other.callable ? other.callable->clone() : nullptr;
        }
        return *this;
    }

    template <typename T>
    constexpr $function &operator=(T $call_o) {
        delete callable;
        callable = new Callable<libcxx::decay_t<T>>(std::Memory::forward<T>($call_o));  // NOLINT
        return *this;
    }

    // Assignment for function pointers
    constexpr $function &operator=(Rt (*func)(Tp...)) {
        delete callable;
        callable = func ? new Callable<Rt (*)(typename std::Meta::reference_removed<Tp>...)>(func)
                        : nullptr;
        return *this;
    }

    explicit constexpr           operator bool() const noexcept { return callable != nullptr; }
    [[nodiscard]] constexpr bool operator$question() const noexcept { return callable != nullptr; }

    constexpr Rt operator()(Tp... args) {
        if (callable == nullptr) {
            throw "called an unset function pointer";
        }

        return callable->invoke(std::Memory::forward<Tp>(args)...);
    }

    constexpr void reset() noexcept {
        if (callable) {
            delete callable;
            callable = nullptr;
        }
    }
};

H_NAMESPACE_END

#endif  // _$_HX_CORE_M13FUNCTION_IMPL
