/**
 * operation.h - list of basic operation types
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2019 MIPT-MIPS
 */
 
#ifndef OPERATION_H
#define OPERATION_H

#include <infra/string_view.h>
#include <infra/types.h>
#include <func_sim/trap_types.h>

enum OperationType : uint8
{
    OUT_ARITHM,
    OUT_DIVMULT,
    OUT_R_ACCUM,
    OUT_R_CONDM,
    OUT_R_JUMP,
    OUT_SYSCALL,
    OUT_BREAK,
    OUT_R_SUBTR,
    OUT_BRANCH,
    OUT_TRAP,
    OUT_LOAD,
    OUT_LOADU,
    OUT_PARTIAL_LOAD,
    OUT_STORE,
    OUT_J_JUMP,
    OUT_J_SPECIAL,
    OUT_UNKNOWN
};

class Operation
{
public:
	//target is known at ID stage and always taken
	bool is_direct_jump() const { return operation == OUT_J_JUMP; }

	//target is known at ID stage but if branch is taken or not is known only at EXE stage
	bool is_direct_branch() const { return operation == OUT_BRANCH; }

	// target is known only at EXE stage
	bool is_indirect_branch() const { return operation == OUT_R_JUMP; }

	bool is_jump() const { return this->is_direct_jump()     ||
				      this->is_direct_branch()   ||
				      this->is_indirect_branch(); }

    bool is_jump_taken() const { return  _is_jump_taken; }

    bool is_partial_load() const
    {
        return operation == OUT_PARTIAL_LOAD;
    }

    bool is_load() const { return operation == OUT_LOAD ||
                                   operation == OUT_LOADU ||
                                   is_partial_load(); }

    int8 get_accumulation_type() const
    {
        return (operation == OUT_R_ACCUM) ? 1 : (operation == OUT_R_SUBTR) ? -1 : 0;
    }
    Trap trap_type() const { return trap; }

    bool is_halt() const { return trap_type() == Trap::HALT; }
    bool is_conditional_move() const { return operation == OUT_R_CONDM; }
    bool is_divmult() const { return operation == OUT_DIVMULT || get_accumulation_type() != 0; }

    bool is_explicit_trap() const { return operation == OUT_TRAP; }
    bool is_syscall() const { return operation == OUT_SYSCALL; }
    bool has_trap() const { return trap_type() != Trap::NO_TRAP; }
    bool is_store() const { return operation == OUT_STORE; }

    auto get_mem_addr() const { return mem_addr; }
    auto get_mem_size() const { return mem_size; }
    auto get_new_PC() const { return new_PC; }
    auto get_PC() const { return PC; }

    void set_sequence_id( uint64 id) { sequence_id = id; }
    auto get_sequence_id() const { return sequence_id; }

    void set_v_imm( uint32 value) { v_imm = value; }

protected:
    Operation(Addr pc, Addr new_pc) : PC(pc), new_PC(new_pc) { }

    std::string_view opname = {};
    OperationType operation = OUT_UNKNOWN;
    Trap trap = Trap::NO_TRAP;

    Addr mem_addr = NO_VAL32;
    uint32 mem_size = NO_VAL32;
    uint32 v_imm = NO_VAL32;

    // convert this to bitset
    bool complete   = false;
    bool _is_jump_taken = false; // actual result
    bool memory_complete = false;
    bool print_dst = false;
    bool print_src1 = false;
    bool print_src2 = false;

    const Addr PC = NO_VAL32;
    Addr new_PC = NO_VAL32;

    uint64 sequence_id = NO_VAL64;
};

template<typename T>
class Datapath : public Operation
{
public:
    friend struct ALU;
    using Execute = void (*)(Datapath*);
    using RegisterUInt = T;
    using RegisterSInt = sign_t<RegisterUInt>;

    void set_v_src( const T& value, uint8 index)
    {
        if ( index == 0)
            v_src1 = value;
        else
            v_src2 = value;
    }

    T get_v_src2() const { return v_src2; } // for stores
    T get_v_dst() const { return v_dst; }
    T get_v_dst2() const { return v_dst2; }
    T get_mask() const { return mask; }

    void load( const T& value);
    void execute();

protected:
    Datapath(Addr pc, Addr new_pc) : Operation(pc, new_pc) {}

    T v_src1 = NO_VAL32;
    T v_src2 = NO_VAL32;
    T v_dst  = NO_VAL32;
    T v_dst2 = NO_VAL32;
    T mask   = all_ones<T>();

    Execute executor;
};

template<typename T>
void Datapath<T>::execute()
{
    executor(this);
    complete = true;
}

template<typename T>
void Datapath<T>::load( const T& value)
{
    memory_complete = true;
    if ( operation == OUT_LOAD || is_partial_load())
    {
        switch ( get_mem_size())
        {
            case 1: v_dst = narrow_cast<int8>( value); break;
            case 2: v_dst = narrow_cast<int16>( value); break;
            case 4: v_dst = narrow_cast<int32>( value); break;
            case 8: v_dst = value; break;
            default: assert( false);
        }
    }
    else if ( operation == OUT_LOADU)
    {
        v_dst = value;
    }
    else
    {
        assert( false);
    }
}

template<typename T, typename R>
class BaseInstruction : public Datapath<T>
{
public:
    using MyDatapath = Datapath<T>;
    using Register = R;
    R get_src_num( uint8 index) const { return ( index == 0) ? src1 : src2; }
    R get_dst_num()  const { return dst;  }
    R get_dst2_num() const { return dst2; }

    std::ostream& dump_content( std::ostream& out, const std::string& disasm) const;

protected:
    BaseInstruction(Addr pc, Addr new_pc) : Datapath<T>(pc, new_pc) { }

    R src1 = R::zero();
    R src2 = R::zero();
    R dst  = R::zero();
    R dst2 = R::zero();
};

template<typename T, typename R>
std::ostream& BaseInstruction<T, R>::dump_content( std::ostream& out, const std::string& disasm) const
{
    if ( this->PC != 0)
        out << std::hex << "0x" << this->PC << ": ";

    out << "{" << std::dec << this->sequence_id << "}\t" << disasm << "\t [";
    bool has_ma = ( this->is_load() || this->is_store()) && this->complete;
    if ( has_ma)
    {
        out << " $ma = 0x" << std::hex << this->get_mem_addr();
    }
    if ( !dst.is_zero() && (this->is_load() ? this->memory_complete : this->complete) && this->get_mask() != 0)
    {
        if ( has_ma)
            out << ",";
        out << " $" << dst << " = 0x" << std::hex << (this->v_dst & this->mask);
        if ( !dst2.is_zero())
            out << ", $" << this->dst2 << " = 0x" << this->v_dst2;
    }
    out << " ]";
    if ( this->trap != Trap::NO_TRAP)
        out << "\t trap";

    out << std::dec;
    return out;
}


#endif // OPERATION_H
 
