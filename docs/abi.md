# Kairo ABI Specification

This document defines the Application Binary Interface (ABI) for the Kairo programming language. The Kairo ABI is largely based on the Itanium C++ ABI but differs in naming conventions to align with Kairo's language design principles. Changes to naming conventions are reflected throughout this document where relevant.

---

## Overview

The Kairo ABI provides a standardized way for Kairo programs to interact at the binary level, ensuring compatibility across different implementations of the Kairo compiler and runtime. It specifies conventions for function calls, data layout, type encoding, and symbol mangling.

---

## Function Calling Convention

The Kairo ABI follows the same function calling convention as the Itanium ABI:

- The first six arguments are passed in registers, with specific registers used depending on the architecture (e.g., `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9` on x86-64).
- Additional arguments are passed on the stack.
- The return value is passed in the `rax` register for integral and pointer types, or in the floating-point registers (e.g., `xmm0`) for floating-point types.

---

## Data Types

The data types in the Kairo ABI match those in the Itanium ABI but use Kairo-specific naming conventions.

| **Kairo Type**  | **Description**                             |
|-----------------|---------------------------------------------|
| `i8`            | 8-bit signed integer (also used for `char`) |
| `i16`           | 16-bit signed integer                       |
| `i32`           | 32-bit signed integer                       |
| `i64`           | 64-bit signed integer                       |
| `i128`          | 128-bit signed integer (simulated)          |
| `i256`          | 256-bit signed integer (simulated)          |
| `u8`            | 8-bit unsigned integer                      |
| `u16`           | 16-bit unsigned integer                     |
| `u32`           | 32-bit unsigned integer                     |
| `u64`           | 64-bit unsigned integer                     |
| `u128`          | 128-bit unsigned integer (simulated)        |
| `u256`          | 256-bit unsigned integer (simulated)        |
| `f32`           | 32-bit floating-point number                |
| `f64`           | 64-bit floating-point number                |
| `f80`           | 80-bit floating-point number                |
| `f128`          | 128-bit floating-point number               |
| `bool`          | 1-bit boolean value (7 bits are padding)    |
| `void`          | No return value                             |
| `isize`         | Signed integer, pointer size                |
| `usize`         | Unsigned integer, pointer size              |

---

# Kairo Name Mangling Scheme Specification

The Kairo Name Mangling Scheme provides a systematic and precise way to encode entity names into unique strings for use in linkage, runtime type identification, and compatibility. This system is based on the **Itanium ABI**, modified to align with Kairo's syntax, semantics, and features as defined in the language and type system.

---

## General Structure of Mangled Names

The mangling for any Kairo entity follows this general pattern:

```
_HX_<EntityType><EncodedName><EncodedParameters><AdditionalMetadata>
```

### Components
1. **_HX_**:
   - Prefix for all mangled names, indicating they adhere to the Kairo ABI.

2. **EntityType**:
   - Indicates the type of entity (e.g., function, type, vtable, etc.).

3. **Length**:
   - The total length of the encoded name (entity name).

4. **EncodedName**:
   - Encodes namespaces, classes, functions, requires, etc., with length-prefixed components.

5. **EncodedParameters**:
   - Encodes function or requires parameters in order of appearance.

6. **AdditionalMetadata**:
   - Contains qualifiers like `const`, `unsafe`, or overloading information.

---

## Entity Types

The `EntityType` defines the category of the mangled name:

| **EntityType** | **Description**                        |
|----------------|----------------------------------------|
| `_FN`          | Function                               |
| `_RM`          | Requires instantiation                 |
| `_VT`          | Virtual table                          |
| `_TI`          | Type information (RTTI)                |
| `_NV`          | Non-virtual member (e.g., static data) |
| `_CT`          | Constructor                            |
| `_DT`          | Destructor                             |
| `_OP`          | Operator                               |
| `_LM`          | Lambda                                 |

## Qualifier Types

| **Qualifier** | **Description**           |
|---------------|---------------------------|
| `_C`          | `const` qualifier         |
| `_U`          | `unsafe` qualifier        |
| `_Q`          | `?` (optional) qualifier  |
| `_R`          | Return type qualifier     |
 
## Kairo Type Encoding

The following table defines type encodings for the Kairo type system.

| **Type**                | **Encoding**           |
|-------------------------|------------------------|
| `void`                  | `_v`                   |
| `bool`                  | `_b`                   |
| `usize`                 | `_us`                  |
| `isize`                 | `_is`                  |
| `f32`, `f64`, `f80`     | `_F32`, `_F64`, `_F80` |
| `u8`, `u16`, `u32`      | `_U8`,  `_U16`, `_U32` |
| `i8`, `i16`, `i32`      | `_I8`,  `_I16`, `_I32` |
| `[T; N]` (array)        | `_A<T><N>`             |
| `[T]`    (vector)       | `_V<T>`                |
| `{K: V}` (map)          | `_M<K><V>`             |
| `{T}`    (set)          | `_S<T>`                |
| `fn (...) -> ...`       | `_FT<Params>_R<Ret>`   |
| `unsafe T`              | `_U<T>`                |
| `const T`               | `_C<T>`                |
| `*T` (pointer)          | `_PK<T>`               |
| `T?` (optional)         | `_QT<T>`               |

---

## Name Encoding

### Module and Class Encoding
Modules, modules, and classes are encoded as:
```
<Length><Name>[<Length><Name>]...
```
- Each component of the qualified name is length-prefixed.
- Components are concatenated in order.

#### Examples
1. Global function:
   ```kairo
   fn runtime_initialize() -> void;
   ```
   Mangled: `_HX_FN18runtime_initializeRv`

2. Module nested function:
   ```kairo
   module kairo {
       fn runtime_initialize() -> void;
   }
   ```
   Mangled: `_HX_FN29kairo_runtime_initialize`

3. Nested classes:
   ```kairo
   module kairo {
       class Outer {
           class Inner {
               fn method() -> void;
           }
       }
   }
   ```
   Mangled: `_HX_FN33kairo_Outer_Inner_method`

---

### Requires Encoding

Templates are encoded as:
```
_HX_RM<Length><Name><RequiresParameters>
```
- `RequiresParameters`: Encodes parameter types recursively, types are expanded in order.

#### Examples
1. Single parameter:
   ```kairo
   struct Vector requires <T>;
   ```
   Mangled: `_HX_RM6Vector_T`

2. Instantiation:
   ```kairo
   Vector<i32>;
   ```
   Mangled: `_HX_RM11Vector_i32`

3. Nested requires:
   ```kairo
   Vector<Vector<i32>>;
   ```
   Mangled: `_HX_RM18Vector_RM11Vector_i32`

4. Module nested requires:
   ```kairo
   module kairo {
       struct Pair requires <T, U>;
   }
   Pair<i32, f32>;
   ```
   Mangled: `_HX_RM22kairo_Pair_i32f32`

---

### Function Parameters
Function parameters are encoded in the order they appear, using the **Kairo Type Encoding**.

#### Function Syntax
```
_HX_FN<Length><Module_and_FunctionName><EncodedParameters>
```

#### Examples
1. Function with parameters:
   ```kairo
   fn add(a: i32, b: i32) -> i32;
   ```
   Mangled: `_HX_FN9addi32i32`

2. Function pointer:
   ```kairo
   fn apply(func: fn (i32, i32) -> i32) -> void;
   ```
   Mangled: `_HX_FN10applyFN_i32i32_i32`

3. Const overloading:
   ```kairo
   fn add(a: i32, b: i32) const -> i32;
   ```
   Mangled: `_HX_FN13addi32i32_C`

4. Unsafe overloading:
   ```kairo
   fn add(a: i32, b: i32) unsafe -> i32;
   ```
   Mangled: `_HX_FN17addi32i32_U`

---

### Operators

Operators are encoded as `_OP` followed by the operator type.
Operators if they have an alias, get 2 encodings, one for the operator and one for the alias.

#### Examples
1. Binary operator:
   ```kairo
   fn op + (a: i32, b: i32) -> i32;
   ```
   Mangled: `_HX_OP$sym_add$i32i32`

2. Unary operator with alias:
   ```kairo
   fn op l- (a: i32)[prefix_negate] -> i32;
   ```
   Mangled: `_HX_OP$sym_lsub$i32_Ri32`; `_HX_FN12prefix_negatei32`

3. Assignment operator:
   ```kairo
   fn op = (a: self)[set] -> *self;
   ```
   Mangled: `_HX_OP$sym_eq$selfself`

---

### RTTI and VTables

1. **RTTI**:
   Encoded with `_HX_TI`:
   ```kairo
   class MyClass;
   ```
   Mangled: `_HX_TI7MyClass`

2. **Virtual Table**:
   Encoded with `_HX_VT`:
   ```kairo
   class MyClass {
       virtual fn foo() -> void;
   }
   ```
   Mangled: `_HX_VT7MyClass`

---

## Examples

### Functions
1. A function pointer returning `f32`:
   ```kairo
   fn callback(fn (i32, i32) -> f32) -> void;
   ```
   Mangled: `_HX_FN12callback_FN_i32i32_f32`

2. A generic function in a module:
   ```kairo
   module math {
       fn max(a: T, b: T) -> T requires <T>;
   }
   ```
   Mangled: `_HX_FN13math_max_RM_T_T_T`




## Exception Handling

The Kairo ABI uses the same exception handling model as the Itanium ABI, including stack unwinding and type-based exception matching.

### Mangling of Exception-Related Symbols

- **Type Info**: `_HX_TI<type_name>`
- **Catch Blocks**:
  - Encodes types in the catch statement for precise matching.

---

## Object Layout

The Kairo ABI uses the same rules as the Itanium ABI for object layout, including:
- Base class offsets.
- Virtual base pointer positions.
- Alignment and padding rules.