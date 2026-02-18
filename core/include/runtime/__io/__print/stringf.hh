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


// MIGRATE: TO KAIRO CORE
// FIXME: slice is very broken if mem forwarded or taken from const char*

#ifndef _$_HX_CORE_M7STRINGF
#define _$_HX_CORE_M7STRINGF

#include <include/config/config.hh>
#include <include/runtime/__error/runtime_error.hh>
#include <include/runtime/__panic/panic_config.hh>
#include <include/types/question/question_impl.hh>
#include <include/types/string/string.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

template <typename... Ty>
inline string stringf(string fmt) {
    return fmt;
}

/// \include belongs to the kairo standard library.
/// \brief format a string with arguments
///
/// a fmt string looks like this: "hello {} world {}" or "hello \\{ {} world \\}" (escaped braces - ony the {} gets replaced, while the \\{ changes to { and the \\} changes to })
/// each if the args go though a to_string function and are replaced in the string
///
/// \param fmt the format string
/// \param args the arguments to format into the string
/// \return the formatted string
// template <typename... Ty>
// inline string stringf(string fmt, Ty &&...args) {
//     const static string placeholder  = L"{}";
//     const static string escape_open  = L"\\{";
//     const static string escape_close = L"\\}";

//     // logic to use
//     // 1. find the first placeholder in the string
//     // note its position
//     // convert arg1 to string and replace the placeholder with the string
//     // increment the position ticker by the length of what was just added
//     // repeat until all args are used, if theres any mismatch then throw an error
//     // 2. if there are no placeholders then just return the string as is

//     string::string_t::size_type search_pos = 0;

//     ([&] {
//         string arg = to_string(std::Memory::forward<Ty>(args));

//         usize new_pos = fmt.raw_string().find(placeholder.raw(), search_pos, placeholder.length());
//         if (new_pos == string::string_t::npos) {
//             _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(Error::RuntimeError(L"[f-string rt]: too few arguments for format string"));
//         }

//         fmt.replace(new_pos, placeholder.length(), arg);

//         // now we need to find every escaped brace from new_pos to search_pos
//         string::string_t::size_type escape_pos = fmt.raw_string().find(escape_open.raw(), search_pos, escape_open.length());
//         while (escape_pos != string::string_t::npos && escape_pos < new_pos) {
//             fmt.raw_string().erase(escape_pos, 1);
//             escape_pos = fmt.raw_string().find(escape_open.raw(), escape_pos, escape_open.length());
//             new_pos--;
//         }

//         escape_pos = fmt.raw_string().find(escape_close.raw(), search_pos, escape_close.length());
//         while (escape_pos != string::string_t::npos && escape_pos < new_pos) {
//             fmt.raw_string().erase(escape_pos, 1);
//             escape_pos = fmt.raw_string().find(escape_close.raw(), escape_pos, escape_close.length());
//             new_pos--;
//         }
    
//         search_pos = new_pos + arg.length();
//     }(), ...);

//     if (fmt.raw_string().find(placeholder.raw(), search_pos, placeholder.length()) != string::string_t::npos) {
//         _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::RuntimeError("[f-string rt]: too many arguments for format string"));
//     }

//     // if there are any escaped braces left then remove them
//     string::string_t::size_type escape_pos = fmt.raw_string().find(escape_open.raw(), search_pos, escape_open.length());
//     while (escape_pos != string::string_t::npos) {
//         fmt.raw_string().erase(escape_pos, 1);
//         escape_pos = fmt.raw_string().find(escape_open.raw(), escape_pos, escape_open.length());
//     }

//     escape_pos = fmt.raw_string().find(escape_close.raw(), search_pos, escape_close.length());
//     while (escape_pos != string::string_t::npos) {
//         fmt.raw_string().erase(escape_pos, 1);
//         escape_pos = fmt.raw_string().find(escape_close.raw(), escape_pos, escape_close.length());
//     }

//     return fmt;
// }

template <typename... Ty>
inline string stringf(string fmt, Ty &&...args) {
    const static string placeholder  = L"{}";
    const static string escape_open  = L"\\{";
    const static string escape_close = L"\\}";

    string result;
    result.raw_string().reserve(fmt.size() + (sizeof...(args) * 8));

    usize index = 0;
    usize pos   = 0;
    const string args_arr[] = {to_string(std::Memory::forward<Ty>(args))...};

    for (size_t i = 0; i < fmt.size();) {
        if (i + 1 < fmt.size() && fmt[i] == L'\\' && (fmt[i + 1] == L'{' || fmt[i + 1] == L'}')) {
            result.push_back(fmt[i + 1]);
            i += 2;
            ++pos;
        } else if (i + 1 < fmt.size() && fmt[i] == L'{' && fmt[i + 1] == L'}') {
            if (index >= sizeof...(args)) {
                _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(Error::RuntimeError(L"[f-string rt]: too few format specifiers (\"{}\")"));
            }
            result.append(args_arr[index++]);
            pos += args_arr[index - 1].size();
            i   += 2;
        } else {
            result.push_back(fmt[i]);
            ++i;
            ++pos;
        }

    }

    if (index < sizeof...(args)) {
        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(Error::RuntimeError(L"[f-string rt]: too many format specifiers (\"{}\")"));
    }

    return result;
}

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M7STRINGF
