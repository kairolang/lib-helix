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

import NetworkError;

/// Error when a hostname cannot be resolved
default class HostNotFoundError derives NetworkError {
    /// Constructor with hostname
    fn HostNotFoundError(self, hostname: string) {
        NetworkError.NetworkError(f"Host not found: \'{hostname}\'");
    }
    
    /// Constructor with hostname and error code
    fn HostNotFoundError(self, hostname: string, error_code: i32) {
        NetworkError.NetworkError(f"Host not found: \'{hostname}\'", error_code);
    }
    
    /// Constructor with custom message
    fn HostNotFoundError(self, msg: string) {
        NetworkError.NetworkError(msg);
    }
    
    /// Constructor with custom message and error code
    fn HostNotFoundError(self, msg: string, error_code: i32) {
        NetworkError.NetworkError(msg, error_code);
    }
    
    /// Default constructor
    default fn HostNotFoundError(self);

    /// Virtual destructor
    override default fn op delete (self);
}