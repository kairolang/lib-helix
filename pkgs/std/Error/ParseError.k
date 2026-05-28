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

/// Error related to parsing operations (JSON, XML, numeric values, etc.)
default class ParseError derives std::Error::BaseError {
    priv var msg: string;
    priv var line: isize;
    priv var column: isize;

    fn ParseError(self, msg: string) {
        self.msg = std::Memory::move(msg);
        self.line = -1;
        self.column = -1;
    }
    
    fn ParseError(self, msg: string, line: isize, column: isize) {
        self.msg = std::Memory::move(msg);
        self.line = line;
        self.column = column;
    }
    
    default fn ParseError(self);

    override default fn op delete (self);

    priv fn generate_string(self) -> string {
        if (self.line >= 0 && self.column >= 0) {
            return f"{self.msg} at line {self.line}, column {self.column}";
        }
        return self.msg;
    }
    
    /// Get a string representation for panic
    override const fn op panic (self) -> string {
        return self.generate_string();
    }
    
    /// Convert the error to a string
    override const fn op as (self) -> string {
        return self.generate_string();
    }
    
    /// Get the line number where the error occurred
    const fn get_line(self) -> isize {
        return self.line;
    }
    
    /// Get the column number where the error occurred
    const fn get_column(self) -> isize {
        return self.column;
    }
}