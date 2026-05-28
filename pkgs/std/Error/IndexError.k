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

/// Error indicating an index is outside the valid range
default class IndexError derives std::Error::BaseError {
    priv var msg: string;

    /// Constructor with a custom error message
    fn IndexError(self, msg: string) {
        self.msg = std::Memory::move(msg);
    }
    
    /// Constructor with index information
    fn IndexError(self, index: i64, length: i64) {
        var builder = std::Text::StringBuilder();
        builder.append("Index out of range: ");
        builder.append(index);
        builder.append(" (length: ");
        builder.append(length);
        builder.append(")");
        self.msg = builder.to_string();
    }
    
    /// Default constructor
    default fn IndexError(self);

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