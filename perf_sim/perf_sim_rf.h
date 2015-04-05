/**
 * perf_sim_rf.h
 * Register file for scalar MIPS CPU simulator.
 * MIPT-MIPS Assignment 4.
 * Ladin Oleg.
 */

/* Protection from multi-including. */
#ifndef PERF_SIM_RF_H
#define PERF_SIM_RF_H

/* Simulator modules. */
#include <func_instr.h>

class RF
{
    private:
        /* Array of registers. */
        struct Reg
        {
            uint32 value;
            bool is_valid;
            Reg() : value( 0ull), is_valid( true) {}
        } array [ REG_NUM_MAX];

    public:
        /* Check validate of register. */
        bool check( RegNum num) const
        {
            /* Check register number. */
            if ( ( num < REG_NUM_ZERO) || ( num >= REG_NUM_MAX))
            {
                cerr << "ERROR: Wrong register number!\n";
                exit( EXIT_FAILURE);
            }
            return array[ num].is_valid;
        }
        /* Makes register invalid. It cannot be used as source. */
        void invalidate( RegNum num)
        {
            /* Check register number. */
            if ( ( num < REG_NUM_ZERO) || ( num >= REG_NUM_MAX))
            {
                cerr << "ERROR: Wrong register number!\n";
                exit( EXIT_FAILURE);
            }
            if ( num == REG_NUM_ZERO) // "zero" register is always valid
            {
                return;
            }
            array[ num].is_valid = false;
        }

        inline void read_src1( FuncInstr& instr) const
        {
            RegNum num = instr.get_src1_num();
            instr.set_v_src1( array[ num].value);
        }
        inline void read_src2( FuncInstr& instr) const
        {
            RegNum num = instr.get_src2_num();
            instr.set_v_src2( array[ num].value);
        }
        inline void write_dst( const FuncInstr& instr)
        {
            RegNum num = instr.get_dst_num();
            if ( num != REG_NUM_ZERO) // "zero" register is unchangable
            {
                if ( check( num)) // register to write must be invalid
                {
                    cerr << "ERROR: Writing to valid register!\n";
                    exit( EXIT_FAILURE);
                }
                /* Write value and make it valid. */
                array[ num].value = instr.get_v_dst();
                array[ num].is_valid = true;
            }
        }
        inline void reset( RegNum num)
        {
            /* Check register number. */
            if ( ( num < REG_NUM_ZERO) || ( num >= REG_NUM_MAX))
            {
                cerr << "ERROR: Wrong register number!\n";
                exit( EXIT_FAILURE);
            }
            array[ num].value = 0;
        }

        /* Registers will be zeroed and valid according Reg() constructor. */
        RF() {}
};

#endif // #ifndef PERF_SIM_RF_H
