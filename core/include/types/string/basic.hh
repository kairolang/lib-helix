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

#ifndef _$_HX_CORE_M5BASIC
#define _$_HX_CORE_M5BASIC

#include <include/config/config.hh>
#include <include/meta/__interfaces/casting.hh>
#include <include/runtime/__memory/forwarding.hh>
#include <include/types/string/char_traits.hh>
#include <include/types/string/slice.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {
template <typename CharT, typename Traits /* defaulted in casting.hh */>
    requires CharTraits<Traits, CharT>
class basic {
  public:
    struct slice : public String::slice<CharT, Traits> {
        using String::slice<CharT, Traits>::slice;
    };

    using char_traits = Traits;
    using char_t      = CharT;
    using size_t      = usize;
    using string_t    = libcxx::basic_string<CharT, Traits>;
    using slice_t     = slice;
    using slice_vec   = vec<slice_t>;
    using char_vec    = vec<CharT>;

  private:
    string_t data;

  public:
    static constexpr size_t npos = string_t::npos;

    // Constructors
    constexpr basic() noexcept
        : data() {}
    constexpr basic(const basic &other) noexcept
        : data(other.data) {}
    constexpr basic(basic &&other) noexcept
        : data(std::Memory::move(other.data)) {}
    constexpr basic(const libcxx::basic_string<CharT, Traits> &str) noexcept
        : data(str) {}
    constexpr basic(const CharT *str) noexcept {
        static constexpr CharT e[] = {0};
        data                       = str ? str : e;
    }
    constexpr basic(const CharT chr, size_t count) noexcept
        : data(count, chr) {}
    constexpr basic(const CharT *str, size_t len) noexcept
        : data(str, len) {}
    constexpr basic(const slice_t &s) noexcept
        : data(s.raw(), s.size()) {}

    template <typename U = CharT>
    basic(const char *str,
          typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

    template <typename U = CharT>
    basic(const char *str,
          usize       size,
          typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

    // Assignment
    basic &operator=(const basic &other) noexcept = default;
    basic &operator=(basic &&other) noexcept;
    basic &operator=(const CharT *str) noexcept;
    basic &operator=(const slice_t &s) noexcept;
    basic &operator=(const string_t &str) noexcept {
        data = str;
        return *this;
    }

    // Access Operators
    CharT       &operator[](size_t index) noexcept { return data[static_cast<size_t>(index)]; }
    const CharT &operator[](size_t index) const noexcept {
        return data[index];
    }

    // Mutable Methods
    void push_back(CharT c) noexcept { data.push_back(c); }
    void append(const basic &other) noexcept { data.append(other.data); }
    void append(const CharT *str, size_t len) noexcept { data.append(str, len); }
    void append(const slice_t &s) noexcept { data.append(s.raw(), s.size()); }
    void clear() noexcept { data.clear(); }
    void replace(size_t pos, size_t len, const slice_t &other) noexcept {
        data.replace(static_cast<size_t>(pos),
                     static_cast<size_t>(len),
                     libcxx::basic_string_view<CharT>(other));
    }

    basic &replace(const basic &old_str, const basic &new_str, size_t count = npos) noexcept {
        if (old_str.empty()) {
            return *this;
        }

        string_t result;
        size_t   pos          = 0;
        size_t   replacements = 0;

        while (pos < data.size() && (count == npos || replacements < count)) {
            size_t next = data.find(old_str.data, pos);

            if (next == npos) {
                result.append(data, pos, data.size() - pos);
                break;
            }

            result.append(data, pos, next - pos);
            result.append(new_str.data);
            pos = next + old_str.size();
            replacements++;
        }

        if (pos < data.size() && (count == npos || replacements < count)) {
            result.append(data, pos, data.size() - pos);
        }

        data = std::Memory::move(result);
        return *this;
    }

    basic &replace(const slice_t &old_str, const slice_t &new_str, size_t count = npos) noexcept {
        return replace(basic(old_str), basic(new_str), count);
    }

    basic &replace(const CharT *old_str, const CharT *new_str, size_t count = npos) noexcept {
        return replace(basic(old_str), basic(new_str), count);
    }

    basic &replace(CharT old_char, CharT new_char, size_t count = npos) noexcept {
        size_t replacements = 0;

        for (size_t i = 0; i < data.size() && (count == npos || replacements < count); ++i) {
            if (data[i] == old_char) {
                data[i] = new_char;
                replacements++;
            }
        }

        return *this;
    }

    void reserve(size_t new_cap) noexcept { data.reserve(static_cast<size_t>(new_cap)); }
    void resize(size_t new_size, CharT c = CharT()) noexcept {
        data.resize(static_cast<size_t>(new_size), c);
    }

    bool empty() const noexcept { return data.empty(); }

    // Concatenation Operators
    basic &operator+=(const basic &other) noexcept;
    basic &operator+=(const CharT *str) noexcept;
    basic &operator+=(const slice_t &s) noexcept;

    template <typename U = CharT>
        requires std::Meta::is_convertible_to<U, CharT>
    basic &operator+=(const U chr) noexcept;

    basic operator+(const basic &other) const;
    basic operator+(const CharT *str) const;
    basic operator+(const slice_t &s) const;

    // implicit conversion to string_t
    operator string_t() const noexcept { return data; }

    template <typename U = CharT>
        requires std::Meta::is_convertible_to<U, CharT>
    basic operator+(const U chr) const;

    // Comparison Operators
    bool operator==(const basic &other) const noexcept { return data == other.data; }
    bool operator!=(const basic &other) const noexcept { return data != other.data; }
    bool operator<(const basic &other) const noexcept { return data < other.data; }
    bool operator>(const basic &other) const noexcept { return data > other.data; }
    bool operator<=(const basic &other) const noexcept { return data <= other.data; }
    bool operator>=(const basic &other) const noexcept { return data >= other.data; }

    // Basic Access
    const CharT    *raw() const noexcept { return data.c_str(); }
    size_t          size() const noexcept { return data.size(); }
    size_t          length() const noexcept { return data.length(); }
    bool            is_empty() const noexcept { return data.empty(); }
    string_t       &raw_string() noexcept { return data; }
    const string_t &raw_string() const noexcept { return data; }

    // Slice Conversion
    operator slice_t() const noexcept { return slice_t(data.data(), data.size()); }
    slice_t operator$cast(const slice_t * /* p */) const noexcept {
        return slice_t(data.data(), data.size());
    }
    const char_t *operator$cast(const char_t * /* p */) const noexcept { return data.data(); }

    // Copy-Returning Methods
    basic      subslice(size_t pos, size_t len) const noexcept;
    basic      l_strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    basic      r_strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    basic      strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    vec<basic> split(const basic &delim, slice_t::Operation op = slice_t::Operation::Remove) const;
    vec<basic> split_lines() const;

    // Search
    bool starts_with(const basic &needle) const noexcept { return data.starts_with(needle.data); }
    bool starts_with(CharT c) const noexcept { return data.starts_with(c); }
    bool starts_with(slice needle) const noexcept { return data.starts_with(needle.raw()); }

    bool ends_with(const basic &needle) const noexcept { return data.ends_with(needle.data); }
    bool ends_with(CharT c) const noexcept { return data.ends_with(c); }
    bool ends_with(slice needle) const noexcept { return data.ends_with(needle.raw()); }

    bool contains(const basic &needle) const noexcept { return data.find(needle.data) != npos; }
    bool contains(CharT c) const noexcept { return data.find(c) != npos; }

    constexpr bool operator$contains(slice needle) const { return contains(needle); }
    constexpr bool operator$contains(CharT chr) const { return contains(chr); }

    // add iterators for the string
    using iterator               = typename string_t::iterator;
    using const_iterator         = typename string_t::const_iterator;
    using reverse_iterator       = typename string_t::reverse_iterator;
    using const_reverse_iterator = typename string_t::const_reverse_iterator;

    iterator begin() noexcept { return data.begin(); }
    iterator end() noexcept { return data.end(); }

    const_iterator begin() const noexcept { return data.begin(); }
    const_iterator end() const noexcept { return data.end(); }
    const_iterator cbegin() const noexcept { return data.cbegin(); }
    const_iterator cend() const noexcept { return data.cend(); }

    reverse_iterator rbegin() noexcept { return data.rbegin(); }
    reverse_iterator rend() noexcept { return data.rend(); }

    const_reverse_iterator rbegin() const noexcept { return data.rbegin(); }
    const_reverse_iterator rend() const noexcept { return data.rend(); }
    const_reverse_iterator crbegin() const noexcept { return data.crbegin(); }
    const_reverse_iterator crend() const noexcept { return data.crend(); }

    std::Questionable<usize> lfind(slice needle) const;
    std::Questionable<usize> rfind(slice needle) const;
    std::Questionable<usize> find_first_of(slice needle) const;
    std::Questionable<usize> find_last_of(slice needle) const;
    std::Questionable<usize> find_first_not_of(slice needle) const;
    std::Questionable<usize> find_last_not_of(slice needle) const;

    std::Questionable<usize> lfind(const basic &needle, usize pos) const;
    std::Questionable<usize> rfind(slice needle, usize pos) const;
    std::Questionable<usize> find_first_of(slice needle, usize pos) const;
    std::Questionable<usize> find_last_of(slice needle, usize pos) const;
    std::Questionable<usize> find_first_not_of(slice needle, usize pos) const;
    std::Questionable<usize> find_last_not_of(slice needle, usize pos) const;
};
}  // namespace String

H_STD_NAMESPACE_END

using nstring  = std::String::basic<char>;
using string   = std::String::basic<wchar_t>;
using string32 = std::String::basic<char32_t>;

H_NAMESPACE_END

namespace std {
template <>
struct hash<kairo::string> {
    size_t operator()(const kairo::string &s) const noexcept {
        return std::hash<std::wstring>{}(std::wstring(s.raw(), s.size()));
    }
};

template <>
struct hash<kairo::nstring> {
    size_t operator()(const kairo::nstring &s) const noexcept {
        return std::hash<std::string>{}(std::string(s.raw(), s.size()));
    }
};

template <>
struct hash<kairo::string::slice> {
    size_t operator()(const kairo::string::slice &s) const noexcept {
        return std::hash<std::wstring>{}(std::wstring(s.raw(), s.size()));
    }
};

template <>
struct hash<kairo::nstring::slice> {
    size_t operator()(const kairo::nstring::slice &s) const noexcept {
        return std::hash<std::string>{}(std::string(s.raw(), s.size()));
    }
};
}  // namespace std

#endif  // _$_HX_CORE_M5BASIC
