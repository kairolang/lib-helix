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
// build.hx - The build script for std

// this is a hypothetical build script in kairo
// syntax is not final tho.

import build_tools as bt;

module build_tools {
class InvalidInput derives BaseError {
    fn InvalidInput(self, msg: string) {
        super.msg = msg;
    }

    fn to_string(self) -> string {
        return super.msg;
    }
}

class BuildConfig {
    var target: string;
    var cxx_toolchain: string;
    // ...

    fn BuildConfig(self) {}

    fn set_target(self, target: string) -> &self {
        self.target = target;

        var allowed_targets = (
            "x86_64-apple-darwin",
            "x86_64-windows-none",
            "x86_64-windows-msvc",
            // ...
        );

        if !(target in allowed_targets) {
            panic InvalidInput(f"provided target: {target} in invalid use a vali tripple");
        }

        return &self;
    }
}
} // end module build_tools

import git;

import python_ffi;

fn build() -> noreturn! {
    var config = bt::BuildConfig()
        .set_target("x86_64-apple-darwin")
        .set_cxx_toolchain("clang++")
        .set_output_dir("build")
        .set_optimization_level(bt::optimization::RELEASE);

    var builder = bt::Builder(config)
           .add_source("src/main.kairo")
           .add_source("src/utils.kairo")
           .add_ffi(python_ffi::FFI())
           .add_ffi(cpp_ffi::FFI())
           .add_include_path("include")
           .set_output("std")
           .set_standalone(true); // no runtime dependents
           .set_build("lib");     // lib | bin
    
    if (git::file_changed("src/test.py")) {
        bt::run();
    }

    builder.watch_file("test.k");
    builder.watch_dir("src");

    builder.compile();
    bt::run_tests();
}