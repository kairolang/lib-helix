///--- The Kairo Project ------------------------------------------------------------------------///
///                                                                                              ///
///   part of the kairo project, under the attribution 4.0 international license (cc by 4.0).    ///
///   you are allowed to use, modify, redistribute, and create derivative works, even for        ///
///   commercial purposes, provided that you give appropriate credit, and indicate if changes    ///
///   were made.                                                                                 ///
///                                                                                              ///
///   for more information on the license terms and requirements, please visit:                  ///
///     https://creativecommons.org/licenses/by/4.0/                                             ///
///                                                                                              ///
///   spdx-license-identifier: cc-by-4.0                                                         ///
///   copyright (c) 2024 the kairo project (cc by 4.0)                                           ///
///                                                                                              ///
///-------------------------------------------------------------------------------- lib-helix ---///

#ifndef _$_HX_CORE_M6STRING
#define _$_HX_CORE_M6STRING

#include <include/config/config.hh>

#include <include/meta/meta.hh>
#include <include/types/string/slice.hh>
#include <include/runtime/__memory/memory.hh>
#include <include/types/string/basic.hh>
#include <include/types/string/c-string.hh>
#include <include/types/string/char_traits.hh>
#include <include/meta/type_properties.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

inline string to_string(const char *t) { return (t != nullptr) ? string(t) : string(L""); }
inline string to_string(const wchar_t *t) { return (t != nullptr) ? string(t) : string(L""); }
inline string to_string(const string &t) { return t; }
inline string to_string(const nstring &t) { return {t.raw(), t.size()}; }
inline string to_string(const libcxx::string &t) { return {t.data(), t.size()}; }
inline string to_string(const libcxx::wstring &t) { return {t.data(), t.size()}; }
inline string to_string(bool t) { return t ? L"true" : L"false"; }

template <typename Ty>
    requires std::Interface::SupportsOStream<std::Meta::all_extents_removed<Ty>>
string to_string(Ty &&t) {
    LIBCXX_NAMESPACE::stringstream ss;
    ss << t;
    return {ss.str().data(), ss.str().size()};
}

template <typename Ty>
    requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, wchar_t *>
inline string to_string(Ty &&t) {
    return t.operator$cast(static_cast<wchar_t **>(nullptr));
}

template <typename Ty>
    requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, char *>
inline string to_string(Ty &&t) {
    return t.operator$cast(static_cast<char **>(nullptr));
}

template <typename Ty>
    requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, nstring>
inline string to_string(Ty &&t) {
    return t.operator$cast(static_cast<nstring *>(nullptr));
}

template <typename Ty>
    requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, string>
inline string to_string(Ty &&t) {
    return t.operator$cast(static_cast<string *>(nullptr));
}

template <typename Ty>
    requires(!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, const char *>) &&
            (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, const wchar_t *>) &&
            (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, string>) &&
            (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, nstring>) &&
            (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, libcxx::string>) &&
            (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, libcxx::wstring>) &&
            (!std::Meta::same_as<std::Meta::all_extents_removed<Ty>, bool>) &&
            (!std::Interface::SupportsOStream<std::Meta::all_extents_removed<Ty>>) &&
            (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, wchar_t *>) &&
            (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, char *>) &&
            (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, nstring>) &&
            (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, string>)
inline string to_string(Ty &&t) {
    LIBCXX_NAMESPACE::wstringstream ss;
#ifdef _MSC_VER
    ss << L"[" << typeid(t).name() << L" at " << LIBCXX_NAMESPACE::hex << &t << L"]";
#else
    int   st;
    char *rn = __cxxabiv1::__cxa_demangle(typeid(t).name(), 0, 0, &st);
    ss << L"[" << rn << L" at " << LIBCXX_NAMESPACE::hex << &t << L"]";
    free(rn);
#endif
    return {ss.str().data(), ss.str().size()};
}

// compile-time check for wchar_t size
static_assert(sizeof(wchar_t) >= sizeof(char), "wchar_t must be at least as large as char");


#ifdef _MSC_VER
#pragma warning(push) // remove msvc deprecation warning
#pragma warning(disable : 4996)
#endif

inline char char_to_cchar(wchar_t wc) {
    char* temp = (char*)alloca(MB_CUR_MAX);  // stack-allocated, typically 4-6 bytes

    int len = wctomb(temp, wc);

    if (len <= 0) {
        throw libcxx::range_error("Invalid wchar_t conversion");
    }
    if (len > 1) {
        throw libcxx::range_error("wchar_t converts to multibyte, not single char");
    }

    return temp[0];
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

inline string nstring_to_string(const nstring &cstr) {
    if (cstr.is_empty()) {
        return string();
    }

    #define cast(type) static_cast<type>

    const char *raw_data  = cstr.raw();
    size_t      cstr_size = cstr.size();
    string      result;
    result.resize(cstr_size);
    auto  *buffer  = const_cast<wchar_t *>(result.raw());
    size_t buf_pos = 0;

    for (size_t i = 0; i < cstr_size;) {
        uint32_t      codepoint = 0;
        int           bytes     = 0;
        auto c   = cast(unsigned char)(raw_data[i]);

        if (c <= 0x7F) {
            codepoint = c;
            bytes     = 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < cstr_size) {
            codepoint = ((c & 0x1F) << 6) | (cast(unsigned char)(raw_data[i + 1]) & 0x3F);
            bytes     = 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < cstr_size) {
            codepoint = ((c & 0x0F) << 12) |
                        ((cast(unsigned char)(raw_data[i + 1]) & 0x3F) << 6) |
                        (cast(unsigned char)(raw_data[i + 2]) & 0x3F);
            bytes = 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < cstr_size) {
            codepoint = ((c & 0x07) << 18) |
                        ((cast(unsigned char)(raw_data[i + 1]) & 0x3F) << 12) |
                        (cast(unsigned char)(raw_data[i + 2]) & 0x3F << 6) |
                        (cast(unsigned char)(raw_data[i + 3]) & 0x3F);
            bytes = 4;
        } else {
            codepoint = L'?';
            bytes     = 1;
        }

        if (buf_pos + (sizeof(wchar_t) == 2 && codepoint > 0xFFFF ? 2 : 1) >= result.size()) {
            result.resize(result.size() * 2);
            buffer = const_cast<wchar_t *>(result.raw());
        }

        if constexpr (sizeof(wchar_t) == 2) {
            if (codepoint <= 0xFFFF) {
                buffer[buf_pos++] = static_cast<wchar_t>(codepoint);
            } else if (codepoint <= 0x10FFFF) {
                codepoint -= 0x10000;
                buffer[buf_pos++] = static_cast<wchar_t>(0xD800 | (codepoint >> 10));
                buffer[buf_pos++] = static_cast<wchar_t>(0xDC00 | (codepoint & 0x3FF));
            } else {
                buffer[buf_pos++] = L'?';
            }
        } else {
            buffer[buf_pos++] = static_cast<wchar_t>(codepoint > 0x10FFFF ? L'?' : codepoint);
        }
        i += bytes;
    }
    result.resize(buf_pos);
    return result;
}

inline string nptr_to_string(const char *cstr, size_t size) {
    if (!cstr && size > 0) {
        throw libcxx::invalid_argument("Null pointer with non-zero size");
    }

    if (size == 0) {
        return {};
    }

    size_t max_size = size;
    string result;
    result.resize(max_size);

    auto  *buffer   = const_cast<wchar_t *>(result.raw());
    size_t buf_pos  = 0;
    size_t char_pos = 0;

    while (char_pos < size) {
        wchar_t wc;
        int     len = mbtowc(&wc, cstr + char_pos, size - char_pos);
        if (len <= 0) {
            throw libcxx::range_error("Invalid multibyte sequence at position " +
                                      libcxx::to_string(char_pos));
        }
        if (buf_pos >= max_size) {
            throw libcxx::runtime_error("Internal buffer overflow (unexpected)");
        }
        buffer[buf_pos++] = wc;
        char_pos += len;
    }

    result.resize(buf_pos);
    return result;
}

inline nstring string_to_nstring(const string &wstr) {
    if (wstr.is_empty()) {
        return nstring();
    }

    const wchar_t *raw_data  = wstr.raw();
    size_t         wstr_size = wstr.size();
    nstring        result;
    result.resize(wstr_size * 4);  // Max UTF-8 size (4 bytes per wchar_t)
    char  *buffer  = const_cast<char *>(result.raw());
    size_t buf_pos = 0;

    for (size_t i = 0; i < wstr_size; ++i) {
        uint32_t codepoint;

        // Decode wchar_t (UTF-16 or UTF-32)
        if constexpr (sizeof(wchar_t) == 2) {
            codepoint = static_cast<uint16_t>(raw_data[i]);
            if (codepoint >= 0xD800 && codepoint <= 0xDBFF && i + 1 < wstr_size) {
                uint16_t low = static_cast<uint16_t>(raw_data[i + 1]);
                if (low >= 0xDC00 && low <= 0xDFFF) {
                    codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
                    i++;  // Skip low surrogate
                }
            }
        } else {
            codepoint = static_cast<uint32_t>(raw_data[i]);
        }

        // Encode to UTF-8
        size_t bytes_needed;
        if (codepoint <= 0x7F)
            bytes_needed = 1;
        else if (codepoint <= 0x7FF)
            bytes_needed = 2;
        else if (codepoint <= 0xFFFF)
            bytes_needed = 3;
        else if (codepoint <= 0x10FFFF)
            bytes_needed = 4;
        else
            bytes_needed = 1;  // '?' for invalid

        if (buf_pos + bytes_needed >= result.size()) {
            result.resize(result.size() * 2);
            buffer = const_cast<char *>(result.raw());
        }

        if (codepoint <= 0x7F) {
            buffer[buf_pos++] = static_cast<char>(codepoint);
        } else if (codepoint <= 0x7FF) {
            buffer[buf_pos++] = static_cast<char>(0xC0 | (codepoint >> 6));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0xFFFF) {
            buffer[buf_pos++] = static_cast<char>(0xE0 | (codepoint >> 12));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0x10FFFF) {
            buffer[buf_pos++] = static_cast<char>(0xF0 | (codepoint >> 18));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else {
            buffer[buf_pos++] = '?';
        }
    }

    result.resize(buf_pos);
    return result;
}

using cstring = libcxx::string;

inline string cstring_to_string(const cstring &cstr) {
    if (cstr.empty()) {
        return string();
    }

    const char *raw_data  = cstr.data();
    size_t      cstr_size = cstr.size();
    string      result;
    result.resize(cstr_size);
    auto  *buffer  = const_cast<wchar_t *>(result.raw());
    size_t buf_pos = 0;

    for (size_t i = 0; i < cstr_size;) {
        uint32_t      codepoint = 0;
        int           bytes     = 0;
        auto c   = static_cast<unsigned char>(raw_data[i]);

        if (c <= 0x7F) {
            codepoint = c;
            bytes     = 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < cstr_size) {
            codepoint = ((c & 0x1F) << 6) | (static_cast<unsigned char>(raw_data[i + 1]) & 0x3F);
            bytes     = 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < cstr_size) {
            codepoint = ((c & 0x0F) << 12) |
                        ((static_cast<unsigned char>(raw_data[i + 1]) & 0x3F) << 6) |
                        (static_cast<unsigned char>(raw_data[i + 2]) & 0x3F);
            bytes = 3;
        } else if ((c & 0xF8) == 0xF0 && i + 3 < cstr_size) {
            codepoint = ((c & 0x07) << 18) |
                        ((static_cast<unsigned char>(raw_data[i + 1]) & 0x3F) << 12) |
                        (static_cast<unsigned char>(raw_data[i + 2]) & 0x3F << 6) |
                        (static_cast<unsigned char>(raw_data[i + 3]) & 0x3F);
            bytes = 4;
        } else {
            codepoint = L'?';
            bytes     = 1;
        }

        if (buf_pos + (sizeof(wchar_t) == 2 && codepoint > 0xFFFF ? 2 : 1) >= result.size()) {
            result.resize(result.size() * 2);
            buffer = const_cast<wchar_t *>(result.raw());
        }

        if constexpr (sizeof(wchar_t) == 2) {
            if (codepoint <= 0xFFFF) {
                buffer[buf_pos++] = static_cast<wchar_t>(codepoint);
            } else if (codepoint <= 0x10FFFF) {
                codepoint -= 0x10000;
                buffer[buf_pos++] = static_cast<wchar_t>(0xD800 | (codepoint >> 10));
                buffer[buf_pos++] = static_cast<wchar_t>(0xDC00 | (codepoint & 0x3FF));
            } else {
                buffer[buf_pos++] = L'?';
            }
        } else {
            buffer[buf_pos++] = static_cast<wchar_t>(codepoint > 0x10FFFF ? L'?' : codepoint);
        }
        i += bytes;
    }
    result.resize(buf_pos);
    return result;
}

inline cstring string_to_cstring(const string &wstr) {
    if (wstr.is_empty()) {
        return cstring();
    }

    const wchar_t *raw_data  = wstr.raw();
    size_t         wstr_size = wstr.size();
    cstring        result;
    result.resize(wstr_size * 4);  // Max UTF-8 size (4 bytes per wchar_t)
    char  *buffer  = const_cast<char *>(result.data());
    size_t buf_pos = 0;

    for (size_t i = 0; i < wstr_size; ++i) {
        uint32_t codepoint;

        // Decode wchar_t (UTF-16 or UTF-32)
        if constexpr (sizeof(wchar_t) == 2) {
            codepoint = static_cast<uint16_t>(raw_data[i]);
            if (codepoint >= 0xD800 && codepoint <= 0xDBFF && i + 1 < wstr_size) {
                uint16_t low = static_cast<uint16_t>(raw_data[i + 1]);
                if (low >= 0xDC00 && low <= 0xDFFF) {
                    codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
                    i++;  // Skip low surrogate
                }
            }
        } else {
            codepoint = static_cast<uint32_t>(raw_data[i]);
        }

        // Encode to UTF-8
        size_t bytes_needed;
        if (codepoint <= 0x7F)
            bytes_needed = 1;
        else if (codepoint <= 0x7FF)
            bytes_needed = 2;
        else if (codepoint <= 0xFFFF)
            bytes_needed = 3;
        else if (codepoint <= 0x10FFFF)
            bytes_needed = 4;
        else
            bytes_needed = 1;  // '?' for invalid

        if (buf_pos + bytes_needed >= result.size()) {
            result.resize(result.size() * 2);
            buffer = const_cast<char *>(result.data());
        }

        if (codepoint <= 0x7F) {
            buffer[buf_pos++] = static_cast<char>(codepoint);
        } else if (codepoint <= 0x7FF) {
            buffer[buf_pos++] = static_cast<char>(0xC0 | (codepoint >> 6));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0xFFFF) {
            buffer[buf_pos++] = static_cast<char>(0xE0 | (codepoint >> 12));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0x10FFFF) {
            buffer[buf_pos++] = static_cast<char>(0xF0 | (codepoint >> 18));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else {
            buffer[buf_pos++] = '?';
        }
    }

    result.resize(buf_pos);
    return result;
}

inline void wptr_to_nptr(const wchar_t *wstr, char *buffer, size_t buffer_size) {
    if (!wstr || !buffer) {
        throw libcxx::invalid_argument("Null pointer provided");
    }
    if (buffer_size == 0) {
        throw libcxx::invalid_argument("Buffer size must be at least 1 for null terminator");
    }

    size_t buf_pos = 0;
    size_t i       = 0;

    while (wstr[i] != L'\0') {
        uint32_t codepoint;

        // Decode wchar_t (UTF-16 or UTF-32)
        if constexpr (sizeof(wchar_t) == 2) {
            codepoint = static_cast<uint16_t>(wstr[i]);
            if (codepoint >= 0xD800 && codepoint <= 0xDBFF && wstr[i + 1] != L'\0') {
                uint16_t low = static_cast<uint16_t>(wstr[i + 1]);
                if (low >= 0xDC00 && low <= 0xDFFF) {
                    codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
                    i++;  // Skip the low surrogate
                }
            }
        } else {
            codepoint = static_cast<uint32_t>(wstr[i]);
        }

        // Encode to UTF-8
        if (codepoint <= 0x7F) {
            if (buf_pos + 1 >= buffer_size) {
                buffer[buf_pos] = '\0';
                throw libcxx::range_error("Buffer too small at index " + libcxx::to_string(i));
            }
            buffer[buf_pos++] = static_cast<char>(codepoint);
        } else if (codepoint <= 0x7FF) {
            if (buf_pos + 2 >= buffer_size) {
                buffer[buf_pos] = '\0';
                throw libcxx::range_error("Buffer too small at index " + libcxx::to_string(i));
            }
            buffer[buf_pos++] = static_cast<char>(0xC0 | (codepoint >> 6));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0xFFFF) {
            if (buf_pos + 3 >= buffer_size) {
                buffer[buf_pos] = '\0';
                throw libcxx::range_error("Buffer too small at index " + libcxx::to_string(i));
            }
            buffer[buf_pos++] = static_cast<char>(0xE0 | (codepoint >> 12));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else if (codepoint <= 0x10FFFF) {
            if (buf_pos + 4 >= buffer_size) {
                buffer[buf_pos] = '\0';
                throw libcxx::range_error("Buffer too small at index " + libcxx::to_string(i));
            }
            buffer[buf_pos++] = static_cast<char>(0xF0 | (codepoint >> 18));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
            buffer[buf_pos++] = static_cast<char>(0x80 | (codepoint & 0x3F));
        } else {
            // Invalid codepoint: use '?'
            if (buf_pos + 1 >= buffer_size) {
                buffer[buf_pos] = '\0';
                throw libcxx::range_error("Buffer too small at index " + libcxx::to_string(i));
            }
            buffer[buf_pos++] = '?';
        }
        i++;
    }
    buffer[buf_pos] = '\0';
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_hx_core_m6string
