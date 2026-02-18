# This file contains everything contained in the kairo std

# std files
## std/system.kro
## std/file_system.kro
## std/bit.kro
## std/input.kro
## std/stream.kro
## std/any.kro
## std/meta.kro
## std/flat_map.kro
## std/debug.kro
###  - `class Frame`
#### - `
### - `type TraceFrames = vector<std::debug::Frame>`
### - `std::debug::breakpoint()`
### - `std::debug::backtrace()  -> std::debug::TraceFrames`
### - `std::debug::is_enabled() -> bool`
## std/interfaces.kro