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

/// General-purpose error for runtime problems
default class RuntimeError derives std::Error::BaseError {
    priv var msg: string;

    /// Constructor with a custom error message
    fn RuntimeError(self, msg: string) {
        self.msg = std::Memory::move(msg);
    }
    
    /// Default constructor
    default fn RuntimeError(self);

    /// Virtual destructor
    override default fn op delete (self);
    
    /// Get a string representation for panic
    override const fn op panic (self) -> string {
        return self.msg;
    }
    
    /// Convert the error to a string
    override const fn op as (self) -> string {
        return self.msg;
    }
}