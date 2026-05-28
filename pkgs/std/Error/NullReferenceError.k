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

/// Error raised when attempting to access or operate on a null reference
default class NullReferenceError derives std::Error::BaseError {
    priv var msg: string;

    /// Constructor with a custom error message
    fn NullReferenceError(self, msg: string) {
        self.msg = std::Memory::move(msg);
    }
    
    /// Constructor with operation information
    fn NullReferenceError(self, operation: string) {
        var builder = std::Text::StringBuilder();
        builder.append("Null reference encountered in '");
        builder.append(operation);
        builder.append("'");
        self.msg = builder.to_string();
    }
    
    /// Default constructor
    fn NullReferenceError(self) {
        self.msg = "Null reference exception";
    }

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