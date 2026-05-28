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

/// Error for network connection failures
default class ConnectionError derives NetworkError {
    /// Constructor with host and port information
    fn ConnectionError(self, host: string, port: u16) {
        NetworkError.NetworkError(f"Failed to connect to {host}:{port}");
    }
    
    /// Constructor with host, port, and error code
    fn ConnectionError(self, host: string, port: u16, error_code: i32) {
        NetworkError.NetworkError(f"Failed to connect to {host}:{port}", error_code);
    }
    
    /// Constructor with custom message
    fn ConnectionError(self, msg: string) {
        NetworkError.NetworkError(msg);
    }
    
    /// Constructor with custom message and error code
    fn ConnectionError(self, msg: string, error_code: i32) {
        NetworkError.NetworkError(msg, error_code);
    }
    
    /// Default constructor
    fn ConnectionError(self) {
        NetworkError.NetworkError("Connection failed");
    }

    /// Virtual destructor
    override default fn op delete (self);
}