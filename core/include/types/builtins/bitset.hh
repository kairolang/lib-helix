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

#ifndef _$_HX_CORE_M6BITSET
#define _$_HX_CORE_M6BITSET

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/runtime/__panic/panic_fwd.hh>
#include <include/meta/traits.hh>
#include <include/meta/enable_if.hh>
#include <include/types/builtins/size_t.hh>
#include <include/types/builtins/primitives.hh>
#include <include/types/builtins/num_data.hh>

H_NAMESPACE_BEGIN
// -----------------------------------------------------------------------------
// Helper: Character-to-Integer Conversion
// -----------------------------------------------------------------------------
KAIRO_FORCE_INLINE constexpr int chr_to_i32(char c, int base) noexcept {
    if (c >= '0' && c <= '9') {
        int d = c - '0';
        return (d < base) ? d : -1;
    }
    if (c >= 'a' && c <= 'f') {
        int d = c - 'a' + 10;
        return (d < base) ? d : -1;
    }
    if (c >= 'A' && c <= 'F') {
        int d = c - 'A' + 10;
        return (d < base) ? d : -1;
    }
    return -1;
}

// -----------------------------------------------------------------------------
// Forward Declaration of __BitSet
// -----------------------------------------------------------------------------
template <typename... Ts>
struct __BitSet;

// Special-case: __BitSet<__BitSet<T>> simply inherits from __BitSet<T>
template <typename T>
struct __BitSet<__BitSet<T>> : public __BitSet<T> {
    using __BitSet<T>::__BitSet;
};

// -----------------------------------------------------------------------------
// __BitSet for a Single Numeric Type T
// -----------------------------------------------------------------------------
template <typename T>
struct __BitSet<T> {
    T value;
    using c_type = T;

    KAIRO_FORCE_INLINE constexpr __BitSet()
        : value(0) {}
    KAIRO_FORCE_INLINE constexpr __BitSet(T val)
        : value(val) {}

    template <typename U, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
    KAIRO_FORCE_INLINE constexpr __BitSet(const U val) noexcept
        : value(static_cast<T>(val)) {
#if defined(__clang__) || defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic push
#pragma GCC diagnostic warning "-Woverflow"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(1 : 4307)  // Warning C4307: integral constant overflow
#endif

        if (val > static_cast<U>(__NumData<T>::max) || val < static_cast<U>(__NumData<T>::min)) {
            value = static_cast<T>(val);
        }

#if defined(__clang__) || defined(__GNUC__) || defined(__MINGW32__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif
    }

    // Total digits/bits (as defined in __NumData)
    KAIRO_FORCE_INLINE static constexpr unsigned bits() noexcept { return __NumData<T>::digits; }

    // Get a specific bit (0 = least-significant)
    [[nodiscard]] KAIRO_FORCE_INLINE constexpr bool get_bit(unsigned idx) const noexcept {
        return (value >> idx) & 1;
    }

    // Set a specific bit.
    KAIRO_FORCE_INLINE constexpr void set_bit(unsigned idx, bool b) noexcept {
        if (b) {
            value |= (T(1) << idx);
        } else {
            value &= ~(T(1) << idx);
        }
    }

    // Explicit conversion to underlying type.
    KAIRO_FORCE_INLINE constexpr operator T() const noexcept { return value; }

    KAIRO_FORCE_INLINE constexpr __BitSet &operator=(T val) noexcept {
        value = val;
        return *this;
    }

    // ── Arithmetic Operators ──
    // Addition operator with compile-time diagnostic and runtime check in debug.
    KAIRO_FORCE_INLINE constexpr __BitSet operator+(const __BitSet &other) const noexcept
    // DIAGNOSE_IF(((__builtin_constant_p(value) && __builtin_constant_p(other.value)) &&
    //                  (__NumData<T>::is_signed
    //                       ? ((other.value > 0 && value > __NumData<T>::max - other.value) ||
    //                          (other.value < 0 && value < __NumData<T>::min - other.value))
    //                       : (value > __NumData<T>::max - other.value)),
    //              "Addition out of bounds",
    //              "warning"))
    {
        T res = value + other.value;
        return __BitSet(res);
    }

    int some_func(const int *oj);

    KAIRO_FORCE_INLINE constexpr __BitSet &operator+=(const __BitSet &other) noexcept {
        *this = *this + other;
        return *this;
    }

    KAIRO_FORCE_INLINE constexpr __BitSet operator-(const __BitSet &other) const noexcept {
        return __BitSet(value - other.value);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator-=(const __BitSet &other) noexcept {
        value -= other.value;
        return *this;
    }

    // Naive multiplication (bit-iterative)
    KAIRO_FORCE_INLINE constexpr __BitSet operator*(const __BitSet &other) const noexcept {
        __BitSet result(0);
        for (unsigned i = 0; i < bits(); i++) {
            if (other.get_bit(i)) {
                result += (__BitSet(value) << i);
            }
        }
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator*=(const __BitSet &other) noexcept {
        *this = *this * other;
        return *this;
    }

    // Naive division (returns quotient)
    KAIRO_FORCE_INLINE constexpr __BitSet operator/(const __BitSet &other) const noexcept {
        __BitSet quotient(0);
        __BitSet remainder(0);
        for (int i = bits() - 1; i >= 0; i--) {
            remainder = remainder.operator<<(__BitSet(1));
            remainder.set_bit(0, get_bit(i));
            if (remainder >= other) {
                remainder -= other;
                quotient.set_bit(i, true);
            }
        }
        return quotient;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator/=(const __BitSet &other) noexcept {
        *this = *this / other;
        return *this;
    }

    // Naive modulo.
    KAIRO_FORCE_INLINE constexpr __BitSet operator%(const __BitSet &other) const noexcept {
        __BitSet quotient = *this / other;
        return *this - (quotient * other);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator%=(const __BitSet &other) noexcept {
        *this = *this % other;
        return *this;
    }

    // ── Bitwise Operators ──
    KAIRO_FORCE_INLINE constexpr __BitSet operator&(const __BitSet &other) const noexcept {
        return __BitSet(value & other.value);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator|(const __BitSet &other) const noexcept {
        return __BitSet(value | other.value);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator^(const __BitSet &other) const noexcept {
        return __BitSet(value ^ other.value);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet  operator~() const noexcept { return __BitSet(~value); }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator&=(const __BitSet &other) noexcept {
        value &= other.value;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator|=(const __BitSet &other) noexcept {
        value |= other.value;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator^=(const __BitSet &other) noexcept {
        value ^= other.value;
        return *this;
    }

    // ── Shift Operators (naive bit-loop) ──
    KAIRO_FORCE_INLINE constexpr __BitSet operator<<(unsigned shift) const noexcept {
        __BitSet result(0);
        for (unsigned i = 0; i < bits(); i++) {
            if (get_bit(i) && (i + shift < bits())) {
                result.set_bit(i + shift, true);
            }
        }
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator<<=(unsigned shift) noexcept {
        *this = *this << shift;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator>>(unsigned shift) const noexcept {
        __BitSet result(0);
        for (unsigned i = shift; i < bits(); i++) {
            if (get_bit(i)) {
                result.set_bit(i - shift, true);
            }
        }
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator>>=(unsigned shift) noexcept {
        *this = *this >> shift;
        return *this;
    }

    // ── Increment / Decrement ──
    KAIRO_FORCE_INLINE constexpr __BitSet &operator++() noexcept {
        *this += __BitSet(1);
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator++(int) noexcept {
        __BitSet temp = *this;
        ++(*this);
        return temp;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator--() noexcept {
        *this -= __BitSet(1);
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator--(int) noexcept {
        __BitSet temp = *this;
        --(*this);
        return temp;
    }

    // ── Comparison Operators ──
    KAIRO_FORCE_INLINE constexpr bool operator==(const __BitSet &other) const noexcept {
        return value == other.value;
    }
    KAIRO_FORCE_INLINE constexpr bool operator!=(const __BitSet &other) const noexcept {
        return value != other.value;
    }
    KAIRO_FORCE_INLINE constexpr bool operator<(const __BitSet &other) const noexcept {
        return value < other.value;
    }
    KAIRO_FORCE_INLINE constexpr bool operator<=(const __BitSet &other) const noexcept {
        return value <= other.value;
    }
    KAIRO_FORCE_INLINE constexpr bool operator>(const __BitSet &other) const noexcept {
        return value > other.value;
    }
    KAIRO_FORCE_INLINE constexpr bool operator>=(const __BitSet &other) const noexcept {
        return value >= other.value;
    }

    // ── Utility and Conversion Methods ──
    static constexpr const T    digits    = __NumData<T>::digits;
    static constexpr const T    max       = __NumData<T>::max;
    static constexpr const T    min       = __NumData<T>::min;
    static constexpr const bool is_signed = __NumData<T>::is_signed;
    static constexpr const bool is_radix  = __NumData<T>::is_radix;

    KAIRO_FORCE_INLINE static constexpr __BitSet max_value() noexcept { return max; }
    KAIRO_FORCE_INLINE static constexpr __BitSet min_value() noexcept { return min; }
    KAIRO_FORCE_INLINE constexpr T               abs() const noexcept {
        return is_signed ? (value < 0 ? -value : value) : value;
    }

    // Conversion to a narrow C-string.
    [[nodiscard]] KAIRO_FORCE_INLINE const char *to_cstr() const noexcept {
        static thread_local char buf[digits + 1];
        __BitSet                 copy = *this;
        int                      pos  = 0;
        __BitSet                 zero(0);
        __BitSet                 ten(10);
        if (copy == zero) {
            buf[0] = '0';
            buf[1] = '\0';
            return buf;
        }
        while (copy > zero) {
            __BitSet rem = copy % ten;
            copy /= ten;
            int digit  = static_cast<int>(rem.value);
            buf[pos++] = '0' + digit;
        }
        for (int i = 0; i < pos / 2; i++) {
            char tmp         = buf[i];
            buf[i]           = buf[pos - 1 - i];
            buf[pos - 1 - i] = tmp;
        }
        buf[pos] = '\0';
        return buf;
    }

    // Conversion to a wide C-string.
    [[nodiscard]] KAIRO_FORCE_INLINE const wchar_t *to_wcstr() const noexcept {
        static thread_local wchar_t buf[digits + 1];
        __BitSet                    copy = *this;
        int                         pos  = 0;
        __BitSet                    zero(0);
        __BitSet                    ten(10);
        if (copy == zero) {
            buf[0] = L'0';
            buf[1] = L'\0';
            return buf;
        }
        while (copy > zero) {
            __BitSet rem = copy % ten;
            copy /= ten;
            int digit  = static_cast<int>(rem.value);
            buf[pos++] = L'0' + digit;
        }
        for (int i = 0; i < pos / 2; i++) {
            wchar_t tmp      = buf[i];
            buf[i]           = buf[pos - 1 - i];
            buf[pos - 1 - i] = tmp;
        }
        buf[pos] = L'\0';
        return buf;
    }

    // Explicit conversion to bool.
    KAIRO_FORCE_INLINE constexpr explicit operator bool() const noexcept {
        return *this != __BitSet(0);
    }

    // ── Parsing from a Narrow C-string ──
    KAIRO_FORCE_INLINE static constexpr __BitSet from_cstr(const char    *ptr,
                                                           libcxx::size_t len) noexcept {
        int            base  = 10;
        libcxx::size_t start = 0;
        if (len >= 2 && ptr[0] == '0') {
            if (ptr[1] == 'x' || ptr[1] == 'X') {
                base  = 16;
                start = 2;
            } else if (ptr[1] == 'b' || ptr[1] == 'B') {
                base  = 2;
                start = 2;
            } else {
                base  = 8;
                start = 1;
            }
        }
        __BitSet result(0);
        __BitSet base_val(base);
        for (libcxx::size_t i = start; i < len; ++i) {
            char c = ptr[i];
            if (c == '_') {
                continue;
            }
            int digit = chr_to_i32(c, base);
            if (digit < 0) {
                break;
            }
            result = result * __BitSet(base) + __BitSet(digit);
        }
        return result;
    }

    // ── Parsing from a Wide C-string ──
    KAIRO_FORCE_INLINE static constexpr __BitSet from_cstr(const wchar_t *ptr,
                                                           libcxx::size_t len) noexcept {
        int            base  = 10;
        libcxx::size_t start = 0;
        if (len >= 2 && ptr[0] == L'0') {
            if (ptr[1] == L'x' || ptr[1] == L'X') {
                base  = 16;
                start = 2;
            } else if (ptr[1] == L'b' || ptr[1] == L'B') {
                base  = 2;
                start = 2;
            } else {
                base  = 8;
                start = 1;
            }
        }
        __BitSet result(0);
        __BitSet base_val(base);
        for (libcxx::size_t i = start; i < len; ++i) {
            wchar_t c = ptr[i];
            if (c == L'_') {
                continue;
            }
            int digit = chr_to_i32(static_cast<char>(c), base);
            if (digit < 0) {
                break;
            }
            result = result * __BitSet(base) + __BitSet(digit);
        }
        return result;
    }
};

// -----------------------------------------------------------------------------
// Composite __BitSet for Multiple Numeric Types
// -----------------------------------------------------------------------------
template <typename Head, typename... Tail>
struct __BitSet<__BitSet<Head>, __BitSet<Tail>...> {
    __BitSet<Head>    head;
    __BitSet<Tail...> tail;

    KAIRO_FORCE_INLINE constexpr __BitSet()
        : head(0)
        , tail() {}

    KAIRO_FORCE_INLINE constexpr __BitSet(Head h)
        : head(h)
        , tail() {}

    KAIRO_FORCE_INLINE constexpr __BitSet(Head h, Tail... t)
        : head(h)
        , tail(t...) {}

#define NARROWING_WARNING_PUSH                                                               \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic warning \"-Wnarrowing\"")         \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic warning \"-Wnarrowing\"") \
            _Pragma("warning(push)") _Pragma("warning(1 : 4244)")  // MSVC: possible loss of data

#define NARROWING_WARNING_POP \
    _Pragma("GCC diagnostic pop") _Pragma("clang diagnostic pop") _Pragma("warning(pop)")

    // Signed conversions
    KAIRO_FORCE_INLINE constexpr explicit operator signed char() const noexcept {
        NARROWING_WARNING_PUSH
        signed char result = static_cast<signed char>(tail) +
                             (static_cast<signed char>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator signed short() const noexcept {
        NARROWING_WARNING_PUSH
        signed short result = static_cast<signed short>(tail) +
                              (static_cast<signed short>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator signed int() const noexcept {
        NARROWING_WARNING_PUSH
        signed int result = static_cast<signed int>(tail) +
                            (static_cast<signed int>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator signed long() const noexcept {
        NARROWING_WARNING_PUSH
        signed long result = static_cast<signed long>(tail) +
                             (static_cast<signed long>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator signed long long() const noexcept {
        NARROWING_WARNING_PUSH
        signed long long result =
            static_cast<signed long long>(tail) +
            (static_cast<signed long long>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    // Unsigned conversions
    KAIRO_FORCE_INLINE constexpr explicit operator unsigned char() const noexcept {
        NARROWING_WARNING_PUSH
        unsigned char result = static_cast<unsigned char>(tail) +
                               (static_cast<unsigned char>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator unsigned short() const noexcept {
        NARROWING_WARNING_PUSH
        unsigned short result = static_cast<unsigned short>(tail) +
                                (static_cast<unsigned short>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator unsigned int() const noexcept {
        NARROWING_WARNING_PUSH
        unsigned int result = static_cast<unsigned int>(tail) +
                              (static_cast<unsigned int>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator unsigned long() const noexcept {
        NARROWING_WARNING_PUSH
        unsigned long result = static_cast<unsigned long>(tail) +
                               (static_cast<unsigned long>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

    KAIRO_FORCE_INLINE constexpr explicit operator unsigned long long() const noexcept {
        NARROWING_WARNING_PUSH
        unsigned long long result =
            static_cast<unsigned long long>(tail) +
            (static_cast<unsigned long long>(head) << __BitSet<Tail...>::bits());
        NARROWING_WARNING_POP
        return result;
    }

#undef NARROWING_WARNING_PUSH
#undef NARROWING_WARNING_POP

    // Total bits = bits in head + bits in tail.
    KAIRO_FORCE_INLINE static constexpr unsigned bits() noexcept {
        return __BitSet<Head>::digits + __BitSet<Tail...>::bits();
    }

    // Get and set a bit (bit 0 is the least-significant, living in the tail).
    [[nodiscard]] KAIRO_FORCE_INLINE constexpr bool get_bit(unsigned idx) const noexcept {
        constexpr unsigned tail_bits = __BitSet<Tail...>::bits();
        if (idx < tail_bits) {
            return tail.get_bit(idx);
        }
        return ((head >> (idx - tail_bits)) & __BitSet(1));
    }
    KAIRO_FORCE_INLINE constexpr void set_bit(unsigned idx, bool b) noexcept {
        constexpr unsigned tail_bits = __BitSet<Tail...>::bits();
        if (idx < tail_bits) {
            tail.set_bit(idx, b);
        } else {
            unsigned bitpos = idx - tail_bits;
            if (b) {
                head |= (Head(1) << bitpos);
            } else {
                head &= ~(Head(1) << bitpos);
            }
        }
    }

    // ── Arithmetic Operators ──
    KAIRO_FORCE_INLINE constexpr __BitSet operator+(const __BitSet &other) const noexcept {
        __BitSet result;
        result.tail = tail + other.tail;
        bool carry  = (result.tail < tail);
        result.head = head + other.head + __BitSet(carry ? 1 : 0);
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator+=(const __BitSet &other) noexcept {
        *this = *this + other;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator-(const __BitSet &other) const noexcept {
        __BitSet result;
        bool     borrow = (tail < other.tail);
        result.tail     = tail - other.tail;
        result.head     = head - other.head - __BitSet<decltype(head.value)>(borrow ? 1 : 0);
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator-=(const __BitSet &other) noexcept {
        *this = *this - other;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator*(const __BitSet &other) const noexcept {
        __BitSet result(0);
        unsigned total_bits = bits();
        for (unsigned i = 0; i < total_bits; i++) {
            if (other.get_bit(i)) {
                result += (*this << i);
            }
        }
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator*=(const __BitSet &other) noexcept {
        *this = *this * other;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator/(const __BitSet &other) const noexcept {
        __BitSet quotient(0);
        __BitSet remainder(0);
        unsigned total_bits = bits();
        for (long i = total_bits - 1; i >= 0; i--) {
            remainder = remainder << 1;
            remainder.set_bit(0, get_bit(i));
            if (remainder >= other) {
                remainder -= other;
                quotient.set_bit(i, true);
            }
        }
        return quotient;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator/=(const __BitSet &other) noexcept {
        *this = *this / other;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator%(const __BitSet &other) const noexcept {
        __BitSet quotient = *this / other;
        return *this - (quotient * other);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator%=(const __BitSet &other) noexcept {
        *this = *this % other;
        return *this;
    }

    // ── Bitwise Operators (elementwise) ──
    KAIRO_FORCE_INLINE constexpr __BitSet operator&(const __BitSet &other) const noexcept {
        return __BitSet(head & other.head, tail & other.tail);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator|(const __BitSet &other) const noexcept {
        return __BitSet(head | other.head, tail | other.tail);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator^(const __BitSet &other) const noexcept {
        return __BitSet(head ^ other.head, tail ^ other.tail);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator~() const noexcept {
        return __BitSet(~head, ~tail);
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator&=(const __BitSet &other) noexcept {
        head &= other.head;
        tail &= other.tail;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator|=(const __BitSet &other) noexcept {
        head |= other.head;
        tail |= other.tail;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator^=(const __BitSet &other) noexcept {
        head ^= other.head;
        tail ^= other.tail;
        return *this;
    }

    // ── Shift Operators ──
    KAIRO_FORCE_INLINE constexpr __BitSet operator<<(unsigned shift) const noexcept {
        __BitSet result(0);
        unsigned total_bits = bits();
        for (unsigned i = 0; i < total_bits; i++) {
            if (get_bit(i)) {
                if (i + shift < total_bits) {
                    result.set_bit(i + shift, true);
                }
            }
        }
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator<<=(unsigned shift) noexcept {
        *this = *this << shift;
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator>>(unsigned shift) const noexcept {
        __BitSet result(0);
        unsigned total_bits = bits();
        for (unsigned i = shift; i < total_bits; i++) {
            if (get_bit(i)) {
                result.set_bit(i - shift, true);
            }
        }
        return result;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator>>=(unsigned shift) noexcept {
        *this = *this >> shift;
        return *this;
    }

    // ── Increment / Decrement ──
    KAIRO_FORCE_INLINE constexpr __BitSet &operator++() noexcept {
        *this += __BitSet(1);
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator++(int) noexcept {
        __BitSet temp = *this;
        ++(*this);
        return temp;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet &operator--() noexcept {
        *this -= __BitSet(1);
        return *this;
    }
    KAIRO_FORCE_INLINE constexpr __BitSet operator--(int) noexcept {
        __BitSet temp = *this;
        --(*this);
        return temp;
    }

    // ── Comparison Operators (lexicographical: head first) ──
    KAIRO_FORCE_INLINE constexpr bool operator==(const __BitSet &other) const noexcept {
        return head == other.head && tail == other.tail;
    }
    KAIRO_FORCE_INLINE constexpr bool operator!=(const __BitSet &other) const noexcept {
        return !(*this == other);
    }
    KAIRO_FORCE_INLINE constexpr bool operator<(const __BitSet &other) const noexcept {
        if (head < other.head) {
            return true;
        }
        if (head > other.head) {
            return false;
        }
        return tail < other.tail;
    }
    KAIRO_FORCE_INLINE constexpr bool operator<=(const __BitSet &other) const noexcept {
        return (*this < other) || (*this == other);
    }
    KAIRO_FORCE_INLINE constexpr bool operator>(const __BitSet &other) const noexcept {
        return !(*this <= other);
    }
    KAIRO_FORCE_INLINE constexpr bool operator>=(const __BitSet &other) const noexcept {
        return !(*this < other);
    }

    static constexpr unsigned digits = __BitSet<Head>::digits + __BitSet<Tail...>::digits;

    KAIRO_FORCE_INLINE static constexpr __BitSet max_value() noexcept {
        return __BitSet(__BitSet<Head>::max_value(), __BitSet<Tail...>::max_value());
    }
    KAIRO_FORCE_INLINE static constexpr __BitSet min_value() noexcept {
        return __BitSet(static_cast<Head>(0), __BitSet<Tail...>::min_value());
    }

    static constexpr bool is_signed = __BitSet<Head>::is_signed;
    static constexpr bool is_radix  = __BitSet<Head>::is_radix;

    KAIRO_FORCE_INLINE constexpr __BitSet abs() const noexcept {
        return __BitSet(head.abs(), tail.abs());
    }

    KAIRO_FORCE_INLINE constexpr explicit operator bool() const noexcept {
        return !(*this == __BitSet(0, __BitSet<Tail...>::min));
    }

    // ── Conversion to C-string (narrow) ──
    [[nodiscard]] KAIRO_FORCE_INLINE const char *to_cstr() const noexcept {
        static thread_local char buf[digits + 1];
        __BitSet                 copy = *this;
        int                      pos  = 0;
        __BitSet                 zero(0, __BitSet<Tail...>::min);
        __BitSet                 ten(10, __BitSet<Tail...>::min);
        if (copy == zero) {
            buf[0] = '0';
            buf[1] = '\0';
            return buf;
        }
        while (copy > zero) {
            __BitSet rem = copy % ten;
            copy /= ten;
            int digit  = static_cast<int>(rem);
            buf[pos++] = '0' + digit;
        }
        for (int i = 0; i < pos / 2; i++) {
            char tmp         = buf[i];
            buf[i]           = buf[pos - 1 - i];
            buf[pos - 1 - i] = tmp;
        }
        buf[pos] = '\0';
        return buf;
    }

    // ── Conversion to C-string (wide) ──
    [[nodiscard]] KAIRO_FORCE_INLINE const wchar_t *to_wcstr() const noexcept {
        static thread_local wchar_t buf[digits + 1];
        __BitSet                    copy = *this;
        int                         pos  = 0;
        __BitSet                    zero(0, __BitSet<Tail...>::min);
        __BitSet                    ten(10, __BitSet<Tail...>::min);
        if (copy == zero) {
            buf[0] = L'0';
            buf[1] = L'\0';
            return buf;
        }
        while (copy > zero) {
            __BitSet rem = copy % ten;
            copy /= ten;
            int digit  = static_cast<int>(rem);
            buf[pos++] = L'0' + digit;
        }
        for (int i = 0; i < pos / 2; i++) {
            wchar_t tmp      = buf[i];
            buf[i]           = buf[pos - 1 - i];
            buf[pos - 1 - i] = tmp;
        }
        buf[pos] = L'\0';
        return buf;
    }

    // ── Parsing from a Narrow C-string ──
    KAIRO_FORCE_INLINE static __BitSet from_cstr(const char *ptr, libcxx::size_t len) noexcept {
        int            base  = 10;
        libcxx::size_t start = 0;
        if (len >= 2 && ptr[0] == '0') {
            if (ptr[1] == 'x' || ptr[1] == 'X') {
                base  = 16;
                start = 2;
            } else if (ptr[1] == 'b' || ptr[1] == 'B') {
                base  = 2;
                start = 2;
            } else {
                base  = 8;
                start = 1;
            }
        }
        __BitSet result(0, __BitSet<Tail...>::min);
        __BitSet base_val(base, __BitSet<Tail...>::min);
        for (libcxx::size_t i = start; i < len; ++i) {
            char c = ptr[i];
            if (c == '_') {
                continue;
            }
            int digit = chr_to_i32(c, base);
            if (digit < 0) {
                break;
            }
            result = result * __BitSet(base, __BitSet<Tail...>::min) +
                     __BitSet(digit, __BitSet<Tail...>::min);
        }
        return result;
    }

    // ── Parsing from a Wide C-string ──
    KAIRO_FORCE_INLINE static __BitSet from_cstr(const wchar_t *ptr, libcxx::size_t len) noexcept {
        int            base  = 10;
        libcxx::size_t start = 0;
        if (len >= 2 && ptr[0] == L'0') {
            if (ptr[1] == L'x' || ptr[1] == L'X') {
                base  = 16;
                start = 2;
            } else if (ptr[1] == L'b' || ptr[1] == L'B') {
                base  = 2;
                start = 2;
            } else {
                base  = 8;
                start = 1;
            }
        }
        __BitSet result(0, __BitSet<Tail...>::min);
        __BitSet base_val(base, __BitSet<Tail...>::min);
        for (libcxx::size_t i = start; i < len; ++i) {
            wchar_t c = ptr[i];
            if (c == L'_') {
                continue;
            }
            int digit = chr_to_i32(static_cast<char>(c), base);
            if (digit < 0) {
                break;
            }
            result = result * __BitSet(base, __BitSet<Tail...>::min) +
                     __BitSet(digit, __BitSet<Tail...>::min);
        }
        return result;
    }
};

template class __BitSet<signed char>;
template class __BitSet<signed short>;
template class __BitSet<signed int>;
template class __BitSet<signed long>;
template class __BitSet<signed long long>;
template class __BitSet<__BitSet<signed long long>, __BitSet<signed long long>>;

template class __BitSet<unsigned char>;
template class __BitSet<unsigned short>;
template class __BitSet<unsigned int>;
template class __BitSet<unsigned long>;
template class __BitSet<unsigned long long>;
template class __BitSet<__BitSet<unsigned long long>, __BitSet<unsigned long long>>;

// -----------------------------------------------------------------------------
// Non-Member Overloads to Allow Arithmetic with Built-in Types
// (This lets you write expressions like: 1 + u64)
// -----------------------------------------------------------------------------
// ── Addition ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator+(const __BitSet<T> &lhs, U rhs) noexcept
    DIAGNOSE_IF(((__builtin_constant_p(rhs) && __builtin_constant_p(lhs.value)) &&
                     (__NumData<T>::is_signed ? ((lhs > 0 && rhs > __NumData<T>::max - lhs) ||
                                                 (lhs < 0 && rhs < __NumData<T>::min - lhs))
                                              : (rhs > __NumData<T>::max - lhs)),
                 L"Addition out of bounds",
                 L"warning")) {
    return lhs + __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator+(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) + rhs;
}

// ── Subtraction ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator-(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs - __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator-(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) - rhs;
}

// ── Multiplication ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator*(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs * __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator*(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) * rhs;
}

// ── Division ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator/(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs / __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator/(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) / rhs;
}

// ── Modulo ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator%(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs % __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator%(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) % rhs;
}

// ── Bitwise AND ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator&(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs & __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator&(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) & rhs;
}

// ── Bitwise OR ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator|(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs | __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator|(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) | rhs;
}

// ── Bitwise XOR ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator^(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs ^ __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator^(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) ^ rhs;
}

// ── Left Shift ── (shift count must be integral)
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_integral_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator<<(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs << static_cast<unsigned>(rhs);
}

// ── Right Shift ── (shift count must be integral)
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_integral_v<U>>>
KAIRO_FORCE_INLINE constexpr __BitSet<T> operator>>(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs >> static_cast<unsigned>(rhs);
}

// ── Equality Comparison ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator==(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs == __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator==(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) == rhs;
}

// ── Inequality Comparison ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator!=(const __BitSet<T> &lhs, U rhs) noexcept {
    return !(lhs == rhs);
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator!=(U lhs, const __BitSet<T> &rhs) noexcept {
    return !(lhs == rhs);
}

// ── Less Than ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator<(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs < __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator<(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) < rhs;
}

// ── Less Than or Equal ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator<=(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs <= __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator<=(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) <= rhs;
}

// ── Greater Than ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator>(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs > __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator>(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) > rhs;
}

// ── Greater Than or Equal ──
template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator>=(const __BitSet<T> &lhs, U rhs) noexcept {
    return lhs >= __BitSet<T>(static_cast<T>(rhs));
}

template <typename U, typename T, typename = std::Meta::enable_if<libcxx::is_arithmetic_v<U>>>
KAIRO_FORCE_INLINE constexpr bool operator>=(U lhs, const __BitSet<T> &rhs) noexcept {
    return __BitSet<T>(static_cast<T>(lhs)) >= rhs;
}

namespace std::Reflection {
KAIRO_FORCE_INLINE constexpr usize cstr_length(const char *str) {
    return (*str != 0) ? usize(1 + cstr_length(str + 1)) : usize(0);
}
}  // namespace std::Reflection

H_NAMESPACE_END

#endif  // _$_HX_CORE_M6BITSET
