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

#ifndef _$_HX_CORE_M10STACKTRACE
#define _$_HX_CORE_M10STACKTRACE

#include <include/abi/abi.hh>
#include <include/c++/libc++.hh>
#include <include/runtime/__io/__print/print.hh>
#include <include/runtime/__io/__print/stringf.hh>
#include <include/types/types.hh>

#ifdef _WIN32
#include <dbghelp.h>
#include <windows.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <string.h>
#include <unistd.h>
#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif
#endif

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Stacktrace {

#if defined(_MSC_VER)
#define __KAIRO_FUNCNAME__ __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define __KAIRO_FUNCNAME__ __PRETTY_FUNCTION__
#else
#define __KAIRO_FUNCNAME__ __func__
#endif

enum class FrameKind : uint8_t {
    Kairo  = 0,
    Hybrid = 1,
    Native = 2,
};
struct Location {
    const wchar_t *file;
    const wchar_t *func;
    uint32_t       line;

    wchar_t file_buf[1024];
    wchar_t func_buf[1024];

    static void char_to_wchar(const char *src, wchar_t *dst, size_t dst_cap) {
        if (!src || !dst || dst_cap == 0) {
            return;
        }

        size_t converted = mbstowcs(dst, src, dst_cap - 1);
        if (converted == (size_t)-1) {
            dst[0] = L'\0';
        } else {
            dst[converted] = L'\0';
        }
    }

    Location(const char *f, const char *fn, uint32_t l)
        : file(file_buf)
        , func(func_buf)
        , line(l) {
        char_to_wchar(f, file_buf, sizeof(file_buf) / sizeof(wchar_t));
        char_to_wchar(fn, func_buf, sizeof(func_buf) / sizeof(wchar_t));
    }

    Location()
        : file(L"")
        , func(L"")
        , line(0) {
        file_buf[0] = L'\0';
        func_buf[0] = L'\0';
    }
};

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4324) // structure was padded due to alignment specifier
#endif

struct alignas(16) FrameSummary {  // exactly 32 bytes on 64-bit and 16 bytes on 32-bit
    Location     *loc;
    FrameSummary *prev;
    FrameKind     kind;
};


#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
inline thread_local FrameSummary *g_tls_kairo_head = nullptr;
#else
inline const FrameSummary *g_tls_kairo_head = nullptr;
#endif

struct RegisterFrame {
    FrameSummary frame{};

    explicit RegisterFrame(const Location *loc, const FrameKind kind = FrameKind::Hybrid) noexcept {
        frame.loc        = const_cast<Location *>(loc);
        frame.prev       = g_tls_kairo_head;
        frame.kind       = kind;
        g_tls_kairo_head = &frame;
    }

    ~RegisterFrame() noexcept { g_tls_kairo_head = frame.prev; }

    RegisterFrame(const RegisterFrame &)            = delete;
    RegisterFrame &operator=(const RegisterFrame &) = delete;
    RegisterFrame(RegisterFrame &&)                 = delete;
    RegisterFrame &operator=(RegisterFrame &&)      = delete;
};

#if defined(_MSC_VER) && _MSC_VER < 1940
#  error "Kairo requires MSVC 19.14 (VS 2017 15.7) or newer for constexpr tracing support."
#endif

#define __REGISTER_TRACE_BLOCK__(file_cstr, line_num, var_n)                   \
    if !consteval {                                                            \
      static ::kairo::std::Stacktrace::Location var_n{                         \
          (file_cstr), __KAIRO_FUNCNAME__, static_cast<uint32_t>(line_num)};   \
      ::kairo::std::Stacktrace::RegisterFrame _hx_scope(                       \
          &var_n, std::Stacktrace::FrameKind::Kairo);                          \
    }

#define __REGISTER_KAIRO_TRACE_BLOCK__(file_cstr, line_num, func_cstr, var_n)  \
    if !consteval {                                                            \
      static ::kairo::std::Stacktrace::Location var_n{                         \
          (file_cstr), (func_cstr), static_cast<uint32_t>(line_num)};          \
      ::kairo::std::Stacktrace::RegisterFrame _hx_scope(                       \
          &var_n, std::Stacktrace::FrameKind::Kairo);                          \
    }

#define __REGISTER_HYBRID_TRACE_BLOCK__(var_n)                                 \
    if !consteval {                                                            \
      static ::kairo::std::Stacktrace::Location var_n{                         \
          __FILE__, __KAIRO_FUNCNAME__, (static_cast<uint32_t>(__LINE__))};    \
      ::kairo::std::Stacktrace::RegisterFrame _hx_cpp_scope(&var_n);           \
    }

#define MAX_STACK_FRAME_DEPTH 1024

// when getting back trace frames we also need to capture native frames
// so we can get a complete picture of the call stack.
FrameSummary *capture(int max_depth = MAX_STACK_FRAME_DEPTH);

#ifdef _WIN32

inline FrameSummary *capture(int max_depth) {
    static thread_local FrameSummary s_nodes[MAX_STACK_FRAME_DEPTH];

    auto clamp_limit = [](int v) -> int {
        if (v <= 0)
            return MAX_STACK_FRAME_DEPTH;
        if (v > MAX_STACK_FRAME_DEPTH)
            return MAX_STACK_FRAME_DEPTH;
        return v;
    };

    const int limit = clamp_limit(max_depth);
    int       idx   = 0;

    // Walk the thread-local Kairo frame chain and snapshot into s_nodes.
    // Preserve kind/loc and rebuild prev pointers to point into the snapshot.
    for (const FrameSummary *cur = g_tls_kairo_head; cur != nullptr && idx < limit;
         cur                     = cur->prev) {
        s_nodes[idx].loc  = cur->loc;
        s_nodes[idx].kind = cur->kind;
        s_nodes[idx].prev = (idx > 0) ? &s_nodes[idx - 1] : nullptr;
        ++idx;
    }

    // If nothing captured, return nullptr (caller expects nullptr when empty).
    if (idx == 0) {
        return nullptr;
    }

    // Return pointer to the most-recently-captured frame (tail of our snapshot).
    return &s_nodes[idx - 1];
}

#else

inline FrameSummary *capture(int max_depth) {
    static thread_local FrameSummary s_nodes[MAX_STACK_FRAME_DEPTH];
    static thread_local Location     s_native_locs[MAX_STACK_FRAME_DEPTH];
    static thread_local wchar_t      s_fn_bufs[MAX_STACK_FRAME_DEPTH][1024];
    static thread_local wchar_t      s_file_bufs[MAX_STACK_FRAME_DEPTH][1024];

    auto clamp_limit = [](int v) {
        if (v <= 0) {
            return MAX_STACK_FRAME_DEPTH;
        }

        if (v > MAX_STACK_FRAME_DEPTH) {
            return MAX_STACK_FRAME_DEPTH;
        }

        return v;
    };

    auto copy_wstr = [](wchar_t *dst, size_t cap, const char *src) {
        if (!dst || cap == 0) {
            return;
        }

        if (!src) {
            dst[0] = L'\0';
            return;
        }

        mbstowcs(dst, src, cap - 1);
        dst[cap - 1] = L'\0';
    };

    const int limit = clamp_limit(max_depth);
    int       idx   = 0;

    for (const FrameSummary *cur = g_tls_kairo_head; (cur != nullptr) && idx < limit;
         cur                     = cur->prev) {
        s_nodes[idx].loc  = cur->loc;
        s_nodes[idx].kind = cur->kind;
        s_nodes[idx].prev = (idx > 0) ? &s_nodes[idx - 1] : nullptr;
        ++idx;
    }

    const int space_left = limit - idx;
    if (space_left <= 0) {
        return (idx > 0) ? &s_nodes[idx - 1] : nullptr;
    }

    void *stack[MAX_STACK_FRAME_DEPTH];
    int   to_capture = (space_left > MAX_STACK_FRAME_DEPTH) ? MAX_STACK_FRAME_DEPTH : space_left;
    if (to_capture <= 0) {
        return (idx > 0) ? &s_nodes[idx - 1] : nullptr;
    }

    int captured = backtrace(stack, to_capture);
    if (captured <= 0) {
        return (idx > 0) ? &s_nodes[idx - 1] : nullptr;
    }

    char **symbols = backtrace_symbols(stack, captured);

    const int skip     = (captured > 2) ? 2 : 0;
    int       native_i = 0;

    for (int i = skip; i < captured && idx < limit && native_i < MAX_STACK_FRAME_DEPTH;
         ++i, ++native_i) {
        const char *fn   = "???";
        const char *file = "???";
        uint32_t    line = 0;

        Dl_info info{};
        if (dladdr(stack[i], &info)) {
            if (info.dli_sname && info.dli_sname[0] != '\0') {
                int   status    = 0;
                char *demangled = abi::__cxa_demangle(info.dli_sname, nullptr, nullptr, &status);

                if (status == 0 && demangled) {
                    fn = demangled;
                } else {
                    fn = info.dli_sname;
                }

                if (fn == nullptr || fn[0] == '\0') {
                    continue;
                }

                ::free(demangled);
            }
            if (info.dli_fname && info.dli_fname[0] != '\0') {
                file = info.dli_fname;
            }
        }

        if ((file == nullptr || file[0] == '\0' || libcxx::string(file) == "???") &&
            (symbols != nullptr) && symbols[i]) {
            const char *sym = symbols[i];
            const char *p   = sym;
            const char *end = sym;

            while (*end && *end != ' ' && *end != '(') {
                ++end;
            }

            if (end > p) {
                thread_local wchar_t tmp_path[1024];
                size_t               len = static_cast<size_t>(end - p);
                if (len >= (sizeof(tmp_path) / sizeof(wchar_t))) {
                    len = (sizeof(tmp_path) / sizeof(wchar_t)) - 1;
                }

                char narrow_buf[1024];

                if (len >= sizeof(narrow_buf)) {
                    len = sizeof(narrow_buf) - 1;
                }

                for (size_t k = 0; k < len; ++k) {
                    narrow_buf[k] = p[k];
                }

                narrow_buf[len] = '\0';

                mbstowcs(tmp_path, narrow_buf, sizeof(tmp_path) / sizeof(wchar_t));
                tmp_path[(sizeof(tmp_path) / sizeof(wchar_t)) - 1] = L'\0';
                file                                               = narrow_buf;

                copy_wstr(
                    s_file_bufs[native_i], sizeof(s_file_bufs[0]) / sizeof(wchar_t), narrow_buf);
            }
        }

        copy_wstr(s_fn_bufs[native_i], sizeof(s_fn_bufs[0]) / sizeof(wchar_t), fn);
        copy_wstr(s_file_bufs[native_i], sizeof(s_file_bufs[0]) / sizeof(wchar_t), file);

        if (s_fn_bufs[native_i] == nullptr || s_fn_bufs[native_i][0] == L'\0') {
            // skip empty function names
            --native_i;
            continue;
        }

        s_native_locs[native_i].func = s_fn_bufs[native_i];
        s_native_locs[native_i].file = s_file_bufs[native_i];
        s_native_locs[native_i].line = line;

        s_nodes[idx].loc  = &s_native_locs[native_i];
        s_nodes[idx].kind = FrameKind::Native;
        s_nodes[idx].prev = (idx > 0) ? &s_nodes[idx - 1] : nullptr;
        ++idx;
    }

    if (symbols != nullptr) {
        ::free(symbols);
    }

    return (idx > 0) ? &s_nodes[idx - 1] : nullptr;
}

#endif  // POSIX/Linux

inline void backtrace(const FrameSummary *cur = capture()) {
    int idx = 0;

    while (cur != nullptr && idx < MAX_STACK_FRAME_DEPTH) {
        if (cur->loc != nullptr) {
            if (cur->loc->file && cur->loc->func && cur->loc->line != 0) {
                std::print(std::stringf(
                    L"\x1b[31m{}\x1b[0m (\x1b[33m{}:{})",
                    std::ABI::strip_kairo_prefix(std::ABI::demangle_partial(cur->loc->func)),
                    cur->loc->file,
                    cur->loc->line));
            } else if (cur->loc->file && cur->loc->func) {
                std::print(std::stringf(
                    L"\x1b[31m{}\x1b[0m (\x1b[33m{}\x1b[0m)",
                    std::ABI::strip_kairo_prefix(std::ABI::demangle_partial(cur->loc->func)),
                    cur->loc->file));
            } else if (cur->loc->func) {
                std::print(std::stringf(L"\x1b[31m{}\x1b[0m", cur->loc->func));
            } else {
                std::print(L"<unknown>");
            }
        } else {
            std::print(L"<unknown>");
        }

        cur = cur->prev;
        ++idx;
    }
}
}  // namespace Stacktrace

H_STD_NAMESPACE_END
H_NAMESPACE_END
#endif