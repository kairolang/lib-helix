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

#ifndef _$_HX_CORE_M4LIBC
#define _$_HX_CORE_M4LIBC

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <memory.h>

#ifndef _MSC_VER
    #include <unistd.h>
    #include <cxxabi.h>
#else
    #include <sys/utime.h>
    #include <sys/locking.h>
    #include <conio.h>
    #include <corecrt.h>
    #include <corecrt_io.h>
    #include <corecrt_malloc.h>
    #include <corecrt_math.h>
    #include <corecrt_math_defines.h>
    #include <corecrt_memcpy_s.h>
    #include <corecrt_memory.h>
    #include <corecrt_search.h>
    #include <corecrt_share.h>
    #include <corecrt_startup.h>
    #include <corecrt_stdio_config.h>
    #include <corecrt_terminate.h>
    #include <corecrt_wconio.h>
    #include <corecrt_wctype.h>
    #include <corecrt_wdirect.h>
    #include <corecrt_wio.h>
    #include <corecrt_wprocess.h>
    #include <corecrt_wstdio.h>
    #include <corecrt_wstdlib.h>
    #include <corecrt_wstring.h>
    #include <corecrt_wtime.h>
    #include <crtdbg.h>
    #include <direct.h>
    #include <dos.h>
    #include <mbctype.h>
    #include <io.h>
    #include <fpieee.h>
    #include <malloc.h>
    #include <mbstring.h>
    #include <minmax.h>
    #include <new.h>
    #include <process.h>
    #include <safeint.h>
    #include <safeint_internal.h>
    #include <share.h>
    #include <tchar.h>
#endif

#endif  // _$_HX_CORE_M4LIBC