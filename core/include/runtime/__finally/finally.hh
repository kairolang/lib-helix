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

#ifndef _$_HX_CORE_M7FINALLY
#define _$_HX_CORE_M7FINALLY

#include <include/config/config.hh>

#include <include/runtime/__function/function.hh>
#include <include/runtime/__memory/memory.hh>

H_NAMESPACE_BEGIN

/// \class $finally
///
/// Provides "finally" semantics in Kairo, allowing guaranteed execution of cleanup or other
/// finalization code at the end of a scope, regardless of whether an exception or error occurred.
///
/// ### Purpose
/// `$finally` ensures that specific code is executed when the scope in which it resides is exited.
/// It is particularly useful for resource management, cleanup tasks, and ensuring consistent state,
/// especially in the presence of exceptions.
///
/// ### Features
/// - Executes a specified callable at the end of the scope.
/// - Can be used independently of `try-catch` blocks or in conjunction with them.
/// - Acts like `defer` in other langs, but with Kairo-specific integration into `finally` blocks.
///
/// ### Usage
/// `$finally` can be used with:
/// 1. Standalone `finally` blocks:
///    ```kairo
///    finally {
///        print("This always runs when the scope ends.");
///    }
///    ```
///
/// 2. `try-catch-finally` constructs:
///    ```kairo
///    try {
///        some_erroring_function();
///    } catch {
///        print("Handling the error.");
///    } finally {
///        print("Cleaning up resources.");
///    }
///    ```
///
/// ### Implementation
/// - `$finally` wraps a callable provided at construction and executes it in its destructor.
/// - It leverages the `$function<void()>` type for callable storage, enabling type erasure and
///   support for various callable types such as lambdas, function pointers, and functors.
///
/// ### Example
/// ```kairo
/// void example() {
///     finally {
///         print("This runs when 'example' exits.");
///     }
///
///     print("Doing something...");
/// }
/// ```
///
/// ### Notes
/// - This construct is unique to Kairo and designed for maximum flexibility and simplicity.
/// - When combined with `try-catch`, it ensures code in the `finally` block executes regardless
///   of whether an error occurred in the `try` block. Or can be used standalone for scope-based
///   cleanup.
///
/// ### Design
/// - `$finally` objects are non-copyable and non-movable to ensure that their associated callable
///   always runs at the intended scope exit.
/// - The destructor executes the stored callable if one was provided at construction.
///
/// ### Limitations
/// - `$finally` depends on `$function` for its callable implementation. Future work may involve
///   reducing this dependency for tighter integration with the Kairo runtime.
class $finally {
  public:
    $finally()                            = default;
    $finally(const $finally &)            = delete;
    $finally($finally &&)                 = delete;
    $finally &operator=(const $finally &) = delete;
    $finally &operator=($finally &&)      = delete;
    ~$finally() {
        if (m_fn) {
            m_fn();
        }
    }

    template <typename Fn>
    explicit $finally(Fn &&fn)
        : m_fn{std::Memory::forward<Fn>(fn)} {}

  private:
    $function<void()> m_fn;
};

H_NAMESPACE_END

#endif  // _$_HX_CORE_M7FINALLY
