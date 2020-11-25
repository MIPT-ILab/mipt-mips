/**
 * Unit tests for configuration parser
 * @author Denis Los
 */

#include <catch.hpp>

#include <infra/argv.h>
#include <infra/config/config.h>
#include <infra/config/main_wrapper.h>
#include <infra/log.h>
#include <infra/macro.h>

#include <iostream>
#include <sstream>

namespace config {
    const AliasedRequiredValue<std::string> string_config = { "b", "string_config_name", "string config description"};
    const AliasedRequiredValue<uint64> uint64_config = { "n", "uint64_config_name", "uint64 config description"};
    const PredicatedValue<uint64> uint64_predicated_config = { "uint64_predicated_config_name", 8, "uint64 config description",
                                                               [](uint64 val){ return val % 4 == 0; }};

    const Switch bool_config_1 = { "bool_config_1", "first bool config description"};
    const Switch bool_config_2 = { "bool_config_2", "second bool config description"};
} // namespace config

template<typename T>
std::string wrap_shift_operator(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return std::move( oss).str();
}

static void handleArgs( const std::vector<const char*>& argv)
{
    config::handleArgs( count_argc( argv.data()), argv.data(), 1);
}

//
// To check whether the returned values
// are equal to passed arguments
//
TEST_CASE( "config_parse: Pass_Valid_Args_1")
{
    const uint64 mandatory_int_value = 145;
    const std::string mandatory_string_value{ "file.elf"};

    std::vector<const char*> argv
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
    CHECK( config::string_config != "another_file.elf");
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

    std::vector<const char*> argv
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
    std::vector<const char*> argv
    {
        "mipt-mips", nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

//
// Pass arguments without a string_config_name option
//
TEST_CASE( "config_parse: Pass_Args_Without_Binary_Option")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "--uint64_config_name", "356",
        nullptr
    };
    
    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

//
// Pass arguments without a numsteps option
//
TEST_CASE( "config_parse:  Pass_Args_Without_Numsteps_Option")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "--string_config_name", "test.elf",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

//
// Pass arguments with unrecognised option
//
TEST_CASE( "config_parse: Pass_Args_With_Unrecognised_Option")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "--string_config_name", "test.elf",
        "-n", "356",
        "-koption",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

TEST_CASE( "config_parse:  Pass_Out_Of_Range")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "-n", "234",
        "--uint64_predicated_config_name", "9",
        "--string_config_name", "test.elf",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

TEST_CASE( "config_parse:  All_Correct")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "-n", "234",
        "--uint64_predicated_config_name", "128",
        "--string_config_name", "test.elf",
        nullptr
    };

    CHECK_NOTHROW( handleArgs( argv));
}

#if 0
//
// Pass a binary option multiple times
//
TEST_CASE( "config_parse:  Pass_Binary_Option_Multiple_Times")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "-b", "run_test_1.elf",
        "--string_config_name", "run_test_2.elf",
        "-n", "412",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}
#endif

//
// Pass a binary option without an argument
//
TEST_CASE( "config_parse:  Pass_Binary_Option_Without_Arg")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "-b",
        "-n", "412",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

//
// Pass a numsteps option without an argument
//
TEST_CASE( "config_parse:  Pass_Numsteps_Option_Without_Arg")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "-b", "run_test",
        "-n",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

TEST_CASE( "config_parse: Pass help option alias")
{
    std::vector<const char*> argv
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
    std::vector<const char*> argv
    {
        "mipt-mips",
        "-b", "run_test.elf",
        "-n", "356",
        "--help",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::HelpOption);
}

TEST_CASE( "config_parse: Pass help option and invalid option")
{
    std::vector<const char*> argv
    {
        "mipt-mips",
        "--help",
        nullptr
    };

    CHECK_THROWS_AS( handleArgs( argv), config::InvalidOption);
}

TEST_CASE( "config_parse: Pass duplicate option")
{
    std::ostringstream oss;

    // NOLINTNEXTLINE(readability-simplify-boolean-expr) https://bugs.llvm.org/show_bug.cgi?id=45507
    if (OStreamWrapper cerr_wrapper( std::cerr, oss); true)
        config::AliasedRequiredValue<uint64> { "n", "uint64_config_name", "uint64 config description"};

    CHECK( oss.str() == "Bad option setup for 'uint64_config_name' \nduplicate short option name '-n'\n");
}

#if 0
//
// To check whether providing configuration parser
// with the same option is a failure
//
TEST_CASE( "config_provide_options: Provide_Config_Parser_With_Binary_Option_Twice")
{
    std::vector<const char*> argv
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

static std::ostream& nullout()
{
    static std::ostream instance( nullptr);
    return instance;
}

TEST_CASE("Mainwrapper: default ctor")
{
    struct Main : public MainWrapper
    {
        Main() : MainWrapper( "Example Unit Test") { }
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        int impl( int /* argc */, const char* /* argv */ []) const final { return 0; }
    };
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
    const char* argv[1] = { nullptr };

    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay, hicpp-no-array-decay)
    CHECK( Main().run(0, argv) == 0);
}

TEST_CASE("MainWrapper: throw help")
{
    struct Main : public MainWrapper
    {
        Main() : MainWrapper( "Example Unit Test", nullout(), nullout()) { }
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        int impl( int /* argc */, const char* /* argv */ []) const final { throw config::HelpOption(); }
    };

    CHECK( Main().run( 0, nullptr) == 0);
}

TEST_CASE("MainWrapper: invalid option")
{
    struct Main : public MainWrapper
    {
        Main() : MainWrapper( "Example Unit Test", nullout(), nullout()) { }
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        int impl( int /* argc */, const char* /* argv */ []) const final { throw config::InvalidOption( "Help!"); }
    };

    CHECK( Main().run( 0, nullptr) == 4);
}

TEST_CASE("MainWrapper: throw exception")
{
    struct Main : public MainWrapper
    {
        Main() : MainWrapper( "Example Unit Test", nullout(), nullout()) { }
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        int impl( int /* argc */, const char* /* argv */ []) const final { throw Exception( "Exception"); }
    };

    CHECK( Main().run( 0, nullptr) == 2);
}

TEST_CASE("MainWrapper: throw std exception")
{
    struct Main : public MainWrapper
    {
        Main() : MainWrapper( "Example Unit Test", nullout(), nullout()) { }
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        int impl( int /* argc */, const char* /* argv */ []) const final { throw std::exception(); }
    };

    CHECK( Main().run( 0, nullptr) == 2);
}

TEST_CASE("MainWrapper: throw integer")
{
    struct Main : public MainWrapper
    {
        Main() : MainWrapper( "Example Unit Test", nullout(), nullout()) { }
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays, modernize-avoid-c-arrays, hicpp-avoid-c-arrays)
        int impl( int /* argc */, const char* /* argv */ []) const final { throw 222; } // NOLINT(hicpp-exception-baseclass) test bad behavior
    };

    CHECK( Main().run( 0, nullptr) == 3);
}
