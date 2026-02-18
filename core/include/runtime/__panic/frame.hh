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

#ifndef _$_HX_CORE_M5FRAME
#define _$_HX_CORE_M5FRAME

#include <include/config/config.hh>

#include <include/runtime/__memory/memory.hh>
#include <include/types/string/basic.hh>

#include "panic_fwd.hh"
#include "panic_interfaces.hh"
#include "frame_context.hh"

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Panic {
/// \class Frame
///
/// `Frame` is a core component of Kairo's panic-handling system, encapsulating the details
/// of an error state (panic). It is designed to store critical information about the panic,
/// such as the reason, file, and line number, and propagate the error through the system.
///
/// ### Design Details
/// - **Immediate Panic**: The `Frame` constructor is designed to immediately invoke a panic
///   on the provided object. This ensures that invalid states are caught and handled as early
///   as possible.
/// - **Static and Instance Support**: The `initialize` function determines whether the
///   provided type supports static or instance-level panic methods and invokes them
///   accordingly.
/// - **Type Constraints**: The `Frame` class enforces strong constraints on the provided type.
///   It ensures that the type is either copy or move constructible and implements the Kairo
///   panic interface (`Panicking` concept).
///
/// ### Responsibilities
/// - Store details about the panic, including:
///   - The reason for the panic.
///   - The file and line number where the panic occurred.
/// - Manage the lifetime of the panic object using `FrameContext`.
/// - Provide methods to retrieve panic details.
/// - Propagate the panic by invoking a panic handler and throwing the managed object.
///
/// ### Constructor Use
/// This class is typically constructed using the `panic` keyword in Kairo:
/// ```kairo
/// panic std::Panic::Frame(std::Error::RuntimeError(), "yes.kro", 12);
/// ```
/// The `Frame` class can also be extracted into a variable and then passed to the `panic`
/// system. These are the only valid ways to create and utilize `Frame` objects.
///
/// ### Panic Propagation
/// When a `Frame` object is panicked:
/// - The internal panic handler is invoked.
/// - The associated object is thrown as an exception.
///
/// ### Key Features
/// - **Static Enforcement**: Ensures only types satisfying the `Panicking` concept are accepted.
/// - **Dynamic Propagation**: Integrates tightly with `FrameContext` to manage and throw objects
///   as part of the panic process.
/// - **Contextual Information**: Provides file, line, and reason details for debugging purposes.
///
/// ### Example Usage
/// ```cpp
/// // Triggering a panic with a custom error
/// Frame frame(std::Error::RuntimeError("Some error occurred"), "example.kro", 42);
/// frame.operator$panic();
/// ```
///
/// ### Notes
/// - The default constructor is explicitly deleted to prevent creating invalid `Frame` instances.
/// - Copy operations are also deleted to maintain unique ownership of the panic context.
/// - Move operations are supported, allowing transfer of ownership.
///
/// ### Interactions with FrameContext
/// The `Frame` class heavily relies on `FrameContext` to manage the lifecycle of the panic object.
/// This ensures proper cleanup and exception safety.
///
/// ### Implementation Details
/// - **`initialize<T>`**:
///   - Determines whether the type supports static (`T::operator$panic`) or instance-level
///     (`obj.operator$panic`) panic methods.
///   - Constructs the panic object dynamically and wraps it in a `FrameContext`.
/// - **`operator$panic`**:
///   - Invokes the internal panic handler.
///   - Calls `FrameContext::crash()` to throw the managed object.
///
/// ### API Summary
/// - Constructors:
///   - `Frame(T obj, const char *filename, usize lineno)`
///   - `Frame(T obj, string filename, usize lineno)`
///   - Deleted: default and copy constructors.
///   - Defaulted: move constructors.
/// - Accessors:
///   - `file()`: Returns the file name associated with the panic.
///   - `line()`: Returns the line number associated with the panic.
///   - `reason()`: Returns the panic reason.
///   - `get_context()`: Returns a pointer to the underlying `FrameContext`.
/// - Methods:
///   - `operator$panic()`: Propagates the panic.
class Frame {
  private:
    mutable FrameContext context;
    string               _reason;
    string               _file;
    usize                _line = 0;

    template <typename T>
        requires std ::Interface ::ClassType<T>
    void initialize(const T *obj);

  public:
    bool show_trace = true;

    template <typename T>
    inline Frame(T obj, const char *filename, usize lineno);

    template <typename T>
    inline Frame(T obj, string filename, usize lineno);

    constexpr Frame() = delete;
    ~Frame()          = default;

    Frame(const Frame &)            = default;
    Frame &operator=(const Frame &) = default;

    Frame(Frame &&other) noexcept            = default;
    Frame &operator=(Frame &&other) noexcept = default;

    [[noreturn]] inline Frame(Frame &obj, const string &, usize);
    [[noreturn]] inline Frame(Frame &&obj, const string &, usize);

    [[nodiscard]] inline string        file() const;
    [[nodiscard]] inline usize         line() const;
    [[nodiscard]] inline string        reason() const;
    [[nodiscard]] inline FrameContext *get_context() const;

    [[noreturn]] inline void operator$panic() const;
};
}  // namespace Panic

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5FRAME
