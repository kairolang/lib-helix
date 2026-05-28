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

import ConcurrencyError;

default class DeadlockError derives ConcurrencyError {
    fn DeadlockError(self, resource1: string, resource2: string) {
        ConcurrencyError.ConcurrencyError(f"Deadlock detected between resources: \'{resource1}\' and \'{resource2}\'");
    }
    
    fn DeadlockError(self, thread_ids: vec::<u64>) {
        var str = "Deadlock detected between threads: ";

        for (var i = 0; i < thread_ids.length(); i++) {
            if (i > 0) {
                str += ", ";
            }

            str += std::to_string(thread_ids[i]);
        }

        ConcurrencyError.ConcurrencyError(str + ".");
    }
    
    fn DeadlockError(self, msg: string) {
        ConcurrencyError.ConcurrencyError(msg);
    }
    
    fn DeadlockError(self) {
        ConcurrencyError.ConcurrencyError("Deadlock detected");
    }

    override default fn op delete (self);
}