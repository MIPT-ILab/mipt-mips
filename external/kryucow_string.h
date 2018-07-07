/**
 * kryucow_string.h - implementation of fixed-size COW string
 * Copyright 2018 Pavel Kryukov
 *
 * MIT License
 *
 * Copyright (c) 2018 Pavel I. Kryukov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef KRYUCOW_STRING_H
#define KRYUCOW_STRING_H

#include <memory>
#include <string>
#include <exception>
#include <type_traits>

#ifndef __has_include
#  error "KryuCowString requires __has_include macro"
#endif

// Support XCode without C++17
#if __has_include("string_view")
#  include <string_view>
#elif __has_include("experimental/string_view")
#  include <experimental/string_view>
#else
#  error "KryuCowString requires std::string_view"
#endif

// This class implements COW string with fixed maximum size.
// It is very useful if you have to do A LOT of string copying
// but modify them rarely.
// List of non-implemented features:
//   * add more std::string compatibility by demand
template<typename CharT, typename Traits = std::char_traits<CharT>>
class BasicKryuCowString { // ha-ha, what a pun
private:
#if __has_include("string_view")
    using View_t = std::basic_string_view<CharT, Traits>;
#else
    using View_t = std::expertimental::basic_string_view<CharT, Traits>;
#endif
    using String_t = std::basic_string<CharT, Traits>;

    // We don't want to initialize arena here, so suppress ClangTidy
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
    struct InternalString {
        using SizeType = std::make_unsigned_t<CharT>;
        static constexpr const size_t SIZE = 127;
        static_assert((1ull << (sizeof(SizeType) * 8)) > SIZE);

        SizeType size = 0;
        CharT arena[SIZE];

        auto get_string_view() const { return View_t( arena, size); }
    };

    // I would like to have a static string of cache line granularity (64B typically)
    static_assert(sizeof(InternalString) % 64 == 0);

    std::shared_ptr<InternalString> pointer = nullptr;

    // Allocate new memory
    void init() { pointer = std::make_shared<InternalString>(); }

    // Appends new pointer to any string
    void append( const CharT* ptr, size_t app_size)
    {
        // The string is empty. Need to allocate memory first.
        if (pointer.use_count() == 0)
            init();

        const auto my_size = pointer->size; // May be zero as well
        if ( my_size + app_size > InternalString::SIZE)
            throw std::length_error("KryuCowSting exceeded length\n");

        // The string has a full copy. That's the place 'copy-on-write' occurs
        if (pointer.use_count() > 1) {
            const auto old_pointer = pointer; // Keep a pointer to old pointer for a moment
            init(); // Allocate new memory
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Use pointer arithmetic
            std::copy( old_pointer->arena, old_pointer->arena + my_size, pointer->arena);
        }

        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic) Use pointer arithmetic
        std::copy( ptr, ptr + app_size, pointer->arena + my_size);
        pointer->size = my_size + app_size; // update size
    }

    // Append wrappers
    void append( const CharT* ptr) { append( ptr, Traits::length( ptr)); }

    template<typename T>
    void append( const T& v) { append( v.data(), v.length());   }

    // Get string view object
    // const std::string_view is fancy here as it may be
    // easily converted to std::string or const char*
    // and has a lot of useful operators already (==, != etc.)
    auto get_string_view() const noexcept {
        if (pointer.use_count() != 0)
            return pointer->get_string_view();

        return View_t(nullptr, 0);
    }

public:
    // std::basic_string typedefs
    using traits_type = Traits;
    using value_type  = CharT;
    // using allocator_type = Allocator;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    // using pointer;
    // using const_pointer;
    // using iterator;
    // using const_iterator;
    // using reverse_iterator;
    // using const_reverse_iterator

    // ctor
    BasicKryuCowString() = default;

    // ctor from any type: just append the value
    // to an empty string
    template<typename T>
    explicit BasicKryuCowString( const T& value) {
        append( value);
    }

    // assignment: clear existing string and
    // append the value to the empty string
    template<typename T>
    BasicKryuCowString& operator=( const T& value) {
        clear();
        append( value);
        return *this;
    }

    // appending: see the method above
    template<typename T>
    auto& operator+=( const T& value) {
        append( value);
        return *this;
    }

    // convert to std::string_view like std::string does
    explicit operator View_t() const noexcept {
        return get_string_view();
    }

    // capacity
    bool empty() const {
        return pointer.use_count() == 0 || pointer->size == 0;
    }

    size_t size() const noexcept {
        return pointer.use_count() == 0 ? 0 : pointer->size;
    }

    size_t length() const noexcept {
        return size();
    }

    constexpr size_type max_size() const noexcept {
        return InternalString::SIZE;
    }

    constexpr size_type capacity() const noexcept {
        return InternalString::SIZE;
    }

    // only CONSTANT get operators
    const CharT& operator[]( size_t value) const noexcept {
        return pointer->arena[value];
    }

    const CharT& at( size_t value) const {
        if ( empty() || value > pointer->size)
            throw std::out_of_range("KryuCowString index is out of range\n");
        return operator[](value);
    }

    const CharT& front() const noexcept {
        return operator[](0);
    }

    const CharT& back() const noexcept {
        return operator[](size() - 1);
    }

    const CharT* data() const noexcept {
        return pointer.use_count() == 0 ? nullptr : pointer->arena;
    }

    const CharT* c_str() const noexcept {
        return data();
    }

    // Equality comparators for obvious types
    // Actually I would like to generalize it with some conversion
    // of RHS types to std::string_view
    bool operator==( const BasicKryuCowString& rhs) const noexcept { return get_string_view() == rhs.get_string_view(); }
    bool operator==( const String_t& rhs) const noexcept { return get_string_view() == static_cast<decltype(get_string_view())>(rhs); }
    bool operator==( const View_t& rhs) const noexcept { return get_string_view() == rhs; }
    bool operator==( const CharT* rhs) const { return get_string_view() == View_t( rhs, Traits::length(rhs)); }

    // Generate inequality comparator
    template<typename T>
    bool operator!=( const T& rhs) const { return !(*this == rhs); }

    // Makes a string empty
    void clear() {
        if (pointer.use_count() > 1) {
            // Someone else uses the pointer, so we
            // must allocate a new pointer block
            // Let's do it on demand and keep
            // nullptr
            pointer = nullptr;
        }
        else if (pointer.use_count() == 1) {
            // We own the pointer, so we may clear it
            // and do not allocate new pointer block
            pointer->size = 0;
        }
    }

    // Dump operator. Very nice that we may reuse std::string_view here.
    friend auto operator<<( std::basic_ostream<CharT, Traits>& out, const BasicKryuCowString& value) -> decltype(out) {
        return out << value.get_string_view();
    }
};

// LHS comparators.
// Some SFINAE here as we don't want to generate
// operator==(const KryuCowString&, const KryuCowString&),
// explicit operator must be use instead
template<typename T, typename CharT, typename Traits,
         typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, BasicKryuCowString<CharT, Traits>>>>
bool operator==( const T& lhs, const BasicKryuCowString<CharT, Traits>& rhs) { return rhs == lhs; }

template<typename T, typename CharT, typename Traits,
         typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, BasicKryuCowString<CharT, Traits>>>>
bool operator!=( const T& lhs, const BasicKryuCowString<CharT, Traits>& rhs) { return rhs != lhs; }

// Typedefs for simple KryuCowString and wide-char version
using KryuCowString = BasicKryuCowString<char>;
using WKryuCowString = BasicKryuCowString<wchar_t>;

#endif // KRYUCOW_STRING_H

