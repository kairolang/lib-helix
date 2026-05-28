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

import ArithmeticError;

/// Error raised when an arithmetic operation results in a value too large to be represented
default class OverflowError derives ArithmeticError {
    /// Constructor with specific operation context
    fn OverflowError(self, operation: string, type_name: string) {
        ArithmeticError.ArithmeticError(f"Arithmetic overflow in {operation} (type: {type_name})");
    }
    
    /// Constructor with custom message
    fn OverflowError(self, msg: string) {
        ArithmeticError.ArithmeticError(msg);
    }
    
    /// Default constructor
    fn OverflowError(self) {
        ArithmeticError.ArithmeticError("Arithmetic overflow");
    }

    /// Virtual destructor
    override default fn op delete (self);
}