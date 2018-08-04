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
#endif

// This class implements COW string with fixed maximum size.
// It is very useful if you have to do A LOT of string copying
// but modify them rarely.
// List of non-implemented features:
//   * add more std::string compatibility by demand
template<typename CharT, typename Traits = std::char_traits<CharT>, typename Allocator = std::allocator<CharT>>
class BasicKryuCowString { // ha-ha, what a pun
private:
    using String = std::basic_string<CharT, Traits, Allocator>;

    std::shared_ptr<String> pointer = nullptr;

    bool allocated() const { return pointer.use_count() != 0; }
    bool owned() const { return pointer.use_count() == 1; }
    bool shared() const { return allocated() && !owned(); }
    
    template<typename ... Args>
    void assign(Args&& ... args)
    {
        if (owned()) // Do not reallocate memory
            *pointer = String(std::forward<Args>(args)...);
        else
            *this = BasicKryuCowString(std::forward<Args>(args)...);
    }
    
    // Gets a detouched instance of string
    // Modification of detouched instance does not affect other strings
    void detouch()
    {
        if (!allocated())
            *this = BasicKryuCowString(String());
        else if (shared()) // The string has a full copy. That's the place 'copy-on-write' occurs
            *this = BasicKryuCowString(*pointer);
    }

    static bool is_empty(const CharT* val) { return Traits::length( val) == 0; }
    
    template<typename T>
    static bool is_empty(const T& val) { return val.empty(); }

public:
    // std::basic_string typedefs
    using traits_type = Traits;
    using value_type  = CharT;
    using allocator_type = Allocator;
    using size_type       = typename String::size_type;
    using difference_type = typename String::difference_type;
    using reference       = value_type&;
    using const_reference = const value_type&;
    // using pointer;
    // using const_pointer;
    // using iterator;
    // using const_iterator;
    // using reverse_iterator;
    // using const_reverse_iterator

    BasicKryuCowString() = default; // Do nothing
    // Big rule of zero: everything is handled by std::shared_ptr

    // ctor from any type: just append the value
    // to an empty string
    template<typename ... Args>
    explicit BasicKryuCowString(Args&& ... args) {
        pointer = std::make_shared<String>(std::forward<Args>(args)...);
    }

    // assignment: clear existing string and
    // append the value to the empty string
    template<typename T>
    BasicKryuCowString& operator=(T&& value) {
        clear();
        assign(std::forward<T>(value));
        return *this;
    }

    // Appending: detouch current object and append internal string
    template<typename T,
             typename = typename std::enable_if<!std::is_same_v<std::remove_cv_t<std::remove_reference_t<T>>, BasicKryuCowString>>::type>
    auto& operator+=(T&& value) {
        detouch();
        *pointer += std::forward<T>(value);
        return *this;
    }

    auto& operator+=(const BasicKryuCowString& value) {
        if (value.allocated())
            return *this += *(value.pointer);
        return *this;
    }

#if __has_include("string_view")
    explicit operator std::basic_string_view<CharT, Traits>() const noexcept {
        return std::basic_string_view<CharT, Traits>( c_str(), size());
    }
#endif

    // capacity
    bool empty() const {
        return !allocated() || pointer->empty();
    }

    size_t size() const noexcept {
        return !allocated() ? 0 : pointer->size();
    }

    size_t length() const noexcept {
        return size();
    }

    size_type max_size() const noexcept {
        return pointer->max_size();
    }

    size_type capacity() const noexcept {
        return !allocated() ? 0 : pointer->capacity();
    }

    // only CONSTANT get operators
    const CharT& operator[]( size_t value) const noexcept {
        return (*pointer)[value];
    }

    const CharT& at( size_t value) const {
        if (!allocated())
            throw std::out_of_range("KryuCowString out of range");
        return pointer->at(value);
    }

    const CharT& front() const noexcept {
        return operator[](0);
    }

    const CharT& back() const noexcept {
        return operator[](size() - 1);
    }

    const CharT* data() const noexcept {
        return pointer->data();
    }

    const CharT* c_str() const noexcept {
        return data();
    }

    // Equality comparators for obvious types
    // Actually I would like to generalize it with some conversion
    // of RHS types to std::string_view
    bool operator==(const BasicKryuCowString& rhs) const noexcept {
        if (empty() || is_empty( rhs))
            return empty() == is_empty( rhs);

        return *pointer == *(rhs.pointer);
    }

    template<typename T>
    bool operator==(const T& rhs) const noexcept {
        if (empty() || is_empty( rhs))
            return empty() == is_empty( rhs);

        return *pointer == rhs;
    }

    // Generate inequality comparator
    template<typename T>
    bool operator!=(const T& rhs) const { return !(*this == rhs); }

    // Makes a string empty
    void clear() {
        if (shared()) {
            // Someone else uses the pointer, so we
            // must allocate a new pointer block
            // Let's do it on demand and keep
            // nullptr
            pointer = nullptr;
        }
        else if (owned()) {
            // We own the pointer, so we may clear it
            // and do not allocate new pointer block
            pointer->clear();
        }
    }

    // Dump operator. Very nice that we may reuse std::string_view here.
    friend auto operator<<(std::basic_ostream<CharT, Traits>& out, const BasicKryuCowString& value) -> decltype(out) {
        if (!value.allocated())
            return out;

        return out << *(value.pointer);
    }
};

// LHS comparators.
// Some SFINAE here as we don't want to generate
// operator==(const KryuCowString&, const KryuCowString&),
// explicit operator must be use instead
template<typename T, typename CharT, typename Traits,
         typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, BasicKryuCowString<CharT, Traits>>>>
bool operator==(const T& lhs, const BasicKryuCowString<CharT, Traits>& rhs) { return rhs == lhs; }

template<typename T, typename CharT, typename Traits,
         typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, BasicKryuCowString<CharT, Traits>>>>
bool operator!=(const T& lhs, const BasicKryuCowString<CharT, Traits>& rhs) { return rhs != lhs; }

// Typedefs for simple KryuCowString and wide-char version
using KryuCowString = BasicKryuCowString<char>;
using WKryuCowString = BasicKryuCowString<wchar_t>;

#endif // KRYUCOW_STRING_H
