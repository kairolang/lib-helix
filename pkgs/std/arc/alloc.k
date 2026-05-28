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

import types::{Allocator, AllocatorStats};

ffi "c++" import "";

class DefaultAllocator derives Allocator {
    fn DefaultAllocator(self) derives default; // call the default constructor for all members.
    fn alloc(self, size: usize, alignment: usize) -> *void {}
    fn dealloc(self, ptr: *void, size: usize, alignment: usize) {}
    fn realloc(self, ptr: *void, old_size: usize, new_size: usize, alignment: usize) -> *void {}

    #[override]
    fn alloc_d(self, size: usize) -> *void {
        return self.alloc(size, 16);
    }

    #[override]
    fn realloc_d(self, ptr: *void, old_size: usize, new_size: usize) -> *void {
        return self.realloc(ptr, old_size, new_size, 16);
    }

    #[override]
    fn get_stats(self) -> AllocatorStats {
        return AllocatorStats::new();
    }

    #[override]
    fn can_alloc(self, size: usize, alignment: usize) {
        return true;
    }

    //===---- vars ----===//
    priv eval var stats = AllocatorStats::new();
}