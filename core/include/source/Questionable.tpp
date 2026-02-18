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

#ifndef _$_HX_CORE_M12QUESTIONABLE_TPP
#define _$_HX_CORE_M12QUESTIONABLE_TPP

#include <include/config/config.hh>

#include <include/c++/libc++.hh>
#include <include/meta/meta.hh>
#include <include/runtime/runtime.hh>
#include <include/types/types.hh>

H_NAMESPACE_BEGIN

template <class T>
[[nodiscard]] bool $question<T>::is_null() const noexcept {
    return state == $State::Null;
}
template <class T>
[[nodiscard]] bool $question<T>::is_err() const noexcept {
    return state == $State::Error;
}
template <class T>
[[nodiscard]] bool $question<T>::is_err(const libcxx::type_info *type) const noexcept {
    return state == $State::Error && (*data.error.get_context()) == type;
}

template <class T>
void $question<T>::set_value(const T &value) {
    new (&data.value) T(value);
}
template <class T>
void $question<T>::set_value(T &&value) {
    new (&data.value) T(std::Memory::move(value));
}
template <class T>
void $question<T>::set_err(std::Panic::Frame &&error) {
    new (&data.error) std::Panic::Frame(std::Memory::move(error));
}

template <class T>
void $question<T>::set_err(std::Panic::Frame &error) {
    new (&data.error) std::Panic::Frame(error);
}

template <class T>
void $question<T>::delete_error() noexcept {
    data.error.~Frame();
}
template <class T>
void $question<T>::delete_value() noexcept {
    data.value.~T();
}

/// ------------------------------- Constructors (Null) -------------------------------
template <class T>
$question<T>::$question() noexcept
    : state($State::Null) {}
template <class T>
$question<T>::$question(const std::null_t &) noexcept
    : state($State::Null) {}
template <class T>
$question<T>::$question(std::null_t &&) noexcept
    : state($State::Null) {}

/// ------------------------------- Constructors (Value) -------------------------------
template <class T>
$question<T>::$question(const T &value)
    : state($State::Value) {
    set_value(value);
}
template <class T>
$question<T>::$question(T &&value)
    : state($State::Value) {
    set_value(std::Memory::move(value));
}

/// ------------------------------- Constructors (Error) -------------------------------
template <class T>
$question<T>::$question(const std::Panic::Frame &error)
    : state($State::Error) {
    set_err(error);
}
template <class T>
$question<T>::$question(std::Panic::Frame &&error)
    : state($State::Error) {
    set_err(std::Memory::move(error));
}

/// ------------------------------- Move Constructor & Assignment
/// -------------------------------
template <class T>
$question<T>::$question($question &&other) noexcept
    : state(other.state) {
    if (state == $State::Error) {
        set_err(std::Memory::move(other.data.error));
    } else if (state == $State::Value) {
        set_value(std::Memory::move(other.data.value));
    }
}

template <class T>
$question<T> &$question<T>::operator=($question &&other) noexcept {
    if (this != &other) {
        if (state == $State::Error) {
            delete_error();
        } else if (state == $State::Value) {
            delete_value();
        }

        state = other.state;
        if (state == $State::Error) {
            set_err(std::Memory::move(other.data.error));
        } else if (state == $State::Value) {
            set_value(std::Memory::move(other.data.value));
        }
    }
    return *this;
}

/// --------------------- Copy Constructor & Assignment ----------------------
template <class T>
$question<T>::$question(const $question &other)
    : state(other.state) {
    if (state == $State::Error) {
        set_err(other.data.error);
    } else if (state == $State::Value) {
        set_value(other.data.value);
    }
}

template <class T>
$question<T> &$question<T>::operator=(const $question &other) {
    if (this != &other) {
        if (state == $State::Error) {
            delete_error();
        } else if (state == $State::Value) {
            delete_value();
        }

        state = other.state;
        if (state == $State::Error) {
            set_err(other.data.error);
        } else if (state == $State::Value) {
            set_value(other.data.value);
        }
    }
    return *this;
}

/// ------------------------------- Destructor -------------------------------
template <class T>
$question<T>::~$question() {
    if (state == $State::Error) {
        delete_error();
    } else if (state == $State::Value) {
        if constexpr (std::Meta::is_destructible<T>) {
            delete_value();
        }
    }
}

/// ------------------------------- Operators -------------------------------
template <class T>
bool $question<T>::operator==(const std::null_t &) const noexcept {
    return is_null();
}
template <class T>
bool $question<T>::operator!=(const std::null_t &) const noexcept {
    return !is_null();
}

template <class T>
template <typename E>
bool $question<T>::operator==(const E &) const noexcept {
    if constexpr (std::Panic::Interface::Panicking<E>) {
        return is_err(&typeid(E));
    }

#ifdef _MSC_VER
    if constexpr (false) {
#endif
        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(
            std::Error::TypeMismatchError(L"Invalid: error type does not match any panic state."));

#ifdef _MSC_VER
    }
#endif
}

template <class T>
template <typename E>
bool $question<T>::operator!=(const E &other) const noexcept {
    return !(*this == other);
}

template <class T>
template <typename E>
bool $question<T>::operator$contains(const E &other) const noexcept {
    return *this == other;
}

template <class T>
[[nodiscard]] bool $question<T>::operator$question() const noexcept {
    return state == $State::Value;
}

/// ------------------------------- Casting -------------------------------
template <class T>
template <typename E>
    requires std::Panic::Interface::Panicking<E>
E $question<T>::operator$cast(E * /*unused*/) const {
    if (state == $State::Error) {
        if (is_err(&typeid(E))) {
            auto *obj = (*data.error.get_context()).object();
            if (obj) {
                return *reinterpret_cast<E *>(obj);
            }
            _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::NullValueError(
                string(L"Invalid Decay: error context object is null.")));
        }
        data.error.operator$panic();
    }

    if (state == $State::Value) {
        if constexpr (std::Meta::same_as<T, E>) {
            return data.value;
        }

        _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::TypeMismatchError(
            L"Invalid cast: value type does not match requested type."));
    }

    _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::NullValueError(L"Invalid Decay: value is null."));
}

template <class T>
T $question<T>::operator$cast(T * /*unused*/) const {
    if (state == $State::Value) {
        return data.value;
    }

    if (state == $State::Error) {
        data.error.operator$panic();
    }

    _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::NullValueError(L"Invalid Decay: value is null."));
}

///
/// \brief Dereference operator that returns a reference to the contained value.
///
/// This function provides access to the underlying value when the state
/// is \c $State::Value. If the state is \c $State::Error, the associated
/// error is propagated by invoking its panic handler. If neither a value
/// nor error is present, a fatal panic is triggered.
///
/// \tparam T The type of the contained value.
/// \return T& Reference to the contained value.
///
/// \warning The returned reference is a direct reference to internal state
///          and is not thread-safe. Concurrent access to the same object
///          from multiple threads results in undefined behavior. Use external
///          synchronization if thread safety is required.
///
/// \throws std::Error::NullValueError If the state does not contain a value
///         or error (invalid decay).
///
template <class T>
[[nodiscard]] T &$question<T>::operator*() {
    if (state == $State::Value) {
        return data.value;
    }

    if (state == $State::Error) {
        data.error.operator$panic();
    }

    _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::NullValueError(L"Invalid Decay: value is null."));
}

///
/// \brief Implicit conversion operator to the contained value.
///
/// This operator allows a \c $question<T> to be used in contexts where
/// a value of type \c T is expected. If the state is \c $State::Value,
/// the contained value is returned by copy. If the state is \c $State::Error,
/// the associated error's panic handler is invoked. If neither a value nor
/// error is present, a fatal panic is triggered.
///
/// \tparam T The type of the contained value.
/// \return T A copy of the contained value.
///
/// \warning This operator returns a copy rather than a reference.
///          Modifications to the returned object do not affect the internal
///          state of \c $question. No thread-safety is provided: accessing
///          the same object concurrently without synchronization results
///          in undefined behavior.
///
/// \throws std::Error::NullValueError If the state does not contain a value
///         or error (invalid decay).
///
template <class T>
[[nodiscard]] $question<T>::operator T() {
    if (state == $State::Value) {
        return data.value;
    }

    if (state == $State::Error) {
        data.error.operator$panic();
    }

    _HX_MC_Q7_INTERNAL_CRASH_PANIC_M(std::Error::NullValueError(L"Invalid Decay: value is null."));
}

H_NAMESPACE_END

#endif  // _$_HX_CORE_M12QUESTIONABLE