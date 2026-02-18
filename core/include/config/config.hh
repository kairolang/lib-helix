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
///------------------------------------------------------------------------------------ Kairo ---///

#ifndef __$LIBKAIRO_CONFIG__
#define __$LIBKAIRO_CONFIG__

namespace std {}  // namespace std

#if __cplusplus < 202002L
#error "C++20 or higher is required to use Kairo."
#endif

const long LIBKAIRO_VERSION_FULL  = (25 << 16) | (4 << 8) | (15);
const long LIBKAIRO_MAJOR_VERSION = (LIBKAIRO_VERSION_FULL >> 16) & 0xFF;
const long LIBKAIRO_MINOR_VERSION = (LIBKAIRO_VERSION_FULL >> 8) & 0xFF;
const long LIBKAIRO_PATCH_VERSION = (LIBKAIRO_VERSION_FULL) & 0xFF;

constexpr long LIBKAIRO_STANDARD = LIBKAIRO_MAJOR_VERSION;

#ifndef _LIBCPP_NODEBUG
#define _LIBCPP_NODEBUG
#endif

#ifndef _LIBCPP_HIDE_FROM_ABI
#define _LIBCPP_HIDE_FROM_ABI
#endif

#ifndef _LIBKAIRO_HIDE_FROM_ABI
#define _LIBKAIRO_HIDE_FROM_ABI
#endif

#ifndef _LIBCPP_TEMPLATE_VIS
#define _LIBCPP_TEMPLATE_VIS
#endif

#ifndef _NOEXCEPT
#define _NOEXCEPT noexcept
#endif

#ifndef _NEW
#define _NEW(x) new x
#endif

namespace kairo {
namespace libcxx = ::std;
}

#define H_NAMESPACE_BEGIN namespace kairo {
#define H_NAMESPACE_END }
#define H_STD_NAMESPACE_BEGIN namespace std {
#define H_STD_NAMESPACE_END }
#define H_STD_NAMESPACE kairo::std
#define H_NAMESPACE kairo
#define LIBCXX_NAMESPACE libcxx
#define LIBC_NAMESPACE libc

// muted yellow color
#define DEBUG_PRINT(...) print("\033[33m", __VA_ARGS__, "\033[0m")

#define KAIRO_FORCE_INLINE [[gnu::always_inline]] inline

#if !defined(__has_builtin)
#   define __has_builtin(x) 0
#endif

#if !defined(__GNUC__) && !defined(__clang__)
#   define __builtin_constant_p(x) 0
#endif

#if defined (_MSC_VER)
#    define _ALLOW_RTCc_IN_STL
#endif

#if defined(__has_attribute)
#   if __has_attribute(diagnose_if)
#       define DIAGNOSE_IF(args) __attribute__((diagnose_if args))
#   else
#       define DIAGNOSE_IF(args)
#   endif
#else
#   define DIAGNOSE_IF(args)
#endif

#if defined(__clang__) || defined(__GNUC__)
#   define _KAIRO_SUPPRESS_DEPRECATED_WARN_PUSH \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
#   define _KAIRO_SUPPRESS_DEPRECATED_WARN_POP _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#   define _KAIRO_SUPPRESS_DEPRECATED_WARN_PUSH \
        __pragma(warning(push)) __pragma(warning(disable : 4996)) __pragma(warning(disable : 4995))
#   define _KAIRO_SUPPRESS_DEPRECATED_WARN_POP __pragma(warning(pop))
#else
#   define _KAIRO_SUPPRESS_DEPRECATED_WARN_PUSH
#   define _KAIRO_SUPPRESS_DEPRECATED_WARN_POP
#endif

#if defined(__clang__) || defined(__GNUC__)
#   define _KAIRO_SUPPRESS_UNREACHABLE_WARN_PUSH \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wunreachable-code\"")
#define _KAIRO_SUPPRESS_UNREACHABLE_WARN_POP _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#   define _KAIRO_SUPPRESS_UNREACHABLE_WARN_PUSH __pragma(warning(push)) __pragma(warning(disable : 4702))
#   define _KAIRO_SUPPRESS_UNREACHABLE_WARN_POP __pragma(warning(pop))
#else
#   define _KAIRO_SUPPRESS_UNREACHABLE_WARN_PUSH
#   define _KAIRO_SUPPRESS_UNREACHABLE_WARN_POP
#endif

#endif