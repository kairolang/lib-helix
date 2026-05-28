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

/// Error that occurs when a race condition is detected
default class RaceConditionError derives std::Error::ConcurrencyError {
    /// Constructor with resource information
    fn RaceConditionError(self, resource_name: string) {
        super.ConcurrencyError(std::String::format("Race condition detected on resource: '{}'", resource_name));
    }
    
    /// Constructor with detailed information
    fn RaceConditionError(self, resource_name: string, thread1: u64, thread2: u64) {
        super.ConcurrencyError(std::String::format("Race condition detected on resource '{}' between threads {} and {}", 
                                                 resource_name, thread1, thread2));
    }
    
    /// Constructor with custom message
    fn RaceConditionError(self, msg: string) {
        super.ConcurrencyError(msg);
    }
    
    /// Default constructor
    fn RaceConditionError(self) {
        super.ConcurrencyError("Race condition detected");
    }

    /// Virtual destructor
    override default fn op delete (self);
}