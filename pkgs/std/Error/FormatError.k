/// Constructor with format string and position information
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

/// Error related to string formatting operations
default class FormatError derives std::Error::BaseError {
    priv var msg: string;

    /// Constructor with a custom error message
    fn FormatError(self, msg: string) {
        self.msg = std::Memory::move(msg);
    }
    
    fn FormatError(self, format: string, position: u32) {
        // build the snippet with a "->" at the error point
        var snippet: string = format;

        if position < format.length() {
            var prefix = format.substring(0, position) if position > 0  else "";
            var chr    = format.substring(position, position + 1);
            var suffix = format.substring(position + 1) if position < format.length() - 1 else "";
            
            snippet = f"{prefix}-> {chr}{suffix}";
        }

        // now use a single f-string for the full message
        self.msg = f"Invalid format at position {position}: \'{snippet}\'";
    }
    
    /// Default constructor
    default fn FormatError(self);

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