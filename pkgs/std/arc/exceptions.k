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

macro __LIB_EXCEPTIONS!: 0xa0;
macro __LIB_EXCEPTIONS_VERSION!: 0b00 << 0b00 << 0b01;
macro __LIB_UNWIND!: 0xa0;

/* 2 kind of exceptions here

panic -> compiler calls a panic_handle with a panic frame and the expr

- panic needs to remain interoperable with the C++ exceptions and the C++ stack unwinding
*/

// without __LIB_UNWIND panic frames are not unwound

eval if macros::defined!(__LIB_UNWIND!) {
    class PanicFrame requires <T> {
        var prev: *PanicFrame;
        var next: *PanicFrame;
        var data: *T;
    }
} else {
    class PanicFrame requires <T> {
        var data: *T;
    }
}

class Exception {
    fn Exception(self) {}
    fn Exception(self, msg: string) {}
    op as (self) -> string {}
}

#[panic()]
fn panic_handle(frame: *PanicFrame<T>, expr: T) requires <T> {
    eval if T derives Exception {
        // handle panic
    } else {
        // handle panic
    }
}

#[panic()]
fn panic_handle(expr: T) requires <T> {
    // handle panic
}

#[panic()]
fn panic_handle() {
    // handle panic
}
