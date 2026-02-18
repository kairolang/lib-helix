///--- The Kairo Project
///------------------------------------------------------------------------///
///                                                                                              ///
///   Part of the Kairo Project, under the Attribution 4.0 International license
///   (CC BY 4.0).    /// You are allowed to use, modify, redistribute, and
///   create derivative works, even for        /// commercial purposes, provided
///   that you give appropriate credit, and indicate if changes    /// were
///   made. ///
///                                                                                              ///
///   For more information on the license terms and requirements, please visit:
///   ///
///     https://creativecommons.org/licenses/by/4.0/ ///
///                                                                                              ///
///   SPDX-License-Identifier: CC-BY-4.0 /// Copyright (c) 2024 The Kairo
///   Project (CC BY 4.0)                                           ///
///                                                                                              ///
///--------------------------------------------------------------------------------
///lib-helix ---///

#ifndef _$_HX_CORE_M10PRIMITIVES
#define _$_HX_CORE_M10PRIMITIVES

#include <include/c++/libc++.hh>
#include <include/config/config.hh>
#include <include/meta/enable_if.hh>
#include <include/meta/traits.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

class null_t {};
H_STD_NAMESPACE_END
H_NAMESPACE_END

using char32 = char32_t;

using u8 = uint8_t;
using i8 = int8_t;

using u16 = uint16_t;
using i16 = int16_t;

using u32 = uint32_t;
using i32 = int32_t;

using u64 = uint64_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;
using f80 = long double;

#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) ||       \
    defined(__ppc64__) || defined(__aarch64__) || defined(__arm64__) ||  \
    defined(__mips64__) || defined(__mips64) || defined(__mips64el__) || \
    defined(__mips64el) || defined(__s390x__) || defined(__KAIRO_64BIT__)
#define __KAIRO_64BIT__
using usize = u64;
using isize = i64;
#elif defined(__ILP32__) || defined(_WIN32) || defined(__i386__) ||  \
    defined(__arm__) || defined(__mips__) || defined(__mips) ||      \
    defined(__mipsel__) || defined(__mipsel) || defined(__s390__) || \
    defined(__KAIRO_32BIT__)
#define __KAIRO_32BIT__
using usize = u32;
using isize = i32;
#elif defined(__16BIT__) || defined(__MSP430__) || defined(__AVR__) || \
    defined(__KAIRO_16BIT__)
#define __KAIRO_16BIT__
using usize = u16;
using isize = i16;
#elif defined(__8BIT__) || defined(__KAIRO_8BIT__)
#define __KAIRO_8BIT__
using usize = u8;
using isize = i8;
#else
#error \
    "Kairo core: Unable to determine platform bitness. Supported macros: __KAIRO_64BIT__, __KAIRO_32BIT__, __KAIRO_16BIT__, __KAIRO_8BIT__. Define one explicitly using '-D' during compilation."
#endif

using _H_RESERVED$char = wchar_t;

namespace kairo::std::Legacy {
    using _H_RESERVED$char = char;
    template <typename T>
    using array = T[];
}  // namespace kairo::std::Legacy

inline constexpr kairo::std::null_t null;

// FIXME: remove later
// template <class T, class A = kairo::libcxx::allocator<T>>
// class vec : public kairo::libcxx::vector<T> {};

/*
[" hi ", "wsg"]
*/

template <typename T>
using vec = kairo::libcxx::vector<T>;

template <typename K,
          typename V,
          class C = kairo::libcxx::less<K>,
          class A = kairo::libcxx::allocator<kairo::libcxx::pair<const K, V>>>
using map = kairo::libcxx::map<K, V, C, A>;

template <typename T, usize S>
using array = kairo::libcxx::array<T, S>;

template <typename T>
using list = kairo::libcxx::list<T>;

template <typename T,
          class C = kairo::libcxx::less<T>,
          class A = kairo::libcxx::allocator<T>>
using set = kairo::libcxx::set<T, C, A>;

template <typename... T>
using tuple = kairo::libcxx::tuple<T...>;

struct i128;

struct u128 {
    u64 high;
    u64 low;

    // Constructors
    inline u128();
    inline u128(u64 val);
    inline u128(u32 val);
    inline u128(u16 val);
    inline u128(u8 val);
    inline u128(i64 val);
    inline u128(i32 val);
    inline u128(i16 val);
    inline u128(i8 val);
    inline u128(u64 high, u64 low);
    inline u128(const i128 &x);

    // Arithmetic Operators
    inline u128 operator+(const u128 &other) const;
    inline u128 operator-(const u128 &other) const;
    inline u128 operator*(const u128 &other) const;
    inline u128 operator/(const u128 &divisor) const;
    inline u128 operator%(const u128 &divisor) const;

    // Bitwise Operators
    inline u128 operator&(const u128 &other) const;
    inline u128 operator|(const u128 &other) const;
    inline u128 operator^(const u128 &other) const;
    inline u128 operator~() const;
    inline u128 operator<<(int shift) const;
    inline u128 operator>>(int shift) const;

    // Comparison Operators
    inline bool operator==(const u128 &other) const;
    inline bool operator!=(const u128 &other) const;
    inline bool operator<(const u128 &other) const;
    inline bool operator>(const u128 &other) const;
    inline bool operator<=(const u128 &other) const;
    inline bool operator>=(const u128 &other) const;

    // Assignment Operators
    inline u128 &operator=(const u128 &other);
    inline u128 &operator+=(const u128 &other);
    inline u128 &operator-=(const u128 &other);
    inline u128 &operator*=(const u128 &other);
    inline u128 &operator/=(const u128 &other);
    inline u128 &operator%=(const u128 &other);
    inline u128 &operator&=(const u128 &other);
    inline u128 &operator|=(const u128 &other);
    inline u128 &operator^=(const u128 &other);
    inline u128 &operator<<=(int shift);
    inline u128 &operator>>=(int shift);

    // Increment/Decrement Operators
    inline u128 &operator++();
    inline u128  operator++(int);
    inline u128 &operator--();
    inline u128  operator--(int);

    // Unary Operators
    inline u128 operator+() const;
    inline u128 operator-() const;

  private:
    inline static u128 mul_u64_to_u128(u64 a, u64 b);
};

struct i128 {
    u64 high;
    u64 low;

    // Constructors
    inline i128();
    inline i128(u64 val);
    inline i128(u32 val);
    inline i128(u16 val);
    inline i128(u8 val);
    inline i128(i64 val);
    inline i128(i32 val);
    inline i128(i16 val);
    inline i128(i8 val);
    inline i128(const u128 &x);
    inline i128(u64 high, u64 low);

    // Helper Functions
    inline bool is_negative() const;

    // Arithmetic Operators
    inline i128 operator+(const i128 &other) const;
    inline i128 operator-(const i128 &other) const;
    inline i128 operator*(const i128 &other) const;
    inline i128 operator/(const i128 &other) const;
    inline i128 operator%(const i128 &other) const;

    // Bitwise Operators
    inline i128 operator&(const i128 &other) const;
    inline i128 operator|(const i128 &other) const;
    inline i128 operator^(const i128 &other) const;
    inline i128 operator~() const;
    inline i128 operator<<(int shift) const;
    inline i128 operator>>(int shift) const;

    // Comparison Operators
    inline bool operator==(const i128 &other) const;
    inline bool operator!=(const i128 &other) const;
    inline bool operator<(const i128 &other) const;
    inline bool operator>(const i128 &other) const;
    inline bool operator<=(const i128 &other) const;
    inline bool operator>=(const i128 &other) const;

    // Assignment Operators
    inline i128 &operator=(const i128 &other);
    inline i128 &operator+=(const i128 &other);
    inline i128 &operator-=(const i128 &other);
    inline i128 &operator*=(const i128 &other);
    inline i128 &operator/=(const i128 &other);
    inline i128 &operator%=(const i128 &other);
    inline i128 &operator&=(const i128 &other);
    inline i128 &operator|=(const i128 &other);
    inline i128 &operator^=(const i128 &other);
    inline i128 &operator<<=(int shift);
    inline i128 &operator>>=(int shift);

    // Increment/Decrement Operators
    inline i128 &operator++();
    inline i128  operator++(int);
    inline i128 &operator--();
    inline i128  operator--(int);

    // Unary Operators
    inline i128 operator+() const;
    inline i128 operator-() const;
};

#endif  // _$_HX_CORE_M10PRIMITIVES
