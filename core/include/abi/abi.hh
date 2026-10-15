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

#ifndef _$_HX_CORE_M3ABI
#define _$_HX_CORE_M3ABI

#include <include/c++/libc++.hh>
#include <include/config/config.hh>
#include <include/runtime/__io/__print/print.hh>
#include <include/runtime/__io/__print/stringf.hh>
#include <include/types/types.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace ABI {

enum class ObjectType : u8 { Module, Class, Struct, Function, Operator, Reserved, Internal, None };

inline string mangle(const string &input, ObjectType type) {
    if (input.empty()) {
        throw libcxx::invalid_argument("Input string cannot be empty");
    }

    string prefix;
    switch (type) {
        case ObjectType::Module:
            prefix = "_$M_";
            break;
        case ObjectType::Class:
            prefix = "_$C_";
            break;
        case ObjectType::Struct:
            prefix = "_$S_";
            break;
        case ObjectType::Function:
            prefix = "_$F_";
            break;
        case ObjectType::Operator:
            prefix = "_$O_";
            break;
        case ObjectType::Reserved:
            prefix = "_$R_";
            break;
        case ObjectType::Internal:
            prefix = "_$I_";
            break;
        default:
            break;
    }

    string output = prefix;

    for (auto ch : input) {
        if (libcxx::isalnum(static_cast<unsigned char>(ch)) || ch == '_') {
            output += ch;
        } else {
            // Encode as $XXXX (4 hex digits); `ch` is a byte, so the top two
            // are always 00 - kept for compatibility with `demangle`.
            char hex[6];
            libcxx::snprintf(hex, 6, "$%04X", static_cast<unsigned int>(ch));
            output += hex;
        }
    }

    // Append length marker. `demangle` checks the decoded length against it,
    // so this must be the length, not the input itself.
    output += "$L";
    output += libcxx::to_string(input.length()).c_str();
    output += "_E$";

    return output;
}

inline ObjectType is_mangled(const string &input) {
    if (input.length() < 4 || input[0] != '_' || input[1] != '$') {
        return ObjectType::None;
    }

    string prefix = input.subslice(0, 4);
    if (prefix == "_$M_")
        return ObjectType::Module;
    if (prefix == "_$C_")
        return ObjectType::Class;
    if (prefix == "_$S_")
        return ObjectType::Struct;
    if (prefix == "_$F_")
        return ObjectType::Function;
    if (prefix == "_$O_")
        return ObjectType::Operator;
    if (prefix == "_$R_")
        return ObjectType::Reserved;
    if (prefix == "_$I_")
        return ObjectType::Internal;

    return ObjectType::None;
}

inline bool is_hex_digit(_H_RESERVED$char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

inline string basename_no_ext(const string &path) {
    std::Questionable<usize> slash  = path.rfind("/");
    std::Questionable<usize> bslash = path.rfind("\\");
    std::Questionable<usize> sep    = slash;

    if (bslash != null && (sep == null || (*bslash) > (*sep))) {
        sep = *bslash;
    }

    usize                    start = (sep == null) ? 0 : (*sep) + 1;  // PROBLEM
    std::Questionable<usize> dot   = path.rfind(".");

    usize end = (dot != null && (*dot) > start) ? (*dot) : static_cast<usize>(path.length());

    return path.subslice(start, end - start);
}

// --- FIXED: supports $XX and $XXXX
inline string demangle(const string &input, ObjectType type) {
    string expected_prefix;
    switch (type) {
        case ObjectType::Module:
            expected_prefix = "_$M_";
            break;
        case ObjectType::Class:
            expected_prefix = "_$C_";
            break;
        case ObjectType::Struct:
            expected_prefix = "_$S_";
            break;
        case ObjectType::Function:
            expected_prefix = "_$F_";
            break;
        case ObjectType::Operator:
            expected_prefix = "_$O_";
            break;
        case ObjectType::Reserved:
            expected_prefix = "_$R_";
            break;
        case ObjectType::Internal:
            expected_prefix = "_$I_";
            break;
        default:
            break;
    }

    if (!input.starts_with(expected_prefix)) {
        throw libcxx::invalid_argument("Invalid mangled name or type mismatch");
    }

    usize len_pos = input.view().rfind("$L");
    if (len_pos == libcxx::string_view::npos) {
        throw libcxx::invalid_argument("Invalid mangled name: missing length");
    }
    usize end_pos = input.view().rfind("_E$");
    if (end_pos == libcxx::string_view::npos || end_pos <= len_pos) {
        throw libcxx::invalid_argument("Invalid mangled name: missing end marker");
    }

    string len_str = input.subslice(len_pos + 2, end_pos - len_pos - 2);
    if (len_str.empty()) {
        throw libcxx::invalid_argument("Invalid length in mangled name");
    }

    usize expected_len = 0;
    try {
        expected_len = libcxx::stoul(libcxx::string(len_str.view()));
    } catch (...) { throw libcxx::invalid_argument("Invalid length in mangled name"); }

    string encoded = input.subslice(expected_prefix.length(), len_pos - expected_prefix.length());
    string output;

    for (usize i = 0; i < encoded.length();) {
        if (encoded[i] == '$') {
            usize j     = i + 1;
            usize count = 0;
            while (j < encoded.length() && count < 4 && is_hex_digit(encoded[j])) {
                ++j;
                ++count;
            }

            if (count == 4 || count == 2) {
                string hx = encoded.subslice(i + 1, count);
                try {
                    unsigned long code = libcxx::stoul(libcxx::string(hx.view()), nullptr, 16);
                    output += static_cast<_H_RESERVED$char>(code);
                    i = j;

                    continue;
                } catch (...) { ; }
            }
        }

        output += encoded[i];
        ++i;
    }

    if (output.length() != expected_len) {
        throw libcxx::invalid_argument("Demangled length mismatch");
    }

    return output;
}

// --- FIXED: replaces mangled segments; for Modules returns basename w/o extension
inline string demangle_partial(const string &input) {
    string output;
    for (usize i = 0; i < input.length(); ++i) {
        if (i + 3 < input.length() && input[i] == '_' && input[i + 1] == '$') {
            ObjectType ty = ObjectType::None;
            switch (input[i + 2]) {
                case 'M':
                    ty = ObjectType::Module;
                    break;
                case 'C':
                    ty = ObjectType::Class;
                    break;
                case 'S':
                    ty = ObjectType::Struct;
                    break;
                case 'F':
                    ty = ObjectType::Function;
                    break;
                case 'O':
                    ty = ObjectType::Operator;
                    break;
                case 'R':
                    ty = ObjectType::Reserved;
                    break;
                case 'I':
                    ty = ObjectType::Internal;
                    break;
                default:
                    output += input[i];
                    continue;
            }

            usize end_pos = input.raw_string().find(string("_E$").raw_string(), i);
            if (end_pos == string::npos) {
                output += input[i];
                continue;
            }

            string mangled = input.subslice(i, end_pos - i + 3);  // include "_E$"
            string dem     = demangle(mangled, ty);

            // For Modules, keep only basename (strip dirs + .k)
            if (ty == ObjectType::Module) {
                dem = basename_no_ext(dem);
            }

            output += dem;
            i = end_pos + 2;  // will ++ at loop end -> land after '$'
            continue;
        }

        output += input[i];
    }
    return output;
}


// --- Utility: remove leading "kairo::" (and the "::kairo::" variant)
inline string strip_kairo_prefix(const string &input) {
    const libcxx::string_view raw    = input.view();
    const libcxx::string_view prefix = "kairo::";
    const libcxx::string_view midfix = "::kairo::";

    libcxx::string result;
    result.reserve(raw.size());

    usize i = 0;
    while (i < raw.size()) {
        if (raw.substr(i).starts_with(midfix)) {
            result.append(midfix);
            i += midfix.size();
            continue;
        }

        if (raw.substr(i).starts_with(prefix) && (i < 2 || raw.substr(i - 2, 2) != "::")) {
            i += prefix.size();  // skip it
            continue;
        }

        result.push_back(raw[i]);
        i++;
    }

    return string(result.data(), result.size()).replace(" __cdecl", "");
}

}  // namespace ABI

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif
