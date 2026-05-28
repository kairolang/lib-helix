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

/// Error raised when attempting to divide by zero
default class DivideByZeroError derives ArithmeticError {
    /// Constructor with specific operation context
    fn DivideByZeroError(self, operation: string) {
        ArithmeticError.ArithmeticError(f"Division by zero in {operation}");
    }
    
    /// Default constructor
    fn DivideByZeroError(self) {
        ArithmeticError.ArithmeticError("Division by zero");
    }

    /// Virtual destructor
    override default fn op delete (self);
}