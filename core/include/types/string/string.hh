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

#ifndef _$_HX_CORE_M6STRING
#define _$_HX_CORE_M6STRING

#include <include/config/config.hh>

#include <include/meta/meta.hh>
#include <include/types/string/slice.hh>
#include <include/runtime/__memory/memory.hh>
#include <include/types/string/basic.hh>
#include <include/types/string/char_traits.hh>
#include <include/meta/type_properties.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

inline string to_string(const char *t) { return (t != nullptr) ? string(t) : string(); }
inline string to_string(const string &t) { return t; }
inline string to_string(const libcxx::string &t) { return {t.data(), t.size()}; }
inline string to_string(bool t) { return t ? string("true") : string("false"); }

template <typename Ty>
    requires std::Interface::SupportsOStream<std::Meta::all_extents_removed<Ty>>
string to_string(Ty &&t) {
    LIBCXX_NAMESPACE::stringstream ss;
    ss << t;
    auto str = ss.str();
    return {str.data(), str.size()};
}

template <typename Ty>
    requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, char *>
inline string to_string(Ty &&t) {
    return t.operator$cast(static_cast<char **>(nullptr));
}

template <typename Ty>
    requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, string>
inline string to_string(Ty &&t) {
    return t.operator$cast(static_cast<string *>(nullptr));
}

template <typename Ty>
    requires(!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, const char *>) &&
            (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, string>) &&
            (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, libcxx::string>) &&
            (!std::Meta::same_as<std::Meta::all_extents_removed<Ty>, bool>) &&
            (!std::Interface::SupportsOStream<std::Meta::all_extents_removed<Ty>>) &&
            (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, char *>) &&
            (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, string>)
inline string to_string(Ty &&t) {
    LIBCXX_NAMESPACE::stringstream ss;
#ifdef _MSC_VER
    ss << "[" << typeid(t).name() << " at " << LIBCXX_NAMESPACE::hex << &t << "]";
#else
    int   st;
    char *rn = __cxxabiv1::__cxa_demangle(typeid(t).name(), 0, 0, &st);
    ss << "[" << rn << " at " << LIBCXX_NAMESPACE::hex << &t << "]";
    free(rn);
#endif
    auto str = ss.str();
    return {str.data(), str.size()};
}

using cstring = libcxx::string;

// `string` is a byte string now, so every narrow/wide conversion below is the
// identity (or a byte copy). They stay only so existing callers keep
// compiling; the deprecation points each one at its removal.

[[deprecated("narrow string: identity")]]
inline string nstring_to_string(const nstring &s) { return s; }

[[deprecated("narrow string: identity")]]
inline nstring string_to_nstring(const string &s) { return s; }

[[deprecated("narrow string: identity")]]
inline string nptr_to_string(const char *p, size_t n) { return string(p, n); }

[[deprecated("narrow string: identity")]]
inline string cstring_to_string(const cstring &s) { return string(s.data(), s.size()); }

[[deprecated("narrow string: identity")]]
inline cstring string_to_cstring(const string &s) { return cstring(s.c_str(), s.size()); }

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_hx_core_m6string
