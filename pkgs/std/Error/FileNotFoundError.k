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

import IOError;

/// Error when a file or directory cannot be found
default class FileNotFoundError derives IOError {
    /// Constructor with filepath
    fn FileNotFoundError(self, path: string) {
        IOError.IOError(f"File not found: \'{path}\'");
    }
    
    /// Constructor with path and error code
    fn FileNotFoundError(self, path: string, error_code: i32) {
        IOError.IOError(f"File not found: \'{path}\'", error_code);
    }
    
    /// Constructor with custom message
    fn FileNotFoundError(self, msg: string) {
        IOError.IOError(msg);
    }
    
    /// Default constructor
    default fn FileNotFoundError(self);

    /// Virtual destructor
    override default fn op delete (self);
}