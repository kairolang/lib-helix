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

#ifndef _$_HX_CORE_M8BUILTINS
#define _$_HX_CORE_M8BUILTINS

#include "literals.hh"
#include "primitives.hh"
#include "size_t.hh"
#include "variant.hh"

// ensure that the sizes are correct and match the platform
static_assert(sizeof(usize) == sizeof(void *), "usize must match the size of a pointer.");
static_assert(sizeof(isize) == sizeof(void *), "isize must match the size of a pointer.");

#endif  // _$_HX_CORE_M8BUILTINS
