#include <cstring>

// Google Test library
#include <gtest/gtest.h>

// uArchSim modules
#include "../cow_string.h"

TEST( Cow_String, Equality)
{
    const CowString a ("Something");
    std::string a_s = "Something_strange";
    ASSERT_NE( a, a_s);
    a_s.erase( a_s.find('_'));
    ASSERT_EQ( a, a_s); 

    const std::string_view a_v = "Something_else";
    ASSERT_NE( a, a_v);
    const auto a_v1 = a_v.substr(0, 9);
    ASSERT_EQ( a, a_v1);
}

TEST( Cow_String, Copy_Check)
{
    CowString a {"Hello"};
    CowString b = a;
    ASSERT_EQ( a, b);
    ASSERT_EQ( a.data(), b.data());

    b += " World!";
    ASSERT_EQ( a, "Hello");
    ASSERT_NE( b, "Hello");
    ASSERT_EQ( b, "Hello World!");
    ASSERT_NE( a, b);
    ASSERT_NE( a.data(), b.data());
}

TEST( Cow_String, Assignment_Check)
{
    CowString a {"Hello"};
    const CowString b = a;
    ASSERT_EQ( a, b);
    ASSERT_EQ( a.data(), b.data());

    a = "Goodbye!";
    ASSERT_EQ( a, "Goodbye!");
    ASSERT_EQ( b, "Hello");
    ASSERT_NE( a, b);
    ASSERT_NE( a.data(), b.data());
}

TEST( Cow_String, Empty_String)
{
    CowString a;
    ASSERT_EQ( a, "");
    ASSERT_TRUE( a.empty());
    CowString b = a;
    ASSERT_EQ( b, "");
    ASSERT_TRUE( b.empty());

    a = "Hello!";
    ASSERT_EQ( a, "Hello!");
    ASSERT_FALSE( a.empty());
    ASSERT_EQ( b, "");
    ASSERT_TRUE( b.empty());
    ASSERT_NE( a, b);

    a = "";
    ASSERT_EQ( a, "");
    ASSERT_TRUE( a.empty());

    b = "Goodbye!";
    ASSERT_EQ( b, "Goodbye!");
    ASSERT_NE( a, b);

    CowString c;
    const CowString d = c;
    ASSERT_EQ( c, "");
    ASSERT_TRUE( c.empty());
    c += "World!";
    ASSERT_EQ( c, "World!");
    ASSERT_EQ( d, "");
}

TEST( Wide_Cow_String, Initializing_Checks)
{
    WCowString a;
    const WCowString b = a;
    ASSERT_EQ( a, b);

    WCowString c( L"Hello!");
    ASSERT_EQ( c, L"Hello!");

    a += c;
    ASSERT_EQ( a, c);
    ASSERT_NE( a, b);
    ASSERT_EQ( a, L"Hello!");

    c += a;
    ASSERT_EQ( c, L"Hello!Hello!");
    ASSERT_EQ( a, L"Hello!");
}

TEST( Cow_String, At)
{
    const CowString a("Hello Goodbye!");
    ASSERT_EQ( a.length(), std::strlen("Hello Goodbye!"));
    ASSERT_EQ( a[0], 'H');
    ASSERT_EQ( a.front(), 'H');
    ASSERT_EQ( a.back(), '!');
    ASSERT_EQ( a.at(3), 'l');
    ASSERT_EQ( a[4], 'o');
}

int main( int argc, char* argv[])
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

