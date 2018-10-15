/*
 * config.cpp - implementation of Config class
 * Copyright 2017-2018 MIPT-MIPS
 */

#include <sstream>

/* Simulator modules */
#include "config.h"

namespace config {

static AliasedSwitch help_option = { "h", "help", "print help"};

/* basic method */
void handleArgs( int argc, const char* const argv[], int start_index)
{
    BaseValue::options().parse( argc, argv, start_index);

    if ( help_option)
        throw HelpOption( BaseValue::options().help());
}

} // namespace config

