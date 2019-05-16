/**
 * string_view.h - definition of string_view for old libc++
 * @author Pavel Kryukov
 * Copyright 2017-2019 MIPT-MIPS team
 */

#ifndef STRING_VIEW_H
#define STRING_VIEW_H

#include <iostream>

#if __has_include("string_view")
#include <string_view>
#else
#include <experimental/string_view>
namespace std {
    using string_view = std::experimental::string_view;

    template<typename CharT, typename Traits = std::char_traits<CharT>>
    using basic_string_view = std::experimental::basic_string_view<CharT, Traits>;

    std::ostream& operator<<(std::ostream&, const std::experimental::string_view&);
}
#endif

#endif // STRING_VIEW_H
