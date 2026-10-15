/// --- The Kairo Project -------------------------------------------------- ///
///
///   Part of the Kairo Project, under the Apache License v2.0 with the
///   Kairo Runtime Library Exception.
///
///   See: https://www.kairolang.org/LICENSE.txt
///   SPDX-License-Identifier: Apache-2.0 WITH KAIRO-RUNTIME-EXCEPTION
///   Copyright (c) 2026 Dhruvan Kartik
///
/// ------------------------------------------------------------------------ ///

#ifndef _$_HX_CORE_M5SLICE_TPP
#define _$_HX_CORE_M5SLICE_TPP

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/types/types.hh>
#include <include/runtime/runtime.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String {
template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(const CharT *str) noexcept
    : data(str, str ? Traits::length(str) : 0) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(const CharT *str, size_t size) noexcept
    : data(str, size) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(view_t view) noexcept
    : data(view) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(char_vec &vec) noexcept
    : data(vec.data(), vec.size()) {}

/// Narrow `char` input is reinterpreted byte-for-byte, never widened: the
/// slice views the caller's bytes directly, so it borrows `str` like every
/// other slice constructor.
template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
slice<CharT, Traits>::slice(const char *str,
                            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> *) noexcept
    : data(reinterpret_cast<const CharT *>(str),
           str ? LIBCXX_NAMESPACE::char_traits<char>::length(str) : 0) {
    static_assert(sizeof(CharT) == 1, "reinterpreting char input needs a byte-sized CharT");
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
slice<CharT, Traits>::slice(const char *str,
                            usize       size,
                            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> *) noexcept
    : data(reinterpret_cast<const CharT *>(str), str ? size : 0) {
    static_assert(sizeof(CharT) == 1, "reinterpreting char input needs a byte-sized CharT");
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::exchange(slice &other) noexcept {
    view_t tmp     = this->data;
    this->data     = other.data;
    other.data     = tmp;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::replace(slice &other) noexcept {
    this->data = other.data;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::replace(CharT *str, usize size) noexcept {
    this->data = view_t(str, size);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::subslice(usize pos, usize leng) const noexcept {
    return slice_t(data.substr(pos, leng));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::l_strip(const char_vec &delim) const {
    usize start = 0;
    usize end   = data.size();
    while (start < end &&
           libcxx::find(delim.begin(), delim.end(), this->operator[](start)) != delim.end()) {
        ++start;
    }
    return slice_t(subslice(start, end - start));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::r_strip(const char_vec &delim) const {
    usize start = 0;
    usize end   = data.size();
    while (end > start &&
           libcxx::find(delim.begin(), delim.end(), this->operator[](end - 1)) != delim.end()) {
        --end;
    }
    return slice_t(subslice(start, end - start));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::strip(const char_vec &delim) const {
    return l_strip(delim).r_strip(delim);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::starts_with(slice &needle) const {
    return data.size() >= needle.size() && subslice(0, needle.size()) == needle;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::ends_with(slice &needle) const {
    return data.size() >= needle.size() && subslice(data.size() - needle.size(), needle.size()) == needle;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::contains(slice &needle) const {
    return data.find(needle) != view_t::npos;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::contains(CharT &chr) const {
    return data.find(chr) != view_t::npos;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
isize slice<CharT, Traits>::compare(slice &other) const noexcept {
    // Compare only the common prefix; the old form read `size()` elements
    // from `other` even when it was shorter.
    auto n = libcxx::min(data.size(), other.data.size());
    if (auto r = Traits::compare(data.data(), other.data.data(), n); r != 0) {
        return r;
    }
    return data.size() < other.data.size() ? -1 : (data.size() > other.data.size() ? 1 : 0);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice_vec slice<CharT, Traits>::split_lines() const {
    slice_vec result;
    bool      encoding_CR = false;
    usize     start       = 0;
    usize     end         = 0;
    while (end < data.size()) {
        if (this->operator[](end) == '\r') {
            encoding_CR = true;
        } else if (this->operator[](end) == '\n') {
            if (encoding_CR) {
                result.push_back(subslice(start, end - start - 1));
                encoding_CR = false;
            } else {
                result.push_back(subslice(start, end - start));
            }
            start = end + 1;
        }
        ++end;
    }
    if (start < end) {
        result.push_back(subslice(start, end - start));
    }
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice_vec slice<CharT, Traits>::split(slice &delim, Operation op) const {
    slice_vec result;
    usize     start = 0;
    usize     end   = 0;
    while (end < data.size()) {
        if (subslice(end, delim.size()) == delim) {
            if (op == Operation::Keep) {
                result.push_back(subslice(start, end - start));
                result.push_back(delim);
            } else {
                result.push_back(subslice(start, end - start));
            }
            end += delim.size();
            start = end;
        } else {
            ++end;
        }
    }
    if (start < end) {
        result.push_back(subslice(start, end - start));
    }
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::lfind(slice &needle) const {
    usize pos = data.find(needle);
    return pos == view_t::npos ? null : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::rfind(slice &needle) const {
    usize pos = data.rfind(needle);
    return pos == view_t::npos ? null : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_first_of(slice &needle) const {
    usize pos = data.find_first_of(needle);
    return pos == view_t::npos ? null : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_last_of(slice &needle) const {
    usize pos = data.find_last_of(needle);
    return pos == view_t::npos ? null : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_first_not_of(slice &needle) const {
    usize pos = data.find_first_not_of(needle);
    return pos == view_t::npos ? null : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
std::Questionable<usize> slice<CharT, Traits>::find_last_not_of(slice &needle) const {
    usize pos = data.find_last_not_of(needle);
    return (pos == view_t::npos) ? null : std::Questionable<usize>(pos);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
CharT slice<CharT, Traits>::operator[](usize index) const noexcept {
    return data[static_cast<usize>(index)];
}

template class slice<unsigned char>;
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5SLICE