///--- The Kairo Project ------------------------------------------------------------------------///
///                                                                                              ///
///   Part of the Kairo Project, under the Attribution 4.0 International license (CC BY 4.0).    ///
///   You are allowed to use, modify, redistribute, and create derivative works, even for        ///
///   commercial purposes, provided that you give appropriate credit, and indicate if changes    ///
///   were made.                                                                                 ///
///                                                                                              ///
///   For more information on the license terms and requirements, please visit:                  ///
///     https://creativecommons.org/licenses/by/4.0/                                             ///
///                                                                                              ///
///   SPDX-License-Identifier: CC-BY-4.0                                                         ///
///   Copyright (c) 2024 The Kairo Project (CC BY 4.0)                                           ///
///                                                                                              ///
///-------------------------------------------------------------------------------- lib-helix ---///

#ifndef _$_HX_CORE_M17PANICFRAMECONTEXT_TPP
#define _$_HX_CORE_M17PANICFRAMECONTEXT_TPP

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>

H_NAMESPACE_BEGIN
H_STD_NAMESPACE_BEGIN

namespace Panic {
FrameContext::~FrameContext() {
    if (error != nullptr) {
        error->destroy();
        delete error;
        error = nullptr;
    }
}

FrameContext::FrameContext()
    : error(nullptr) {}

FrameContext::FrameContext(const FrameContext &other)
    : error((other.error != nullptr) ? other.error->clone() : nullptr)
    , handler(other.handler) {}

FrameContext &FrameContext::operator=(const FrameContext &other) {
    if (this != &other) {
        if (error != nullptr) {
            error->destroy();
            delete error;
        }
        error   = (other.error != nullptr) ? other.error->clone() : nullptr;
        handler = other.handler;
    }
    return *this;
}

FrameContext::FrameContext(FrameContext &&other) noexcept
    : error(other.error)
    , handler(std::Memory::move(other.handler)) {
    other.error = nullptr;
}

FrameContext &FrameContext::operator=(FrameContext &&other) noexcept {
    if (this != &other) {
        if (error != nullptr) {
            error->destroy();
            delete error;
        }
        error       = other.error;
        handler     = std::Memory::move(other.handler);
        other.error = nullptr;
    }
    return *this;
}

template <typename T>
FrameContext::FrameContext(T *obj)
    : error(std::erase_type(obj)) {
    if constexpr (!Panic::Interface::Panicking<T>) {
        static_assert(Panic::Interface::Panicking<T>,
                      "Frame invoked with an object that does not have a panic method, add "
                      "`class ... impl Panic::Interface::Panicking` "
                      "to the definition, and implement 'fn op panic (self) -> string' or the "
                      "static variant.");
    }

    handler = &FrameContext::throw_object<T>;
}

bool FrameContext::operator!=(const libcxx::type_info *rhs) const { return !(*this == rhs); }

bool FrameContext::operator==(const libcxx::type_info *rhs) const {
    if (this->error != nullptr) {
        return this->error->type_info() == rhs;
    }

    return false;
}
}  // namespace Panic

H_STD_NAMESPACE_END
H_NAMESPACE_END

#endif  // _$_HX_CORE_M17PANICFRAMECONTEXT