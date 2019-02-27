/*
 * mips_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015-2018 MIPT-MIPS
 */

#include "mips_instr.h"

#include <infra/instrcache/LRUCache.h>

#include <iomanip>
#include <iostream>
#include <sstream>

static_assert(std::is_trivially_destructible<BaseMIPSInstr<uint32>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
static_assert(std::is_trivially_destructible<BaseMIPSInstr<uint64>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
// Visual Studio implements is_trivially copyable in a buggy way
// https://developercommunity.visualstudio.com/content/problem/170883/msvc-type-traits-stdis-trivial-is-bugged.html
#ifdef __GNUC__
static_assert(std::is_trivially_copyable<BaseMIPSInstr<uint32>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
static_assert(std::is_trivially_copyable<BaseMIPSInstr<uint64>>::value,
              "For performance reasons, BaseMIPSInstr should not contain non-trivial members");
#endif

template<typename R>
typename BaseMIPSInstr<R>::DisasmCache& BaseMIPSInstr<R>::get_disasm_cache()
{
    static DisasmCache instance;
    return instance;
}

template<typename R>
std::string BaseMIPSInstr<R>::string_dump() const
{
    std::ostringstream oss;
    this->dump_content( oss, get_disasm());
    return oss.str();
}

template<typename R>
std::string BaseMIPSInstr<R>::bytes_dump() const
{
     std::ostringstream oss;
     oss << "Bytes:" << std::hex;
     for ( const auto& b : unpack_array<uint32, endian>( raw))
         oss << " 0x" << std::setfill( '0') << std::setw( 2) << static_cast<uint16>( b);
     return oss.str();
}

template<typename R>
std::string BaseMIPSInstr<R>::get_disasm() const
{
    if ( !raw_valid)
        return generate_disasm();

    const auto [found, value] = get_disasm_cache().find( raw);
    if ( found) {
        get_disasm_cache().touch( raw);
        return value;
    }
    auto result = generate_disasm();
    get_disasm_cache().update( raw, result);
    return result;
}

template class BaseMIPSInstr<uint32>;
template class BaseMIPSInstr<uint64>;
