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

/// Error raised when an arithmetic operation results in a value too small to be represented
default class UnderflowError derives std::Error::ArithmeticError {
    /// Constructor with specific operation context
    fn UnderflowError(self, operation: string, type_name: string) {
        super.ArithmeticError(std::String::format("Arithmetic underflow in {} (type: {})", operation, type_name));
    }
    
    /// Constructor with custom message
    fn UnderflowError(self, msg: string) {
        super.ArithmeticError(msg);
    }
    
    /// Default constructor
    fn UnderflowError(self) {
        super.ArithmeticError("Arithmetic underflow");
    }

    /// Virtual destructor
    override default fn op delete (self);
}