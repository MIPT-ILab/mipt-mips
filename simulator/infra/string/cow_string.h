/**
 * kryucow_string.h - implementation of fixed-size COW string
 * @author Pavel Kryukov
 * Copyright 2017 MIPT-MIPS team
 */

#ifndef COW_STRING_H
#define COW_STRING_H

#include <cassert>
#include <cstring>

#include <memory>
#include <string>

#include <infra/string/string_view.h>

// This class implements COW string with fixed maximum size
// It is very useful if you have to do A LOT of string copying
// but modify them rarely.
// List of non-implemented features:
//   * generalize with CharT
//   * add proper error handling
//   * make resizeable?
//   * add std::string compatibility by demand
class CowString { // KryuCowString, ha-ha, what a pun
    struct InternalString {
        static constexpr const size_t SIZE = 63;
        char arena[SIZE] = "";
        int8_t size = 0;
        const std::string_view get_string_view() const { return std::string_view( arena, size); }
    };

    // I would like to have a static string of cache line size (64B typically)
    static_assert(sizeof(InternalString) == 64);

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
    void append( const char* ptr, size_t app_size)
    {
        // The string is empty. Need to allocate memory first.
        if (data.use_count() == 0)
            init();

        const auto my_size = data->size; // May be zero as well
        assert( my_size + app_size <= InternalString::SIZE);

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
    void append( const char* ptr)           { append( ptr, std::strlen( ptr)); }
    void append( const std::string_view& v) { append( v.data(), v.length());   }
    void append( const std::string& v)      { append( static_cast<std::string_view>(v)); }
    void append( const CowString& v)        { append( static_cast<std::string_view>(v)); }

    // Get string view object
    // const std::string_view is fancy here as it may be
    // easily converted to std::string or const char*
    // and has a lot of useful operators already (==, != etc.)
    const std::string_view get_string_view() const {
        return data->get_string_view();
    }

public:
    // ctor
    CowString() = default;

    // ctor from any type: just append the value
    // to an empty string
    template<typename T>
    explicit CowString( const T& value) {
        append( value);
    }

    // assignment: clear existing string and
    // append the value to the empty string
    template<typename T>
    CowString& operator=( const T& value) {
        clear();
        append( value);
        return *this;
    }

    // appending: see the method above
    template<typename T>
    CowString& operator+=( const T& value) {
        append( value);
        return *this;
    }

    // convert to std::string_view like std::string does
    explicit operator const std::string_view() const {
        return get_string_view();
    }

    // Equality comparators for obvious types
    // Actually I would like to generalize it with some conversion
    // of RHS types to std::string_view
    bool operator==( const CowString& rhs) const { return get_string_view() == rhs.get_string_view(); }
    bool operator==( const std::string& rhs) const { return get_string_view() == static_cast<std::string_view>(rhs); }
    bool operator==( const char* rhs) const { return get_string_view() == std::string_view( rhs, std::strlen(rhs)); }

    // Generate inequality comparator
    template<typename T>
    bool operator!=( const T& rhs) const { return !(*this == rhs); }
};

// Dump operator. Very nice that we may reuse std::string_view here.
static inline std::ostream& operator<<( std::ostream& out, const CowString& value) {
    return out << static_cast<std::string_view>(value);
}

// LHS comparators.
// Some SFINAE here as we don't want to generate 
// operator==(const CowString&, const CowString&),
// explicit operator must be use instead
template<typename T, typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, CowString>>>
bool operator==( const T& lhs, const CowString& rhs) { return rhs == lhs; }

template<typename T, typename = typename std::enable_if_t<!std::is_same_v<std::remove_reference_t<T>, CowString>>>
bool operator!=( const T& lhs, const CowString& rhs) { return rhs != lhs; }

#endif // COW_STRING_H

