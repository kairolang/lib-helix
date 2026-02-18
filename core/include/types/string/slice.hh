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

#ifndef _$_HX_CORE_M5SLICE
#define _$_HX_CORE_M5SLICE

#include <include/config/config.hh>

#include <include/types/builtins/primitives.hh>
#include <include/types/builtins/size_t.hh>
#include <include/types/question/question_fwd.hh>
#include <include/types/string/char_traits.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {

template <typename CharT, typename Traits = libcxx::char_traits<CharT>>
    requires CharTraits<Traits, CharT>
class slice {
    using view_t = libcxx::basic_string_view<CharT>;

    usize  len;
    view_t data{};

  public:
    enum class Operation { Keep, Remove };

    using char_traits = Traits;
    using char_t      = CharT;
    using slice_t     = slice<CharT>;
    using size_t      = usize;
    using slice_vec   = vec<slice_t>;
    using char_vec    = vec<CharT>;

    [[nodiscard("size() returns the length of the slice's data, which is essential for bounds "
                "checking and iteration; discarding it may lead to unsafe or undefined behavior")]]
    constexpr size_t size() const noexcept {
        return data.size();
    }

    constexpr slice() noexcept                   = default;
    constexpr slice(const slice &other) noexcept = default;
    constexpr slice(slice &&other) noexcept      = default;
    slice(const CharT *str) noexcept;
    slice(const CharT *str, size_t size) noexcept;
    slice(view_t view) noexcept;
    slice(char_vec &vec) noexcept;
    slice(char_vec &&vec) noexcept;

    template <typename U = CharT>
    slice(const char *str,
          typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

    template <typename U = CharT>
    slice(const char *str,
          usize       size,
          typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

    constexpr operator view_t() const noexcept { return data; }

    void exchange(slice &other) noexcept;
    void replace(slice &other) noexcept;
    void replace(CharT *str, usize size) noexcept;

    [[nodiscard("raw() returns a pointer to the slice's underlying data, essential for direct "
                "access; ignoring it may discard critical information")]]
    constexpr const CharT *raw() const noexcept {
        return data.data();
    }
    [[nodiscard("is_empty() indicates whether the slice has no data, crucial for control flow; "
                "discarding it may lead to incorrect assumptions")]]
    constexpr bool is_empty() const noexcept {
        return len == 0;
    }
    [[nodiscard("subslice() creates a view into a portion of the slice, vital for safe substring "
                "operations; ignoring it wastes the result")]]
    slice subslice(usize pos, usize len) const noexcept;

    slice l_strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    slice r_strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    slice strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;

    usize length() const noexcept { return len; }

    bool starts_with(slice &needle) const;
    bool ends_with(slice &needle) const;

    bool contains(slice &needle) const;
    bool contains(CharT &chr) const;

    bool operator==(const slice &other) const noexcept { return data == other.data; }
    bool operator!=(const slice &other) const noexcept { return data != other.data; }
    bool operator<(const slice &other) const noexcept { return data < other.data; }
    bool operator>(const slice &other) const noexcept { return data > other.data; }
    bool operator<=(const slice &other) const noexcept { return data <= other.data; }
    bool operator>=(const slice &other) const noexcept { return data >= other.data; }

    constexpr bool operator$contains(slice &needle) const { return contains(needle); }
    constexpr bool operator$contains(CharT &chr) const { return contains(chr); }

    isize compare(slice &other) const noexcept;

    [[nodiscard("split_lines() returns a vector of line views, necessary for line-based "
                "processing; discarding it neglects the parsed structure")]]
    slice_vec split_lines() const;

    [[nodiscard("split() returns a vector of views, necessary for delimeter-based processing; "
                "discarding it neglects the parsed structure")]]
    slice_vec split(slice &delim, Operation op = Operation::Remove) const;

    std::Questionable<usize> lfind(slice &needle) const;
    std::Questionable<usize> rfind(slice &needle) const;
    std::Questionable<usize> find_first_of(slice &needle) const;
    std::Questionable<usize> find_last_of(slice &needle) const;
    std::Questionable<usize> find_first_not_of(slice &needle) const;
    std::Questionable<usize> find_last_not_of(slice &needle) const;

    CharT operator[](usize index) const noexcept;
};
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

// make hash able
namespace std {
template <>
struct hash<kairo::std::String::slice<wchar_t>> {
    size_t operator()(const kairo::std::String::slice<wchar_t> &s) const noexcept {
        return std::hash<std::wstring>{}(std::wstring(s.raw(), s.size()));
    }
};

template <>
struct hash<kairo::std::String::slice<char>> {
    size_t operator()(const kairo::std::String::slice<char> &s) const noexcept {
        return std::hash<std::string>{}(std::string(s.raw(), s.size()));
    }
};
}  // namespace std

#endif  // _$_HX_CORE_M5SLICE
