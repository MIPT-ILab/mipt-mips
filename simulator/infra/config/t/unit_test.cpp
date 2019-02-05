/**
 * Unit tests for configuration parser
 * @author Denis Los
 */

#include "../config.h"
#include "infra/argv.h"
#include "infra/macro.h"

#include <catch.hpp>

#include <sstream>

namespace config {
    AliasedRequiredValue<std::string> string_config = { "b", "string_config_name", "string config description"};
    AliasedRequiredValue<uint64> uint64_config = { "n", "uint64_config_name", "uint64 config description"};

    Switch bool_config_1 = { "bool_config_1", "first bool config description"};
    Switch bool_config_2 = { "bool_config_2", "second bool config description"};
} // namespace config

template<typename T>
std::string wrap_shift_operator(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<size_t N> // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
void handleArgs( const char* (& array)[N])
{
    CHECK( count_argc( argv_cast<N>( array)) == N - 1);
    config::handleArgs( N - 1, argv_cast<N>( array), 1);
}

//
// To check whether the returned values
// are equal to passed arguments
//
TEST_CASE( "config_parse: Pass_Valid_Args_1")
{
    const uint64 mandatory_int_value = 145;
    const std::string mandatory_string_value{ "file.elf"};

    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "file.elf",
        "-n", "145",
        "--bool_config_2",
        nullptr
    };

    CHECK_NOTHROW( handleArgs( argv) );

    CHECK( config::uint64_config == mandatory_int_value);
    CHECK( config::string_config == mandatory_string_value);
    CHECK_FALSE( config::bool_config_1);
    CHECK( config::bool_config_2);

    CHECK( wrap_shift_operator( config::uint64_config) == "145");
    CHECK( wrap_shift_operator( config::string_config) == mandatory_string_value);
    CHECK( wrap_shift_operator( config::bool_config_1) == "false");
    CHECK( wrap_shift_operator( config::bool_config_2) == "true");
}

//
// To check whether returned values
// are equal to passed arguments
//
TEST_CASE( "config_parse:  Pass_Valid_Args_2")
{
    const uint64 mandatory_int_value = 356;
    const std::string mandatory_string_value{ "run_test.elf"};

    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test.elf",
        "-n", "356",
        "--bool_config_1",
        nullptr
    };

    CHECK_NOTHROW( handleArgs( argv));

    CHECK( config::uint64_config == mandatory_int_value);
    CHECK( config::string_config == mandatory_string_value);
    CHECK( config::bool_config_1);
    CHECK_FALSE( config::bool_config_2);
    
    CHECK( wrap_shift_operator( config::uint64_config) == "356");
    CHECK( wrap_shift_operator( config::string_config) == mandatory_string_value);
    CHECK( wrap_shift_operator( config::bool_config_1) == "true");
    CHECK( wrap_shift_operator( config::bool_config_2) == "false");
}

//
// Pass no arguments
//
TEST_CASE( "config_parse: Pass_No_Args")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips", nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), std::exception);
}

//
// Pass arguments without a string_config_name option
//
TEST_CASE( "config_parse: Pass_Args_Without_Binary_Option")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "--uint64_config_name", "356",
        nullptr
    };
    
    CHECK_THROWS_AS( handleArgs( argv), std::exception);
}

//
// Pass arguments without a numsteps option
//
TEST_CASE( "config_parse:  Pass_Args_Without_Numsteps_Option")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "--string_config_name", "test.elf",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), std::exception);
}

//
// Pass arguments with unrecognised option
//
TEST_CASE( "config_parse: Pass_Args_With_Unrecognised_Option")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "--string_config_name", "test.elf",
        "-n", "356",
        "-koption",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), std::exception);
}

#if 0
//
// Pass a binary option multiple times
//
TEST_CASE( "config_parse:  Pass_Binary_Option_Multiple_Times")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test_1.elf",
        "--string_config_name", "run_test_2.elf",
        "-n", "412",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), std::exception);
}
#endif

//
// Pass a binary option without an argument
//
TEST_CASE( "config_parse:  Pass_Binary_Option_Without_Arg")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b",
        "-n", "412",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), std::exception);
}

//
// Pass a numsteps option without an argument
//
TEST_CASE( "config_parse:  Pass_Numsteps_Option_Without_Arg")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test",
        "-n",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), std::exception);
}

TEST_CASE( "config_parse: Pass help option alias")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test.elf",
        "-n", "356",
        "-h",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::HelpOption);
}

TEST_CASE( "config_parse: Pass help option")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test.elf",
        "-n", "356",
        "--help",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::HelpOption);
}

#if 0
//
// To check whether providing configuration parser
// with the same option is a failure
//
TEST_CASE( "config_provide_options: Provide_Config_Parser_With_Binary_Option_Twice")
{
    // Replace with std::make_array in C++20
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "test.elf",
        "-n", "100",
        nullptr
    };

    // should not throw any exceptions
    CHECK_NOTHROW( handleArgs( argv));

    auto test_function = []()
    {
        config::RequiredValue<std::string> second_binary_file_option =
            {
                "string_config_name,b",
                "input string_config_name file"
            };
    };

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( test_function(), ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}
#endif

