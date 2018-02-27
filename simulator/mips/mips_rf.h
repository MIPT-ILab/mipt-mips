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
        std::array<Reg, REG_NUM_MAX> array = {};

        Reg& get_entry( RegNum num) { return array.at( static_cast<size_t>( num)); }
        const Reg& get_entry( RegNum num) const { return array.at( static_cast<size_t>( num)); }

        uint32 read( RegNum num) const
        {
            assert( num != REG_NUM_HI_LO);
            return get_entry( num).value;
        }

        void write( RegNum num, uint64 val)
        {
            if ( num == REG_NUM_ZERO)
                return;
            if ( num == REG_NUM_HI_LO) {
                write( REG_NUM_HI, val >> 32);
                write( REG_NUM_LO, val);
                return;
            }            

            auto& entry = get_entry(num);
            entry.value = val;
        }

    public:
        MIPSRF() = default;

        inline void read_source_1( MIPSInstr* instr) const
        {
            instr->set_v_src1( read(instr->get_src1_num()));
        }

        inline void read_source_2( MIPSInstr* instr) const
        {
            instr->set_v_src2( read(instr->get_src2_num()));
        }

        inline void read_sources( MIPSInstr* instr) const
        {
            read_source_1( instr);
            read_source_2( instr);
        }

        inline void write_dst( const MIPSInstr& instr)
        {
            RegNum reg_num  = instr.get_dst_num();
            bool writes_dst = instr.get_writes_dst();
            if ( REG_NUM_ZERO != reg_num && writes_dst)
                write( reg_num, instr.get_v_dst());
            else
                write( reg_num, read(reg_num));
        }
};

#endif

