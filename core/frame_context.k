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

eval fn <T> std::Panic::FrameContext::throw_object(error: unsafe *std::TypeErasure) {
    if error == (&null) {
        std::crash(std::Error::RuntimeError("Tried to crash with a null object."));
    }

    if error->type_info() != (&typeid(T)) {
        std::crash(std::Error::TypeMismatchError("Type mismatch when crashing."));
    }

    var object: unsafe *T = (**error) as unsafe *T;
    std::crash(*object);
}

const fn std::Panic::FrameContext::object() -> unsafe *void {
    return (**self.error) if (self.error != (&null)) else (&null);
}

fn std::Panic::FrameContext::crash() {
    self.handler(self.error);
    std::crash(std::Error::RuntimeError("Object \'" + string(self.error->type_info()->name()) + "\' failed to panic."));
}


const fn std::Panic::FrameContext::type_name() -> string {
    if (self.error == (&null)) {
        return "null";
    }

    const eval if defined(_MSC_VER) {
        return self.error->type_info()->name();
    } else {
        const mangled_name: unsafe *std::Legacy::char = self.error->type_info()->name();

        if (mangled_name == (&null)) || ((*mangled_name) == 0) {
            return string();
        }

        var status: i32 = 0;
        
        // __cxa_demangle returns a malloc-allocated string
        var demangle: unsafe *std::Legacy::char = __cxxabiv1::__cxa_demangle(mangled_name, (&null), (&null), (&status));
        if status == 0 && (demangle != (&null)) {
            var result: string = nstring_to_string(demangle);
        
            free(demangle);
            return result;
        }
        
        return nstring_to_string(mangled_name);
    }
}