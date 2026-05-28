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

/// Error that occurs when a module or package import fails
default class ImportError derives std::Error::BaseError {
    priv var msg: string;
    priv var module_name: string;

    /// Constructor with module name
    fn ImportError(self, module_name: string) {
        self.module_name = std::Memory::move(module_name);
        var builder = std::Text::StringBuilder();
        builder.append("Failed to import module: '");
        builder.append(self.module_name);
        builder.append("'");
        self.msg = builder.to_string();
    }
    
    /// Constructor with module name and specific reason
    fn ImportError(self, module_name: string, reason: string) {
        self.module_name = std::Memory::move(module_name);
        var builder = std::Text::StringBuilder();
        builder.append("Failed to import module: '");
        builder.append(self.module_name);
        builder.append("' (");
        builder.append(reason);
        builder.append(")");
        self.msg = builder.to_string();
    }
    
    /// Constructor with custom message
    fn ImportError(self, msg: string) {
        self.msg = std::Memory::move(msg);
        self.module_name = "";
    }
    
    /// Default constructor
    default fn ImportError(self);

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
    
    /// Get the name of the module that failed to import
    const fn get_module_name(self) -> string {
        return self.module_name;
    }
}