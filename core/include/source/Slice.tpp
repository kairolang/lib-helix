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

#ifndef _$_HX_CORE_M5SLICE_TPP
#define _$_HX_CORE_M5SLICE_TPP

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/types/types.hh>
#include <include/runtime/runtime.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace String::__internal {
namespace {
    constexpr static usize log2_sizeof_wchar_t = static_cast<usize>(sizeof(wchar_t) > 1) +
                                                 static_cast<usize>(sizeof(wchar_t) > 2) +
                                                 static_cast<usize>(sizeof(wchar_t) > 4);
}
}  // namespace String::__internal

namespace String {
template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(const CharT *str) noexcept
    : len(str ? Traits::length(str) : 0)
    , data(str, len) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(const CharT *str, size_t size) noexcept
    : len(size)
    , data(str, len) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(view_t view) noexcept
    : len(view.size())
    , data(view) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(char_vec &vec) noexcept
    : len(vec.size())
    , data(vec.data(), len) {}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice(char_vec &&vec) noexcept
    : len(vec.size())
    , data(std::Memory::move(vec).data(), len) {}

/// NOTE: The code bellow needs to be reviewed to ensure that it is correct and safe.
/// I'm not sure if i should be using CharT or wchar_t here.
/// if further testing shows that this is not correct, need to change it to use CharT.
template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
slice<CharT, Traits>::slice(const char *str,
                            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> *) noexcept {
    usize    size = LIBCXX_NAMESPACE::char_traits<char>::length(str);
    wchar_t *buff =
        static_cast<wchar_t *>(alloca((size + 1) << String::__internal::log2_sizeof_wchar_t));

    try {
        for (usize i = 0; i < size; ++i) {
            buff[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
        }
        this->replace(buff, size);
    } catch (...) { this->replace((wchar_t *)nullptr, 1_usize); }
}

/// Same as above, also needs to be reviewed.
/// NOTE: The code bellow needs to be reviewed to ensure that it is correct and safe.
template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
template <typename U>
slice<CharT, Traits>::slice(const char *str,
                            usize       size,
                            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> *) noexcept {
    wchar_t *buff =
        static_cast<wchar_t *>(alloca((size) << String::__internal::log2_sizeof_wchar_t));

    try {
        for (usize i = 0; i < size; ++i) {
            buff[i] = static_cast<wchar_t>(static_cast<unsigned char>(str[i]));
        }
        this->replace(buff, size);
    } catch (...) { this->replace((wchar_t *)nullptr, 1_usize); }
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::exchange(slice &other) noexcept {
    view_t tmp     = this->data;
    this->data     = other.data;
    other.data     = tmp;
    size_t tmp_len = this->len;
    this->len   = other.len;
    other.len   = tmp_len;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::replace(slice &other) noexcept {
    this->data   = other.data;
    this->len = other.len;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
void slice<CharT, Traits>::replace(CharT *str, usize size) noexcept {
    this->data   = view_t(str, size);
    this->len = size;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::subslice(usize pos, usize leng) const noexcept {
    return slice_t(data.substr(pos, leng));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::l_strip(char_vec &delim) const {
    usize start = 0;
    usize end   = len;
    while (start < end &&
           libcxx::find(delim.begin(), delim.end(), this->operator[](start)) != delim.end()) {
        ++start;
    }
    return slice_t(subslice(start, end - start));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::r_strip(char_vec &delim) const {
    usize start = 0;
    usize end   = len;
    while (end > start &&
           libcxx::find(delim.begin(), delim.end(), this->operator[](end - 1)) != delim.end()) {
        --end;
    }
    return slice_t(subslice(start, end - start));
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits> slice<CharT, Traits>::strip(char_vec &delim) const {
    return l_strip(delim).r_strip(delim);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::starts_with(slice &needle) const {
    return len >= needle.size() && subslice(0, needle.size()) == needle;
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
bool slice<CharT, Traits>::ends_with(slice &needle) const {
    return len >= needle.size() && subslice(len - needle.size(), needle.size()) == needle;
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
    return Traits::compare(data.data(), other.data.data(), len);
}

template <typename CharT, typename Traits>
    requires CharTraits<Traits, CharT>
slice<CharT, Traits>::slice_vec slice<CharT, Traits>::split_lines() const {
    slice_vec result;
    bool      encoding_CR = false;
    usize     start       = 0;
    usize     end         = 0;
    while (end < len) {
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
    while (end < len) {
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

template class slice<char>;
template class slice<wchar_t>;
}  // namespace String

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M5SLICE