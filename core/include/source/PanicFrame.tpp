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

#ifndef _$_HX_CORE_M10PANICFRAME_TPP
#define _$_HX_CORE_M10PANICFRAME_TPP

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Panic {

template <typename T>
inline Frame::Frame(T obj, const char *filename, usize lineno)
    : _file(filename)
    , _line(lineno) {
    initialize<T>(&obj);
}

template <typename T>
inline Frame::Frame(T obj, string filename, usize lineno)
    : _file(std::Memory::move(filename))
    , _line(lineno) {
    initialize<T>(&obj);
}

[[noreturn]] inline Frame::Frame(Frame &obj, const string &, usize) { obj.operator$panic(); }
[[noreturn]] inline Frame::Frame(Frame &&obj, const string &, usize) { obj.operator$panic(); }

inline void Frame::operator$panic() const {
    HX_FN_Vi_Q5_13_kairopanic_handler_Q3_5_5_stdPanicFrame_C_PK_Rv(this);
    throw;
}
}  // namespace Panic

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M10PANICFRAME