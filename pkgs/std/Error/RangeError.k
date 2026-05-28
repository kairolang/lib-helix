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

/// Error indicating a value outside of a valid range
default class RangeError derives std::Error::BaseError {
    priv var msg: string;

    /// Constructor with a custom error message
    fn RangeError(self, msg: string) {
        self.msg = std::Memory::move(msg);
    }
    
    /// Constructor with value and range information
    fn RangeError(self, value: f64, min: f64, max: f64) {
        var builder = std::Text::StringBuilder();
        builder.append("Value ");
        builder.append(value);
        builder.append(" is outside the valid range [");
        builder.append(min);
        builder.append(", ");
        builder.append(max);
        builder.append("]");
        self.msg = builder.to_string();
    }
    
    /// Default constructor
    default fn RangeError(self);

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