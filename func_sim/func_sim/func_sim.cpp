/*
 * func_sim.cpp - MIPS single-cycle implementation
 * @author Aleksandr Shashev <aleksandr.shashev.phystech.edu>
 * Copyright 2016 MIPT-MIPS
 */

// Generic C++
#include <string>

//MIPT-MIPS modules
#include <func_sim.h>
#include <func_memory.h>


MIPS::MIPS ()
{
   rf = new RF;
}

MIPS::~MIPS ()
{
    delete rf;
}

void MIPS::run( const string& tr, uint instr_to_run)
{
    // load trace
    
    mem = new FuncMemory( tr.c_str());
    this->PC = mem->startPC();
    
    std::cout << mem->dump ();

    for (uint i = 0; i < instr_to_run; ++i) 
    {
        uint32 instr_bytes;
        
        instr_bytes = fetch ();// Fetch
        
        std::cout << hex << instr_bytes << std::endl;

        FuncInstr instr ( instr_bytes, PC);// Decode and read sources 
        read_src        ( instr          );//
  
        instr.execute ();// Execute
 
        if (instr.operation == instr.OUT_I_LOAD)// Memory access
            load (instr);//
        else if (instr.operation == instr.OUT_I_STORE)//
            store (instr);//    
 
        wb (instr);// Writeback
        
        if (instr.new_PC == PC) break;
 
        updatePC (instr);// Update PC
 
        std::cout << instr << std::endl;// Dump
        
        // registers dump (only to debugging)
        //std::cout << std::endl;
        //rf->Dump ();
        //std::cout << std::endl << std::endl;
    }
    
    //std::cout << mem->dump ();
}

void MIPS::read_src( FuncInstr& instr) 
{
    instr.v_src1 = rf->read( (RegNum) instr.get_src1_num_index());
    instr.v_src2 = rf->read( (RegNum) instr.get_src2_num_index());
    instr.v_C    = instr.get_C ()                       ;
    
    instr.v_HI   = HI;
    instr.v_LO   = LO;
}
 
 
void MIPS::wb ( FuncInstr& instr) 
{
    if ( instr.format != instr.FORMAT_J && 
       (instr.operation != instr.OUT_I_BRANCH || 
        instr.operation != instr.OUT_I_STORE))
    {
        //this swich doesn't work in c++11 standart, but actually has to
        /*switch (instr.isaTable [instr.isaNum].name)
        {
            case std::string ("mthi") :
            {
                HI = instr.v_HI;
                break;
            }
            case ("mtlo") :
            {
                LO = instr.v_LO;
                break;           
            }
            case ("mult") :
            case ("multu") :
            case ("div") :
            case ("divu") :
            {
                LO = instr.v_LO;
                HI = instr.v_HI;
                break;           
            }
            case ("j") :
            case ("jr") :
            {
                break;
            }
            
            default : 
            {
                rf->write ( (RegNum) instr.get_dst_num_index (), instr.v_dst);
                break;    
            }
        }
    */
        if (instr.isaTable [instr.isaNum].name == std::string ("mthi"))
            HI = instr.v_HI;
        else if (instr.isaTable [instr.isaNum].name == std::string ("mtlo"))
            LO = instr.v_LO;
        else if (instr.isaTable [instr.isaNum].name == std::string ("mult" ) ||
                 instr.isaTable [instr.isaNum].name == std::string ("multu") ||
                 instr.isaTable [instr.isaNum].name == std::string ("div"  ) ||
                 instr.isaTable [instr.isaNum].name == std::string ("divu" ))
            {     
                LO = instr.v_LO;
                HI = instr.v_HI;
            }
        else if (instr.isaTable [instr.isaNum].name == std::string ("j") ||
                 instr.isaTable [instr.isaNum].name == std::string ("jr"))
                 int this_is_useless_variable = 1;    
        else 
            rf->write ( (RegNum) instr.get_dst_num_index (), instr.v_dst);
    }
}

void MIPS::load ( FuncInstr& instr) 
{
    //this swich doesn't work in c++11 standart, but actually has to
    /*switch (instr.isaTable [instr.isaNum].name)
    {
        case std::string ("lb") :
        {
            instr.v_dst = mem->read ( instr.mem_addr, sizeof (uint8));
            break;
        }
        case std::string ("lbu") :
        {
            instr.v_dst = (uint8) mem->read ( instr.mem_addr, sizeof (uint8));
            break;
        }
        case std::string ("lh") :
        {
            instr.v_dst = mem->read ( instr.mem_addr, sizeof (uint16));
            break;
        }
        case std::string ("lhu") :
        {
            instr.v_dst = (uint16) mem->read ( instr.mem_addr, sizeof (uint8));
            break;
        }
        case std::string ("lw") :
        {
            instr.v_dst = mem->read ( instr.mem_addr, sizeof (uint32));
            break;
        }
       
    }*/
    if (instr.isaTable [instr.isaNum].name == std::string ("lb"))
         instr.v_dst = mem->read ( instr.mem_addr, sizeof (uint8));
    else if (instr.isaTable [instr.isaNum].name == std::string ("lbu"))
        instr.v_dst = (uint8) mem->read ( instr.mem_addr, sizeof (uint8));
    else if (instr.isaTable [instr.isaNum].name == std::string ("lh"))
        instr.v_dst = mem->read ( instr.mem_addr, sizeof (uint16));
    else if (instr.isaTable [instr.isaNum].name == std::string ("lhu"))
        instr.v_dst = (uint16) mem->read ( instr.mem_addr, sizeof (uint8));
    else if (instr.isaTable [instr.isaNum].name == std::string ("lw"))
        instr.v_dst = mem->read ( instr.mem_addr, sizeof (uint32));
}

void MIPS::store ( const FuncInstr& instr) 
{
    //this swich doesn't work in c++11 standart, but actually has to
    /*switch (instr.isaTable [instr.isaNum].name)
    {
        case std::string ("sb") :
        {
            mem->write ( instr.mem_addr, instr.v_dst, sizeof (uint8));
            break;
        }
        case std::string ("sh") :
        {
            mem->write ( instr.mem_addr, instr.v_dst, sizeof (uint16));
            break;
        }
        case std::string ("sw") :
        {
            mem->write ( instr.mem_addr, instr.v_dst, sizeof (uint32));
            break;
        }
    }*/
    if (instr.isaTable [instr.isaNum].name == std::string ("sb"))
        mem->write ( instr.mem_addr, instr.v_dst, sizeof (uint8));
    else if (instr.isaTable [instr.isaNum].name == std::string ("sh"))
        mem->write ( instr.mem_addr, instr.v_dst, sizeof (uint16));
    else if (instr.isaTable [instr.isaNum].name == std::string ("sw"))
        mem->write ( instr.mem_addr, instr.v_dst, sizeof (uint32));
}

      
