/*
 * rf.h - mips register file
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

#ifndef RF_H
#define RF_H

#include <array>

#include "mips_instr.h"

class MIPSRF
{
        struct Reg {
            uint32 value = 0;
        };
        std::array<Reg, MIPS_REG_MAX> array = {};

        Reg& get_entry( MIPSRegNum num) { return array.at( static_cast<size_t>( num)); }
        const Reg& get_entry( MIPSRegNum num) const { return array.at( static_cast<size_t>( num)); }

        uint32 read( MIPSRegNum num) const
        {
            assert( num != MIPS_REG_HI_LO);
            return get_entry( num).value;
        }

        void write( MIPSRegNum num, uint64 val)
        {
            if ( num == MIPS_REG_ZERO)
                return;
            if ( num == MIPS_REG_HI_LO) {
                write( MIPS_REG_HI, val >> 32);
                write( MIPS_REG_LO, val);
                return;
            }            

            auto& entry = get_entry(num);
            entry.value = val;
        }

    public:
        MIPSRF() = default;

        inline void read_source( MIPSInstr* instr, std::size_t src_index) const
        {
            if ( src_index == 0)
                instr->set_v_src1( read( instr->get_src1_num()));
            else
                instr->set_v_src2( read( instr->get_src2_num()));
        }

        inline void read_sources( MIPSInstr* instr) const
        {
            read_source( instr, 0);
            read_source( instr, 1);
        }

        inline void write_dst( const MIPSInstr& instr)
        {
            MIPSRegNum reg_num  = instr.get_dst_num();
            bool writes_dst = instr.get_writes_dst();
            if ( MIPS_REG_ZERO != reg_num && writes_dst)
                write( reg_num, instr.get_v_dst());
            else
                write( reg_num, read(reg_num));
        }
};

#endif

