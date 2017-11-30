/**
 * Unit tests for configuration parser
 * @author Denis Los
 */



// Google test library
#include <gtest/gtest.h>


// Testing module
#include "../config.h"


// Utils
#include "infra/macro.h"



namespace config {
RequiredValue<std::string> binary_filename = { "binary,b", "input binary file"};
RequiredValue<uint64> num_steps = { "numsteps,n", "number of instructions to run"};

Value<bool> disassembly_on = { "disassembly,d", false, "print disassembly"};
Value<bool> functional_only = { "functional-only,f", false, "run functional simulation only"};
} // namespace config





//
// To check whether the returned values
// are equal to passed arguments
//
TEST( config_parse, Pass_Valid_Args_1)
{
    const uint64 mandatory_int_value = 145;
    const std::string mandatory_string_value{ "file.elf"};

    const char* argv[] =
    {
        "mipt-mips",
        "-b", "file.elf",
        "-n", "145",
        "-f",
        "--bp-mode", "dynamic_two_bit",
        "--bp-size", "128",
        "--bp-ways", "16"

    };
    const int argc = countof(argv);

    // should not throw any exceptions
    ASSERT_NO_THROW( config::handleArgs( argc, argv));


    ASSERT_EQ( config::num_steps, mandatory_int_value);
    ASSERT_FALSE( mandatory_string_value.compare( config::binary_filename));
    ASSERT_EQ( config::disassembly_on, false);
    ASSERT_EQ( config::functional_only, true);
}



//
// To check whether returned values
// are equal to passed arguments
//
TEST( config_parse,  Pass_Valid_Args_2)
{
    const uint64 mandatory_int_value = 356;
    const std::string mandatory_string_value{ "run_test.elf"};

    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test.elf",
        "-n", "356",
        "-d"
    };
    const int argc = countof(argv);

    // should not throw any exceptions
    ASSERT_NO_THROW( config::handleArgs( argc, argv));


    ASSERT_EQ( config::num_steps, mandatory_int_value);
    ASSERT_FALSE( mandatory_string_value.compare( config::binary_filename));
    ASSERT_EQ( config::disassembly_on, true);
    ASSERT_EQ( config::functional_only, false);
}



//
// Pass no arguments
//
TEST( config_parse,  Pass_No_Args)
{
    const char* argv[] =
    {
        "mipt-mips"
    };
    const int argc = countof(argv);

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( config::handleArgs( argc, argv),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "");
}



//
// Pass arguments without a binary option
//
TEST( config_parse,  Pass_Args_Without_Binary_Option)
{
    const char* argv[] =
    {
        "mipt-mips",
        "--numsteps", "356",
    };
    const int argc = countof(argv);

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( config::handleArgs( argc, argv),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "");
}



//
// Pass arguments without a numsteps option
//
TEST( config_parse,  Pass_Args_Without_Numsteps_Option)
{
    const char* argv[] =
    {
        "mipt-mips",
        "--binary", "test.elf",
    };
    const int argc = countof(argv);

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( config::handleArgs( argc, argv),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "");
}



//
// Pass arguments with unrecognised option
//
TEST( config_parse,  Pass_Args_With_Unrecognised_Option)
{
    const char* argv[] =
    {
        "mipt-mips",
        "--binary", "test.elf",
        "-n", "356",
        "-koption"
    };
    const int argc = countof(argv);

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( config::handleArgs( argc, argv),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "");
}



//
// Pass a binary option multiple times
//
TEST( config_parse,  Pass_Binary_Option_Multiple_Times)
{
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test_1.elf",
        "--binary", "run_test_2.elf",
        "-n", "412",
    };
    const int argc = countof(argv);

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( config::handleArgs( argc, argv),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "");
}



//
// Pass a binary option without an argument
//
TEST( config_parse,  Pass_Binary_Option_Without_Arg)
{
    const char* argv[] =
    {
        "mipt-mips",
        "-b",
        "-n", "412",
    };
    const int argc = countof(argv);

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( config::handleArgs( argc, argv),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "");
}



//
// Pass a numsteps option without an argument
//
TEST( config_parse,  Pass_Numsteps_Option_Without_Arg)
{
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "run_test.elf",
        "-n",
        "-f",
        "-d"
    };
    const int argc = countof(argv);

    // should exit with EXIT_FAILURE
    ASSERT_EXIT( config::handleArgs( argc, argv),
                 ::testing::ExitedWithCode( EXIT_FAILURE), "");
}



//
// To check whether providing configuration parser
// with the same option is a failure
//
TEST( config_provide_options, Provide_Config_Parser_With_Binary_Option_Twice)
{
    const char* argv[] =
    {
        "mipt-mips",
        "-b", "test.elf",
        "-n", "100"
    };
    const int argc = countof(argv);

    // should not throw any exceptions
    ASSERT_NO_THROW( config::handleArgs( argc, argv));


    auto test_function = []()
    {
        config::RequiredValue<std::string> second_binary_file_option =
            {
                "binary,b",
                "input binary file"
            };
    };
    // should exit with EXIT_FAILURE
    ASSERT_EXIT( test_function(), ::testing::ExitedWithCode( EXIT_FAILURE), "ERROR.*");
}



int main( int argc, char** argv)
{
    ::testing::InitGoogleTest( &argc, argv);
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    return RUN_ALL_TESTS();
}

