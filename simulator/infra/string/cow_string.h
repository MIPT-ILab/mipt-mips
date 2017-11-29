/**
 * kryucow_string.h - implementation of fixed-size COW string
 * @author Pavel Kryukov
 * Copyright 2017 MIPT-MIPS team
 */

#ifndef COW_STRING_H
#define COW_STRING_H

#include <cassert>
#include <memory>
#include <string>
#include <exception>

#include <infra/string/string_view.h>

// This class implements COW string with fixed maximum size
// It is very useful if you have to do A LOT of string copying
// but modify them rarely.
// List of non-implemented features:
//   * make resizeable?
//   * add std::string compatibility by demand

class BasicCowStringOverflowException : public std::exception {
public:
    const char * what() const noexcept final
    {
        return "Limit of CowString size is reached\n";
    }
};

template<typename CharT, typename Traits = std::char_traits<CharT>>
class BasicCowString { // KryuCowString, ha-ha, what a pun
private:
    using View_t = std::basic_string_view<CharT, Traits>;
    using String_t = std::basic_string<CharT, Traits>;

    struct InternalString { // NOLINT Clang-Tidy doesn't like uninitalized array here
        static constexpr const size_t SIZE = 63;
        CharT size = 0;
        CharT arena[SIZE];
        auto get_string_view() const { return View_t( arena, size); }
    };

    // I would like to have a static string of cache line granularity (64B typically)
    static_assert(sizeof(InternalString) % 64 == 0);

    std::shared_ptr<InternalString> data = nullptr;

    // Allocate new memory
    void init() { data = std::make_shared<InternalString>(); }

    // Makes a string empty
    void clear() {
        if (data.use_count() > 1) {
            // Someone else uses the data, so we
            // must allocate a new data block
            // Let's do it on demand and keep
            // nullptr
            data = nullptr;
        }
        else if (data.use_count() == 1) {
            // We own the data, so we may clear it
            // and do not allocate new data block
            data->size = 0;
        }
    }

    // Appends new data to any string
    void append( const CharT* ptr, size_t app_size)
    {
        // The string is empty. Need to allocate memory first.
        if (data.use_count() == 0)
            init();

        const auto my_size = data->size; // May be zero as well
        if ( my_size + app_size > InternalString::SIZE)
            throw BasicCowStringOverflowException();

        // The string has a full copy. That's the place 'copy-on-write' occurs
        if (data.use_count() > 1) {
            const auto old_data = data; // Keep a pointer to old data for a moment
            init(); // Allocate new memory
            std::copy( old_data->arena, old_data->arena + my_size, data->arena); // NOLINT deep copy
        }

        std::copy( ptr, ptr + app_size, data->arena + my_size); // NOLINT copy new data
        data->size = my_size + app_size; // update size
    }

    // Append wrappers
    void append( const CharT* ptr)        { append( ptr, Traits::length( ptr)); }
    void append( const View_t& v)         { append( v.data(), v.length());   }
    void append( const String_t& v)       { append( static_cast<View_t>(v)); }
    void append( const BasicCowString& v) { append( static_cast<View_t>(v)); }

public:
    // ctor
    BasicCowString() = default;

    // ctor from any type: just append the value
    // to an empty string
    template<typename T>
    explicit BasicCowString( const T& value) {
        append( value);
    }

    // assignment: clear existing string and
    // append the value to the empty string
    template<typename T>
    BasicCowString& operator=( const T& value) {
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

    // Get string view object
    // const std::string_view is fancy here as it may be
    // easily converted to std::string or const char*
    // and has a lot of useful operators already (==, != etc.)
    auto get_string_view() const noexcept {
        if (data.use_count() != 0)
            return data->get_string_view();

        return View_t(nullptr, 0);
    }

    // convert to std::string_view like std::string does
    explicit operator View_t() const noexcept {
        return get_string_view();
    }

    // Equality comparators for obvious types
    // Actually I would like to generalize it with some conversion
    // of RHS types to std::string_view
    bool operator==( const BasicCowString& rhs) const noexcept { return get_string_view() == rhs.get_string_view(); }
    bool operator==( const String_t& rhs) const noexcept { return get_string_view() == static_cast<decltype(get_string_view())>(rhs); }
    bool operator==( const CharT* rhs) const { return get_string_view() == View_t( rhs, Traits::length(rhs)); }

    // Generate inequality comparator
    template<typename T>
    bool operator!=( const T& rhs) const { return !(*this == rhs); }
};

// Dump operator. Very nice that we may reuse std::string_view here.
template<typename CharT, typename Traits = std::char_traits<CharT>>
static inline auto operator<<( std::basic_ostream<CharT, Traits>& out, const BasicCowString<CharT, Traits>& value) -> decltype(out) {
    return out << static_cast<decltype(value.get_string_view())>(value);
}

// LHS comparators.
// Some SFINAE here as we don't want to generate
// operator==(const CowString&, const CowString&),
// explicit operator must be use instead
template<typename T, typename CharT, typename Traits,
         typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, BasicCowString<CharT, Traits>>>>
bool operator==( const T& lhs, const BasicCowString<CharT, Traits>& rhs) { return rhs == lhs; }

template<typename T, typename CharT, typename Traits,
         typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, BasicCowString<CharT, Traits>>>>
bool operator!=( const T& lhs, const BasicCowString<CharT, Traits>& rhs) { return rhs != lhs; }

// Typedefs for simple CowString and wide-char version
using CowString = BasicCowString<char>;
using WCowString = BasicCowString<wchar_t>;

#endif // COW_STRING_H

