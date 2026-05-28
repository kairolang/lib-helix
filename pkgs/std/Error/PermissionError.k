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

// import IOError;

/// Error when an operation is denied due to insufficient permissions
default class PermissionError derives std::Error::IOError {
    /// Constructor with filepath
    fn PermissionError(self, path: string, operation: string) {
        IOError.IOError(f"Permission denied: \'{path}\' (operation: {operation})");
    }
    
    /// Constructor with path and error code
    fn PermissionError(self, path: string, operation: string, error_code: i32) {
        IOError.IOError(f"Permission denied: '{path}' (operation: {operation})", error_code);
    }
    
    /// Constructor with custom message
    fn PermissionError(self, msg: string) {
        IOError.IOError(msg);
    }
    
    /// Constructor with custom message and error code
    fn PermissionError(self, msg: string, error_code: i32) {
        IOError.IOError(msg, error_code);
    }
    
    /// Default constructor
    default fn PermissionError(self);

    /// Virtual destructor
    override default fn op delete (self);
}