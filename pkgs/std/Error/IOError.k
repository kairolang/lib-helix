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

/// Error indicating problems with input/output operations
default class IOError derives std::Error::BaseError {
    priv var msg: string;
    priv var error_code: i32;

    /// Constructor with a custom error message
    fn IOError(self, msg: string) {
        self.msg = std::Memory::move(msg);
        self.error_code = 0;
    }
    
    /// Constructor with error message and code
    fn IOError(self, msg: string, error_code: i32) {
        self.msg = std::Memory::move(msg);
        self.error_code = error_code;
    }
    
    /// Default constructor
    default fn IOError(self);

    /// Virtual destructor
    override default fn op delete (self);

    priv fn generate_string(self) -> string {
        if (self.error_code != 0) {
            return f"{self.msg} (error code: {self.error_code})";
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
    
    /// Get the error code
    const fn get_error_code(self) -> i32 {
        return self.error_code;
    }
}