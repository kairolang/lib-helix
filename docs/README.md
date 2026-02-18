# Kairo Core Library Documentation Index

This documentation provides comprehensive coverage of all components in the `lib-helix/core` directory. The documentation follows ISO standards and provides practical information for developers working with the Kairo runtime.

## Documentation Structure

### [Core Library Overview](core.md)
Complete overview of the entire core library, including all modules, functions, and integration points. This is the primary reference document.

### Module-Specific Documentation

#### [Symbol Mangling](symbol-mangling.md)
- **File:** `abi.kro` | **Module:** `std::abi`
- Functions for converting symbol names between human-readable and binary-compatible formats
- Essential for linking, debugging, and symbol resolution

#### [Panic Handling System](panic-handling.md)
- **File:** `handler.kro`
- Global panic handler that manages unrecoverable runtime errors
- Provides detailed diagnostic information and formatted error output

#### [Panic Frame Management](frame-management.md)
- **File:** `frame.kro`
- Core functionality for creating and managing panic frames
- Handles object initialization, context preservation, and data access

#### [Frame Context Management](frame-context.md)
- **File:** `frame_context.kro`
- Object introspection and crash handling for panic frames
- Platform-specific type name demangling and error object management

#### [Range Operations](range-operations.md)
- **File:** `range.kro` | **Module:** `std`
- Comprehensive range generation and iteration support
- Integration with range operators (`..`, `..=`) and for-loop constructs

### C++ FFI Components
- **Directory:** `include/source/`
- Low-level C++ implementations bridging Kairo with C++ runtime
- Template implementations for casting, memory management, and data types

## Quick Reference

### Core Entry Point
- **File:** `core.kro`
- Runtime initialization and global symbol definitions
- Required for all Kairo programs

### Key Functions by Category

#### Error Handling
- `HX_FN_Vi_Q5_32_kairo_default_panic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv()` - Main panic handler
- `std::Panic::Frame::initialize()` - Frame initialization
- `std::Panic::FrameContext::crash()` - Crash execution

#### Symbol Management  
- `std::abi::mangle()` - Symbol name mangling
- `std::abi::demangle()` - Symbol name demangling
- `std::abi::is_mangled()` - Mangled symbol detection

#### Range Generation
- `range(last)` - Simple range creation
- `range(first, last, step)` - Full range specification
- `std::Range<T>` - Range class with iteration support

#### Data Access
- `std::Panic::Frame::file()` - Error file location
- `std::Panic::Frame::line()` - Error line number
- `std::Panic::Frame::reason()` - Error message

### Required Interfaces

#### RangeCompliant
For types to work with range functions:
- `fn op ++ (self) -> self` - Increment
- `fn op < (self, other: self) -> bool` - Comparison

#### Panic::Panicking
For types to work with panic system:
- `fn op panic (self) -> string` - Instance panic
- OR `static fn op panic () -> string` - Static panic

## Usage Guidelines

### Error Handling
1. Use the panic system for unrecoverable errors
2. Implement `Panic::Panicking` interface for custom error types
3. Panic frames provide detailed debugging information

### Range Operations
1. Prefer range operators (`..`, `..=`) over explicit function calls
2. Implement `RangeCompliant` for custom types
3. Ranges integrate with for-loops and comprehensions

### Symbol Management
1. Use ABI utilities when interfacing with external libraries
2. Demangling improves debug output readability
3. Symbol validation helps detect linking issues

### C++ Integration
1. FFI components handle low-level interoperability
2. Template implementations provide zero-cost abstractions
3. Platform-specific optimizations are handled automatically

## Standards Compliance

This documentation follows:
- **ISO/IEC 26514:2008** - Systems and software engineering documentation
- **Technical writing standards** for clarity and accuracy
- **Kairo Project conventions** for consistency

---

**Note:** All core library components are automatically available without explicit import statements. The core library is essential for Kairo runtime operation and should not be modified without thorough understanding of the compilation process.
