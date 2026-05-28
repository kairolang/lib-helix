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

#ifndef _$_HX_CORE_M4UNIX
#define _$_HX_CORE_M4UNIX

#if defined(__linux__) || defined(__APPLE__)
#   include <pthread.h>
#   include <sys/mman.h>
#   include <sys/resource.h>
#   include <unistd.h>
#   include <cxxabi.h>
#   include <fcntl.h>
#endif

#endif  // _$_HX_CORE_M4UNIX
