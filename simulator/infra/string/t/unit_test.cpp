/*
* This is an open source non-commercial project. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include "../cow_string.h"

TEST( Cow_String, Copy_Check)
{
    CowString a {"Hello"};
    CowString b = a;
    ASSERT_EQ( a, b);
    ASSERT_EQ( static_cast<std::string_view>(a).data(), static_cast<std::string_view>(b).data());

    b += " World!";
    ASSERT_EQ( a, "Hello");
    ASSERT_EQ( b, "Hello World!");
    ASSERT_NE( a, b);
    ASSERT_NE( static_cast<std::string_view>(a).data(), static_cast<std::string_view>(b).data());
}

TEST( Cow_String, Assignment_Check)
{
    CowString a {"Hello"};
    CowString b = a;
    ASSERT_EQ( a, b);
    ASSERT_EQ( static_cast<std::string_view>(a).data(), static_cast<std::string_view>(b).data());

    a = "Goodbye!";
    ASSERT_EQ( a, "Goodbye!");
    ASSERT_EQ( b, "Hello");
    ASSERT_NE( a, b);
    ASSERT_NE( static_cast<std::string_view>(a).data(), static_cast<std::string_view>(b).data());
}

TEST( Cow_String, Empty_String)
{
    CowString a;
    ASSERT_EQ( a, "");
    CowString b = a;
    ASSERT_EQ( b, "");

    a = "Hello!";
    ASSERT_EQ( a, "Hello!");
    ASSERT_EQ( b, "");
    ASSERT_NE( a, b);
    
    b = "Goodbye!";
    ASSERT_EQ( b, "Goodbye!");
    ASSERT_NE( a, b);

    CowString c;
    ASSERT_EQ( c, "");
    c += "World!";
    ASSERT_EQ( c, "World!");
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}
