/**
 * argv.h - argument list helper functions
 * @author Pavel Kryukov
 * Copyright 2019 MIPT-MIPS
 */
 
#ifndef INFRA_ARGV_H
#define INFRA_ARGV_H

static inline constexpr int count_argc( const char *const *argv)
{
    int argc = 0;
    while (argv[argc] != nullptr)
        argc++;
    return argc;
}

#endif
