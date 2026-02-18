## libcxx::*:
- Cross-Platform C++ Standard Library Headers:
  - <sys/utime.h>
  - <sys/locking.h>
  - <conio.h>
  - <corecrt.h>
  - <corecrt_io.h>
  - <corecrt_malloc.h>
  - <corecrt_math.h>
  - <corecrt_math_defines.h>
  - <corecrt_memcpy_s.h>
  - <corecrt_memory.h>
  - <corecrt_search.h>
  - <corecrt_share.h>
  - <corecrt_startup.h>
  - <corecrt_stdio_config.h>
  - <corecrt_terminate.h>
  - <corecrt_wconio.h>
  - <corecrt_wctype.h>
  - <corecrt_wdirect.h>
  - <corecrt_wio.h>
  - <corecrt_wprocess.h>
  - <corecrt_wstdio.h>
  - <corecrt_wstdlib.h>
  - <corecrt_wstring.h>
  - <corecrt_wtime.h>
  - <crtdbg.h>
  - <direct.h>
  - <dos.h>
  - <mbctype.h>
  - <io.h>
  - <fpieee.h>
  - <malloc.h>
  - <mbstring.h>
  - <minmax.h>
  - <new.h>
  - <process.h>
  - <safeint.h>
  - <safeint_internal.h>
  - <share.h>
  - <tchar.h>
  - <fcntl.h>
  - <sys/stat.h>
  - <sys/timeb.h>
  - <sys/types.h>
  - <memory.h>
  - <cstdlib>
  - <cstddef>
  - <cstdio>
  - <cassert>
  - <ccomplex>
  - <csignal>
  - <ctgmath>
  - <cmath>
  - <cerrno>
  - <cfenv>
  - <clocale>
  - <ctime>
  - <csetjmp>
  - <cstdarg>
  - <cuchar>
  - <cwchar>
  - <cstring>
  - <cwctype>
  - <cctype>
  - <cinttypes>
  - <cfloat>
  - <algorithm>
  - <array>
  - <atomic>
  - <bitset>
  - <chrono>
  - <complex>
  - <condition_variable>
  - <coroutine>
  - <deque>
  - <exception>
  - <fstream>
  - <functional>
  - <future>
  - <iomanip>
  - <ios>
  - <iosfwd>
  - <iostream>
  - <istream>
  - <iterator>
  - <limits>
  - <list>
  - <locale>
  - <codecvt>
  - <map>
  - <memory>
  - <mutex>
  - <numeric>
  - <optional>
  - <ostream>
  - <queue>
  - <random>
  - <ranges>
  - <regex>
  - <scoped_allocator>
  - <set>
  - <shared_mutex>
  - <span>
  - <sstream>
  - <stack>
  - <stdexcept>
  - <streambuf>
  - <string>
  - <string_view>
  - <system_error>
  - <thread>
  - <tuple>
  - <type_traits>
  - <typeindex>
  - <typeinfo>
  - <unordered_map>
  - <unordered_set>
  - <utility>
  - <valarray>
  - <variant>
  - <vector>

- Unix:
  - <unistd.h>
  - <cxxabi.h>
  - <pthread.h>
  - <sys/mman.h>
  - <sys/resource.h>
  - <unistd.h>
  - <cxxabi.h>
  - <fcntl.h>

- Windows:
  - <windows.h>
  - <processthreadsapi.h>
  - <tlhelp32.h>

## std::Interfaces::*
- SupportsPointerCast::<T> - dynamic cast requirement
- Castable::<From, To> - has `op as` operator or C++ implicit cast operator
- ConvertibleToString::<T> - has `fn op as (self) -> string` operator or supports C++ Ostream insertion or C++ implicit conversion to kairo::`string`
- RangeCompliant::<T> - has `op ++` and `op <` operators for range iteration
- SupportsOStream::<T> - has `op <<` operator for output streaming or c++ Ostream insertion operator that returns `libcxx::ostream&`
- ClassType::<T> - must be a class type
- ConstType::<T> - must be a const type
- ReferenceableType::<T> - must be referenceable
- RValueReference::<T> - must be an rvalue reference type
- LValueReference::<T> - must be an lvalue reference type
- ReferenceType::<T> - must be a reference type
- MoveConstructible::<T> - has move constructor
- NothrowAssignable::<T, Arg> - non-throwing assignment operator
- CopyConstructible::<T> - has copy constructor

## std::Meta::*
- const_removed::<T> - removes const qualifier from a type
- const_volatile_removed::<T> - removes both const and volatile qualifiers from a type
- cvref_removed::<T> - removes const, volatile, and reference qualifiers from a type
- as_const::<T> - converts a type to a const type
- as_const_volatile::<T> - converts a type to a const volatile type
- as_cvref::<T> - converts a type to a const volatile reference type
- declval::<T> - provides a type that can be used to simulate an rvalue of the specified type
- enable_if::<Condition, T> - enables a type if the condition is true
- true_t - represents a true boolean value
- false_t - represents a false boolean value
- ref_as_ptr::<T> - converts a reference type to a pointer type
- as_rvalue_reference::<T> - converts a type to an rvalue reference type
- as_lvalue_reference::<T> - converts a type to an lvalue reference type
- is_lval_reference::<T> - checks if a type is an lvalue reference type
- is_rval_reference ::<T> - checks if a type is an rvalue reference type
- is_reference::<T> - checks if a type is a reference type
- is_pointer::<T> - checks if a type is a pointer type
- is_referenceable::<T> - checks if a type is referenceable
- reference_removed::<T> - removes reference from a type
- is_convertible::<From, To> - checks if a type can be converted to another type
- same_as::<From, To>  checks if two types are the same
- is_convertible_to::<From, To> - checks if a type can be converted to another type
- is_same_as::<From, To> - checks if two types are the same
- is_const::<T> - checks if a type is const
- is_derived_of::<Base, Derived> - checks if a type is derived from another type
- is_class::<T> - checks if a type is a class type
- all_extents_removed::<T> - removes all extents from an array type
- can_move_noexcept::<T> - checks if a type can be moved without throwing exceptions
- can_assign_noexcept::<T, Arg> - checks if a type can be assigned without throwing exceptions
- can_copy_construct::<T> - checks if a type can be copy constructed
- wellformed_destructor::<T> - checks if a type has a well-formed destructor
- is_function::<T> - checks if a type is a function type
- is_destructible::<T> - checks if a type is destructible

## std::*
- as_cast::<To, From>(_: From) -> To - performs a safe cast from one type to another, use `T as U` syntax
- as_const::<To, From>(_: From) -> To - converts a type to a const type to another use `T as const U` syntax
- as_unsafe::<To, From>(_: From) -> To - performs an unsafe cast from one type to another, use `T as unsafe U` syntax
- Function::<ReturnType, Args...> - represents a function type with specified return type and argument types, constrcutor takes a function pointer or lambda
- Generator::<YieldType> - represents a generator type that yields values of the specified type, C++ coroutines are used in the implementation
- next(_: Generator::<T>) -> T - retrieves the next value from a generator
- endl(_: string | char) -> endl - represents a endline character for output streaming in print function call
- stringf(format: string, ...args) -> string - formats a string using the specified format and arguments, similar to `printf` in C
- crash::<T>(_: T) -> void - crashes the program with a specified Panicable object.
- null_t - represents a null value, used for null pointers or null values in containers (no argument constructor) no other methods
- TypeErasure - a type erasure mechanism that allows for type removel has the follwing methods: destroy() -> void, defrefence operator -> *void, type_info() -> libcxx::type_info, clone() -> *TypeErasure
- erase_type::<T>(_: *T) -> *TypeErasure - erases the type of an object and returns a TypeErasure object
- Range::<T> - represents a range of values of type `T`, has the following methods:
    ```kairo
    fn Range(self, first: T, last: T, step: isize = 1)
    fn Range(self, last: T)
    fn op in (self)[iter] -> yield T
    fn op in (self, value: T)[contains] -> bool
    ```
- range(last: T) -> Range::<T> - creates a range from 0 to last with step 1
- range(first: T, last: T, step: isize = 1) -> Range::<T> - creates a range from first to last with specified step
    ```cpp
    Range(T start, T end, T step = 1);
    Range(const Range &other) = default;
    Range(Range &&other) noexcept = default;
    Range &operator=(const Range &other) = default;
    Range &operator=(Range &&other) noexcept = default;

    T begin() const noexcept;
    T end() const noexcept;
    T step() const noexcept;

    bool contains(const T &value) const noexcept;
    bool is_empty() const noexcept;

    // Iteration support
    class iterator {
- Questionable::<T> - represents a type that can be nullable or panicable, has `op ?` to colapse the type to `T | null_T | Error` where `Error` is an Error type:
    ```cpp
    /// ------------------------------- Constructors (Null) -------------------------------
    $question() noexcept;
    $question(const std::null_t &) noexcept;
    $question(std::null_t &&) noexcept;
    /// ------------------------------- Constructors (Value) -------------------------------
    $question(const T &value);
    $question(T &&value);
    /// ------------------------------- Constructors (Error) -------------------------------
    $question(const std::Panic::Frame &error);
    $question(std::Panic::Frame &&error);
    /// --------------------------- Move Constructor & Assignment ---------------------------
    $question($question &&other) noexcept;
    $question &operator=($question &&other) noexcept;
    /// -------------------------- Copy Constructor & Assignment ----------------------------
    $question(const $question &other);
    $question &operator=(const $question &other);
    /// ------------------------------- Destructor -------------------------------
    ~$question();
    /// ------------------------------- Operators -------------------------------
    bool operator==(const std::null_t &) const noexcept;
    bool operator!=(const std::null_t &) const noexcept;
    template <typename E>
    bool operator==(const E &) const noexcept;
    template <typename E>
    bool operator!=(const E &other) const noexcept;
    template <typename E>
    bool operator$contains(const E &other) const noexcept; // use `E in Questionable<T>` syntax
    [[nodiscard]] bool operator$question() const noexcept; // use `Questionable<T> ?` syntax
    /// ------------------------------- Casting -------------------------------
    template <typename E>
        requires std::Panic::Interface::Panicking<E>
    E operator$cast(E * /*unused*/) const; // use `Questionable<T> as E` syntax
    T operator$cast(T * /*unused*/) const; // use `Questionable<T> as T` syntax
    [[nodiscard]] T &operator*();
    [[nodiscard]] operator T();
    ```
- inline string to_string(const char *t);
- inline string to_string(const wchar_t *t);
- inline string to_string(const string &t);
- inline string to_string(const nstring &t);
- inline string to_string(const libcxx::string &t);
- inline string to_string(const libcxx::wstring &t);
- inline string to_string(bool t);
- template <typename Ty> requires std::Interface::SupportsOStream<std::Meta::all_extents_removed<Ty>> string to_string(Ty &&t)
- template <typename Ty> requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, wchar_t *> inline string to_string(Ty &&t)
- template <typename Ty> requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, char *> inline string to_string(Ty &&t)
- template <typename Ty> requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, nstring> inline string to_string(Ty &&t)
- template <typename Ty> requires std::Interface::Castable<std::Meta::all_extents_removed<Ty>, string> inline string to_string(Ty &&t)
- template <typename Ty>
        requires(!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, const char *>) &&
                (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, const wchar_t *>) &&
                (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, string>) &&
                (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, nstring>) &&
                (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, libcxx::string>) &&
                (!std::Meta::is_convertible<std::Meta::all_extents_removed<Ty>, libcxx::wstring>) &&
                (!std::Meta::same_as<std::Meta::all_extents_removed<Ty>, bool>) &&
                (!std::Interface::SupportsOStream<std::Meta::all_extents_removed<Ty>>) &&
                (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, wchar_t *>) &&
                (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, char *>) &&
                (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, nstring>) &&
                (!std::Interface::Castable<std::Meta::all_extents_removed<Ty>, string>)
    inline string to_string(Ty &&t);
- inline char char_to_cchar(wchar_t wc) - converts a wide character to a char (c-char)
- inline string nstring_to_string(const nstring &cstr) - converts a nstring to a string (nstring being kairo::String::basic<char> - c++ char not wchar_t)
- inline string nptr_to_string(const char *cstr, size_t size)
- inline nstring string_to_nstring(const string &wstr)
- inline void wptr_to_nptr(const wchar_t *wstr, char *buffer, size_t buffer_size)


## std::String::*
- basic::<Char, Traits> - represents a basic string type with specified character type and traits, similar to `std::wbasic_string` in C++ has the following methods:
    ```cpp
    struct slice : public String::slice<CharT, Traits> {
        using String::slice<CharT, Traits>::slice;
    };

    using char_traits = Traits;
    using char_t      = CharT;
    using size_t      = usize;
    using string_t    = libcxx::basic_string<CharT, Traits>;
    using slice_t     = slice;
    using slice_vec   = vec<slice_t>;
    using char_vec    = vec<CharT>;

    static constexpr size_t npos = string_t::npos;
    // Constructors
    constexpr basic() noexcept;
    constexpr basic(const basic &other) noexcept;
    constexpr basic(basic &&other) noexcept;
    constexpr basic(const libcxx::basic_string<CharT, Traits> &str) noexcept;
    constexpr basic(const CharT *str) noexcept;
    constexpr basic(const CharT chr, size_t count) noexcept;
    constexpr basic(const CharT *str, size_t len) noexcept;
    constexpr basic(const slice_t &s) noexcept;
    template <typename U = CharT>
    basic(const char *str,
        typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;
    template <typename U = CharT>
    basic(const char *str,
        typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;
    // Assignment
    basic &operator=(const basic &other) noexcept = default;
    basic &operator=(basic &&other) noexcept;
    basic &operator=(const CharT *str) noexcept;
    basic &operator=(const slice_t &s) noexcept;
    // Access Operators
    CharT       &operator[](size_t index) noexcept;
    const CharT &operator[](size_t index) const noexcept;
    // Mutable Methods
    void push_back(CharT c) noexcept;
    void append(const basic &other) noexcept;
    void append(const CharT *str, size_t len) noexcept;
    void append(const slice_t &s) noexcept;
    void clear() noexcept;
    void replace(size_t pos, size_t len, const slice_t &other) noexcept;

    void reserve(size_t new_cap) noexcept;
    void resize(size_t new_size, CharT c = CharT()) noexcept;
    bool empty() const noexcept;
    // Concatenation Operators
    basic &operator+=(const basic &other) noexcept;
    basic &operator+=(const CharT *str) noexcept;
    basic &operator+=(const slice_t &s) noexcept;
    template <typename U = CharT>
        requires std::Meta::is_convertible_to<U, CharT>
    basic &operator+=(const U chr) noexcept;
    basic operator+(const basic &other) const;
    basic operator+(const CharT *str) const;
    basic operator+(const slice_t &s) const;
    template <typename U = CharT>
        requires std::Meta::is_convertible_to<U, CharT>
    basic operator+(const U chr) const;
    // Comparison Operators
    bool operator==(const basic &other) const noexcept;
    bool operator!=(const basic &other) const noexcept;
    bool operator<(const basic &other) const noexcept;
    bool operator>(const basic &other) const noexcept;
    bool operator<=(const basic &other) const noexcept;
    bool operator>=(const basic &other) const noexcept;
    // Basic Access
    const CharT *raw() const noexcept;
    size_t       size() const noexcept;
    size_t       length() const noexcept;
    bool         is_empty() const noexcept;
    string_t    &raw_string() noexcept;
    // Slice Conversion
    operator slice_t() const noexcept;
    slice_t operator$cast(const slice_t * /* p */) const noexcept;
    const char_t *operator$cast(const char_t * /* p */) const noexcept;
    // Copy-Returning Methods
    basic      subslice(size_t pos, size_t len) const noexcept;
    basic      l_strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    basic      r_strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    basic      strip(const char_vec &delim = {' ', '\t', '\n', '\r'}) const;
    vec<basic> split(const basic &delim, slice_t::Operation op = slice_t::Operation::Remove) const;
    vec<basic> split_lines() const;
    // Search
    bool starts_with(const basic &needle) const noexcept;
    bool starts_with(CharT c) const noexcept;
    bool starts_with(slice needle) const noexcept;

    bool ends_with(const basic &needle) const noexcept;
    bool ends_with(CharT c) const noexcept;
    bool ends_with(slice needle) const noexcept;
    bool contains(const basic &needle) const noexcept;
    bool contains(CharT c) const noexcept;
    constexpr bool operator$contains(slice needle) const;
    constexpr bool operator$contains(CharT chr) const;
    // add iterators for the string
    using iterator               = typename string_t::iterator;
    using const_iterator         = typename string_t::const_iterator;
    using reverse_iterator       = typename string_t::reverse_iterator;
    using const_reverse_iterator = typename string_t::const_reverse_iterator;
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator crend() const noexcept;
    std::Questionable<usize> lfind(slice needle) const;
    std::Questionable<usize> rfind(slice needle) const;
    std::Questionable<usize> find_first_of(slice needle) const;
    std::Questionable<usize> find_last_of(slice needle) const;
    std::Questionable<usize> find_first_not_of(slice needle) const;
    std::Questionable<usize> find_last_not_of(slice needle) const;
    std::Questionable<usize> lfind(const basic &needle, usize pos) const;
    std::Questionable<usize> rfind(slice needle, usize pos) const;
    std::Questionable<usize> find_first_of(slice needle, usize pos) const;
    std::Questionable<usize> find_last_of(slice needle, usize pos) const;
    std::Questionable<usize> find_first_not_of(slice needle, usize pos) const;
    std::Questionable<usize> find_last_not_of(slice needle, usize pos) const;
    ```
- slice::<Char, Traits> - represents a slice of a string with specified character type and traits, similar to `std::wstring_view` in C++ has the following methods:
    ```cpp
    private:
        using view_t = libcxx::basic_string_view<CharT>;

        usize  len;
        view_t data{};

    public:
        enum class Operation { Keep, Remove };

        using char_traits = Traits;
        using char_t      = CharT;
        using slice_t     = slice<CharT>;
        using size_t      = usize;
        using slice_vec   = vec<slice_t>;
        using char_vec    = vec<CharT>;

        [[nodiscard("size() returns the length of the slice's data, which is essential for bounds "
                    "checking and iteration; discarding it may lead to unsafe or undefined behavior")]]
        constexpr size_t size() const noexcept {
        }

        constexpr slice() noexcept                   = default;
        constexpr slice(const slice &other) noexcept = default;
        constexpr slice(slice &&other) noexcept      = default;
        slice(const CharT *str) noexcept;
        slice(const CharT *str, size_t size) noexcept;
        slice(view_t view) noexcept;
        slice(char_vec &vec) noexcept;
        slice(char_vec &&vec) noexcept;

        template <typename U = CharT>
        slice(const char *str,
            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

        template <typename U = CharT>
        slice(const char *str,
            typename libcxx::enable_if_t<!libcxx::is_same_v<U, char>> * = nullptr) noexcept;

        constexpr operator view_t() const noexcept { return data; }

        void exchange(slice &other) noexcept;
        void replace(slice &other) noexcept;
        void replace(CharT *str, usize size) noexcept;

        [[nodiscard("raw() returns a pointer to the slice's underlying data, essential for direct "
                    "access; ignoring it may discard critical information")]]
        constexpr const CharT *raw() const noexcept {
        }
        [[nodiscard("is_empty() indicates whether the slice has no data, crucial for control flow; "
                    "discarding it may lead to incorrect assumptions")]]
        constexpr bool is_empty() const noexcept {
        }
        [[nodiscard("subslice() creates a view into a portion of the slice, vital for safe substring "
                    "operations; ignoring it wastes the result")]]
        slice subslice(usize pos, usize len) const noexcept;

        slice l_strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;
        slice r_strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;
        slice strip(char_vec &delim = {' ', '\t', '\n', '\r'}) const;

        usize length() const noexcept { return len; }

        bool starts_with(slice &needle) const;
        bool ends_with(slice &needle) const;

        bool contains(slice &needle) const;
        bool contains(CharT &chr) const;

        bool operator==(const slice &other) const noexcept { return data == other.data; }
        bool operator!=(const slice &other) const noexcept { return data != other.data; }
        bool operator<(const slice &other) const noexcept { return data < other.data; }
        bool operator>(const slice &other) const noexcept { return data > other.data; }
        bool operator<=(const slice &other) const noexcept { return data <= other.data; }
        bool operator>=(const slice &other) const noexcept { return data >= other.data; }

        constexpr bool operator$contains(slice &needle) const { return contains(needle); }
        constexpr bool operator$contains(CharT &chr) const { return contains(chr); }

        isize compare(slice &other) const noexcept;

        [[nodiscard("split_lines() returns a vector of line views, necessary for line-based "
                    "processing; discarding it neglects the parsed structure")]]
        slice_vec split_lines() const;

        [[nodiscard("split() returns a vector of views, necessary for delimeter-based processing; "
                    "discarding it neglects the parsed structure")]]
        slice_vec split(slice &delim, Operation op = Operation::Remove) const;

        std::Questionable<usize> lfind(slice &needle) const;
        std::Questionable<usize> rfind(slice &needle) const;
        std::Questionable<usize> find_first_of(slice &needle) const;
        std::Questionable<usize> find_last_of(slice &needle) const;
        std::Questionable<usize> find_first_not_of(slice &needle) const;
        std::Questionable<usize> find_last_not_of(slice &needle) const;

        CharT operator[](usize index) const noexcept;
    ```

- template <typename T> constexpr inline T *concat(T *dest, const T *src) noexcept
- template <typename T> constexpr inline T *concat_n(T *dest, const T *src, usize n) noexcept
- template <typename T> constexpr inline T *copy(T *dest, const T *src) noexcept
- template <typename T> constexpr inline T *copy_n(T *dest, const T *src, usize n) noexcept
- template <typename T> constexpr inline std::Questionable<T *> find(const T *str, int c) noexcept
- template <typename T> constexpr inline std::Questionable<T *> find_last(const T *str, int c) noexcept
- template <typename T> constexpr inline std::Questionable<T *> find_any(const T *str, const T *accept) noexcept
- template <typename T> constexpr inline std::Questionable<T *> find_sub(const T *haystack, const T *needle) noexcept
- template <typename T> constexpr inline vec<T *> split(T *str, const T *delim) noexcept
- template <typename T> constexpr inline usize length(const T *str) noexcept
- template <typename T> constexpr inline usize prefix_length(const T *str, const T *chars, bool exclude = false) noexcept
- template <typename T> constexpr inline int compare(const T *a, const T *b) noexcept
- template <typename T> constexpr inline int compare_n(const T *a, const T *b, usize n) noexcept
- template <typename T> constexpr inline int compare_locale(const T *a, const T *b) noexcept
- template <typename T> constexpr inline usize transform(T *dest, const T *src, usize n) noexcept
- inline const char *error(int errnum) noexcept



## std::Memory::*
- enum AddressType (ROTData, Stack, Heap, Unknown) - type of memeory address for any pointer
- address_type(ptr: *void) -> AddressType - returns the type of memory address for a pointer
- new_aligned::<type>(...args) -> *type - allocates memory for a type with specified alignment (aligignemnt of T) and arguments
- delete_aligned(ptr: *T) -> void - deallocates memory for a type allocated with `new_aligned`
- type buffer::<T, N> == T[N] - creates a fixed-size buffer of type `T` with size `N`, same safety as C arrays (not very safe, use with caution)
- as_reference::<T>(_: *T) -> &&T - converts a type to an lvalue reference type
- as_pointer::<T>(_: &&T) -> *T - converts a type to a pointer type
- exchange::<T>(ptr: *T, value: T) -> &&T - atomically exchanges the value at the pointer with the specified value, returns the old value
- forward::<T>(value: T) -> T - forwards a value as an rvalue reference, used for perfect forwarding in function templates
- heap_start() -> void* - returns the start address of the heap memory region
- copy(dest: *void, src: *const void, size: usize) -> *void- copies memory from source to destination
- move(dest: *void, src: *const void, size: usize) -> *void- moves memory from source to destination
- set(dest: *void, value: i32, size: usize) -> *void - sets a block of memory to a specified value
- find(ptr: *const void, value: i32, size: usize) -> *T? - finds the first occurrence of a value in a block of memory
- compare(ptr1: *const void, ptr2: *const void, size: usize) i32 - compares two blocks of memory for equality
- in_rotdata(ptr: *const void) -> bool - checks if a pointer is in the ROTData memory region
- stack_size() -> usize - returns the size of the stack memory region
- stack_start() -> *void - returns the start address of the stack memory region
- stack_bounds() -> libcxx::pair(start: *void, end: *void) - returns the bounds of the stack memory region as a pair of pointers
- in_stack(ptr: *const void) -> bool - checks if a pointer is in the stack memory region

# std::Panic::*
- FrameContext - provides context for a panic frame, contains crach(), object() -> void*, type_name() -> string, also has equality operator that takes a libcxx::type_info
- Frame - represents a panic frame, contains:
    ```cpp
    template <typename T>
    Frame(T obj, const char *filename, usize lineno);

    template <typename T>
    Frame(T obj, string filename, usize lineno);

    constexpr Frame() = delete;
    ~Frame()          = default;

    Frame(const Frame &)            = default;
    Frame &operator=(const Frame &) = default;

    Frame(Frame &&other) noexcept            = default;
    Frame &operator=(Frame &&other) noexcept = default;

    [[noreturn]] Frame(Frame &obj, const string &, usize);
    [[noreturn]] Frame(Frame &&obj, const string &, usize);

    [[nodiscard]] string        file() const;
    [[nodiscard]] usize         line() const;
    [[nodiscard]] string        reason() const;
    [[nodiscard]] FrameContext *get_context() const;

    [[noreturn]] void operator$panic() const; // used in kairo code as `panic ...` <...> would be the object that contains a operator `panic`
    // panic operator can be defiend by implementing `fn op panic(self) -> Frame`
    ```

# std::Panic::Interface::*
- Panicking::<T> - interface for types that can panic, must implement `fn op panic(self) -> Frame` operator
- PanickingStatic::<T> - interface for types that can panic, must implement `static fn op panic() -> Frame` operator, but is static
- PanickingInstance::<T> - interface for types that can panic, must implement `fn op panic(self) -> Frame` operator, but is instance based

# * (global functions)
- macro reference(T) - creates a reference to a type, same as `&T` in C++ (can be used as a type like `_: refrence(i32)`)
- macro move_reference(T) - creates an rvalue reference to a type, same as `&&T` in C++ (can be used as a type like `_: move_reference(i32)`)
- print(...args) -> void - prints formatted output to the console or a string, supports multiple argument types all args must be castable to `string` or `char` and can be used with `endl` for custom endlines

## std::Legacy::*
- new::<T>(...args) - allocates memory for a type and calls its constructor with the specified arguments (equivalent to `new T(args...)`)

## std::Error::*
- BaseError - Base Class for all Kairo errors (must implement `op as` and `op panic` operators aside from constructors)
- NullValueError
- RuntimeError
- StateMismatchError
- TypeMismatchError


## lang features:
`finally {}` finally block syntax
`T as <spec?> U` cast syntax
`fn (...) -> T` lambda syntax
`T?` nullable/panicable type syntax

## built-in types:
- `string` - represents a string type, compatiable with `std::wstring` in C++
- `char` - represents a character type directly compatible with `wchar_t` in C++
- `usize` - represents an unsigned size type, equivalent to `size_t` in C++
- `isize` - represents a signed size type, equivalent to `ssize_t` in C++
- `void` - represents a void type, equivalent to `void` in C++
- `bool` - represents a boolean type, equivalent to `bool` in C++
- `i8` - `signed char`
- `i16` - `signed short`
- `i32` - `signed int`
- `i64` - `signed long long`
- `u8` - `unsigned char`
- `u16` - `unsigned short`
- `u32` - `unsigned int`
- `u64` - `unsigned long long`
- `f32` - `float`
- `f64` - `double`
- `f80` - `long double`
- `null` - a static instance of `kairo::std::null_t` type, used for null values
- `vec::<T>` - represents a dynamic array type, alias for `std::vector<T>`
- `map::<Key, Value, Compare = libcxx::less::<Key>, Alloc = libcxx::allocator::<std::pair::<const Key, Value>>>` - represents a map type, alias for `std::map<Key, Value, Compare, Alloc>`
- `array::<T, N>` - represents a fixed-size array type, equivalent to `std::array<T, N>`
- `list::<T>` - represents a doubly linked list type, equivalent to `std::list<T>`
- `set::<T, Compare = libcxx::less::<T>, Alloc = libcxx::allocator::<T>>` - represents a set type, equivalent to `std::set<T, Compare, Alloc>`
- `tuple::<T...>` - represents a tuple type, equivalent to `std::tuple<T...>`