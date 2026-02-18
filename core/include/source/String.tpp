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

#ifndef _$_HX_CORE_M6STRING_TPP
#define _$_HX_CORE_M6STRING_TPP

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String::__internal {
namespace {
    constexpr static usize log2_sizeof_wchar_t_2 = static_cast<usize>(sizeof(wchar_t) > 1) +
                                                   static_cast<usize>(sizeof(wchar_t) > 2) +
                                                   static_cast<usize>(sizeof(wchar_t) > 4);
}
}  // namespace String::__internal

namespace String {
/////////////////////////////// ASSIGNMENT //////////////////////////////////////

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator=(basic &&other) noexcept {
    data = std::Memory::move(other.data);
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator=(const CharT *str) noexcept {
    static constexpr CharT e[] = {0};
    data                       = str ? str : e;
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator=(const slice_t &s) noexcept {
    data.assign(s.raw(), s.size());
    return *this;
}

///////////////////////////// ADDITION ASSIGNMENT //////////////////////////////////////

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const basic &other) noexcept {
    data += other.data;
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const CharT *str) noexcept {
    data += str;
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const slice_t &s) noexcept {
    data.append(s.raw(), s.size());
    return *this;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U> requires std::Meta::is_convertible_to<U, CharT>
inline basic<CharT, Traits> &basic<CharT, Traits>::operator+=(const U chr) noexcept {
    static_assert(std::Meta::is_same_as<U, CharT>,
        "Error: Tried to add a string type to a non-char type.");
    data += chr;
    return *this;
}

///////////////////////////////////// ADDITION /////////////////////////////////////

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const basic &other) const {
    basic result = *this;
    result += other;
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const CharT *str) const {
    basic result = *this;
    result += str;
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const slice_t &s) const {
    basic result = *this;
    result += s;
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U> requires std::Meta::is_convertible_to<U, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::operator+(const U chr) const {
    static_assert(std::Meta::is_same_as<U, CharT>,
        "Error: Tried to add a string type to a non-char type.");
    basic result = *this;
    result += chr;
    return result;
}

////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
inline basic<CharT, Traits>::basic(
    const char *str,
    typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * /* unused */) noexcept {
    if (!str) {
        data = L"";
        return;
    }
    usize size = LIBCXX_NAMESPACE::char_traits<char>::length(str);
    data       = nstring_to_string(nstring(str, size)).data;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
inline basic<CharT, Traits>::basic(
    const char *str,
    usize       size,
    typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * /* unused */) noexcept {
    if (!str || size == 0) {
        data = L"";
        return;
    }
    data = nstring_to_string(nstring(str, size)).data;
}

//////////////////////////////////// SUBSTRING //////////////////////////////////////

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::subslice(size_t pos, size_t len) const noexcept {
    return basic(data.substr(static_cast<size_t>(pos), static_cast<size_t>(len)).c_str());
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::l_strip(const char_vec &delim) const {
    size_t start = 0;
    while (start < size() &&
           libcxx::find(delim.begin(), delim.end(), operator[](start)) != delim.end()) {
        ++start;
    }
    return subslice(start, size() - start);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::r_strip(const char_vec &delim) const {
    size_t end = size();
    while (end > 0 &&
           libcxx::find(delim.begin(), delim.end(), operator[](end - 1)) != delim.end()) {
        --end;
    }
    return subslice(0, end);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline basic<CharT, Traits> basic<CharT, Traits>::strip(const char_vec &delim) const {
    size_t start = 0;
    size_t end   = size();
    while (start < end &&
           libcxx::find(delim.begin(), delim.end(), operator[](start)) != delim.end()) {
        ++start;
    }
    while (end > start &&
           libcxx::find(delim.begin(), delim.end(), operator[](end - 1)) != delim.end()) {
        --end;
    }
    return subslice(start, end - start);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline vec<basic<CharT, Traits>> basic<CharT, Traits>::split(const basic       &delim,
                                                             slice_t::Operation op) const {
    vec<basic> result;
    size_t     start      = 0;
    size_t     end        = 0;
    size_t     delim_size = delim.size();
    while (end <= size()) {
        if (end + delim_size <= size() && subslice(end, delim_size) == delim) {
            if (op == slice_t::Operation::Keep) {
                result.push_back(subslice(start, end - start));
                result.push_back(delim);
            } else {
                result.push_back(subslice(start, end - start));
            }
            start = end + delim_size;
            end   = start;
        } else {
            ++end;
        }
    }
    if (start < size()) {
        result.push_back(subslice(start, size() - start));
    }
    return result;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline vec<basic<CharT, Traits>> basic<CharT, Traits>::split_lines() const {
    vec<basic> result;
    size_t     start = 0;
    size_t     end   = 0;
    while (end < size()) {
        if (operator[](end) == '\n') {
            result.push_back(subslice(start, end - start));
            start = end + 1;
        } else if (operator[](end) == '\r') {
            if (end + 1 < size() && operator[](end + 1) == '\n') {
                result.push_back(subslice(start, end - start));
                start = end + 2;
                end += 1;
            } else {
                result.push_back(subslice(start, end - start));
                start = end + 1;
            }
        }
        ++end;
    }
    if (start < size()) {
        result.push_back(subslice(start, size() - start));
    }
    return result;
}

/////////////////////////////////////// FIND //////////////////////////////////////

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::lfind(slice needle) const {
    return slice_t(data.data(), data.size()).lfind(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::rfind(slice needle) const {
    return slice_t(data.data(), data.size()).rfind(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_first_of(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_last_of(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_not_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_first_not_of(needle);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_not_of(slice needle) const {
    return slice_t(data.data(), data.size()).find_last_not_of(needle);
}

///////////////////////// FIND WITH POSITION /////////////////////////

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::lfind(const basic& needle, usize pos) const {
    // For empty needle, return pos if valid
    if (needle.size() == 0) {
        return pos <= size() ? std::Questionable<usize>(pos) : null;
    }
    
    // Bounds check
    if (pos >= size() || size() < needle.size()) {
        return null;
    }
    
    auto fnd = data.find(needle.raw(), pos, needle.size());
    return fnd != npos ? std::Questionable<usize>(fnd) : null;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::rfind(slice needle, usize pos) const {
    // For empty needle, return min(pos, size())
    if (needle.size() == 0) {
        return std::Questionable<usize>(pos < size() ? pos : size());
    }
    
    // If needle is larger than string or string is empty, no match
    if (size() < needle.size() || size() == 0) {
        return null;
    }
    
    // Adjust pos if it's beyond string length
    if (pos >= size() - needle.size() + 1) {
        pos = size() - needle.size();
    }
    
    // Manual implementation for exact match of a slice
    const CharT* haystack_ptr = data.data();
    const CharT* needle_ptr = needle.raw();
    const usize needle_length = needle.size();
    
    // Search backward from pos
    for (usize i = pos + 1; i > 0; --i) {
        bool found = true;
        for (usize j = 0; j < needle_length; ++j) {
            if (haystack_ptr[i - 1 + j] != needle_ptr[j]) {
                found = false;
                break;
            }
        }
        if (found) {
            return std::Questionable<usize>(i - 1);
        }
    }
    
    return null;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_of(slice needle, usize pos) const {
    // For empty needle or invalid pos, return null
    if (needle.size() == 0 || pos >= size()) {
        return null;
    }
    
    // Manual implementation for matching any character in needle
    const CharT* haystack_ptr = data.data() + pos;
    const CharT* needle_ptr = needle.raw();
    const usize haystack_length = size() - pos;
    const usize needle_length = needle.size();
    
    for (usize i = 0; i < haystack_length; ++i) {
        for (usize j = 0; j < needle_length; ++j) {
            if (haystack_ptr[i] == needle_ptr[j]) {
                return std::Questionable<usize>(pos + i);
            }
        }
    }
    
    return null;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_of(slice needle, usize pos) const {
    // For empty needle or empty string, return null
    if (needle.size() == 0 || size() == 0) {
        return null;
    }
    
    // Adjust pos if it's beyond string length
    if (pos >= size()) {
        pos = size() - 1;
    }
    
    // Manual implementation for matching any character in needle
    const CharT* haystack_ptr = data.data();
    const CharT* needle_ptr = needle.raw();
    const usize needle_length = needle.size();
    
    // Search backward from pos
    for (usize i = pos + 1; i > 0; --i) {
        for (usize j = 0; j < needle_length; ++j) {
            if (haystack_ptr[i - 1] == needle_ptr[j]) {
                return std::Questionable<usize>(i - 1);
            }
        }
    }
    
    return null;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_first_not_of(slice needle, usize pos) const {
    // For empty string or invalid pos, return null
    if (size() == 0 || pos >= size()) {
        return null;
    }
    
    // For empty needle, return first position
    if (needle.size() == 0) {
        return std::Questionable<usize>(pos);
    }
    
    // Manual implementation for finding first character not in needle
    const CharT* haystack_ptr = data.data() + pos;
    const CharT* needle_ptr = needle.raw();
    const usize haystack_length = size() - pos;
    const usize needle_length = needle.size();
    
    for (usize i = 0; i < haystack_length; ++i) {
        bool found = false;
        for (usize j = 0; j < needle_length; ++j) {
            if (haystack_ptr[i] == needle_ptr[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            return std::Questionable<usize>(pos + i);
        }
    }
    
    return null;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
inline std::Questionable<usize> basic<CharT, Traits>::find_last_not_of(slice needle, usize pos) const {
    // For empty string, return null
    if (size() == 0) {
        return null;
    }
    
    // For empty needle, return last valid position
    if (needle.size() == 0) {
        return std::Questionable<usize>(pos < size() ? pos : size() - 1);
    }
    
    // Adjust pos if it's beyond string length
    if (pos >= size()) {
        pos = size() - 1;
    }
    
    // Manual implementation for finding last character not in needle
    const CharT* haystack_ptr = data.data();
    const CharT* needle_ptr = needle.raw();
    const usize needle_length = needle.size();
    
    // Search backward from pos
    for (usize i = pos + 1; i > 0; --i) {
        bool found = false;
        for (usize j = 0; j < needle_length; ++j) {
            if (haystack_ptr[i - 1] == needle_ptr[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            return std::Questionable<usize>(i - 1);
        }
    }
    
    return null;
}

template class basic<char>;
template class basic<wchar_t>;
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M6STRING