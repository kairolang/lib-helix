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

default class AttributeError derives std::Error::BaseError {
    priv var msg: string;

    fn AttributeError(self, msg: string) {
        self.msg = std::Memory::move(msg);
    }
    
    default fn AttributeError(self);

    override default fn op delete (self);
    
    override const fn op panic (self) -> string {
        return self.msg;
    }
    
    override const fn op as (self) -> string {
        return msg;
    }
}

