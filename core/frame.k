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
#[trivially_import(true)]

ffi "c++" import "include/runtime/__panic/panic.hh";

fn <T impl std::Interface::ClassType> std::Panic::Frame::initialize(const obj: unsafe *T) {
    eval if __inline_cpp("Panic::Interface::PanickingStatic<T>") {
        _reason = __inline_cpp("T::operator$panic()");
    } else if __inline_cpp("Panic::Interface::PanickingInstance<T>") {
        _reason = __inline_cpp("(*obj).operator$panic()");
    } else {
        static_assert(
            __inline_cpp("Panic::Interface::Panicking<T>"),
            r"Frame invoked with a type that lacks a panic operator. Ensure the type "
             "declares `class ... impl Panic::Panicking` and implements either `op "
             "panic fn(self) -> string` or the static equivalent."
        );
    }

    var object: unsafe *T = &null;

    static_assert(
        __inline_cpp("libcxx::is_copy_constructible_v<T>") || __inline_cpp("libcxx::is_move_constructible_v<T>"),
         r"Frame invoked with a type that is not copy or move constructible.");

    try {
        eval if (__inline_cpp("libcxx::is_move_constructible_v<T>")) {
            object        = _NEW(T(std::Memory::move(*obj)));
            self.context = Panic::FrameContext(object);
        } else if (__inline_cpp("libcxx::is_copy_constructible_v<T>")) {
            object        = _NEW(T(*obj));
            self.context = Panic::FrameContext(object);
        }
    } catch {
        __inline_cpp("delete object");
        std::crash(std::Error::RuntimeError("Failed to initialize panic frame."));
    }
}

const fn std::Panic::Frame::file() -> string {
    return self._file;
}

const fn std::Panic::Frame::line() -> usize {
    return self._line;
}

const fn std::Panic::Frame::reason() -> string {
    return self._reason;
}

const fn std::Panic::Frame::get_context() -> *std::Panic::FrameContext {
    return &context;
}