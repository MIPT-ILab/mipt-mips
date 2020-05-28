/*
 * bpu.h - the branch prediction unit for MIPS
 * @author George Korepanov <gkorepanov.gk@gmail.com>
 * Copyright 2017-2018 MIPT-MIPS
 */

#ifndef BRANCH_PREDICTION_UNIT
#define BRANCH_PREDICTION_UNIT

#include "bp_interface.h"

// MIPT_MIPS modules
#include <infra/exception.h>
#include <infra/types.h>

#include <memory>
#include <string>

struct BPInvalidMode final : Exception
{
    BPInvalidMode( const std::string& mode, const std::string& all_modes)
        : Exception("Invalid mode of branch prediction", mode + '\n' + all_modes)
    { }
};

/*
 *******************************************************************************
 *                           BRANCH PREDICTION UNIT                            *
 *******************************************************************************
 */
class BaseBP
{
    BaseBP( const BaseBP&) = default;
    BaseBP( BaseBP&&) = default;
protected:
    BaseBP() = default;
public:
    virtual bool is_taken( Addr PC) const = 0;
    virtual bool is_hit( Addr PC) const = 0;
    virtual Addr get_target( Addr PC) const = 0;
    virtual void update( const BPInterface& bp_upd) = 0;

    BPInterface get_bp_info( Addr PC) const
    {
        if ( is_hit( PC))
            return BPInterface( PC, is_taken( PC), get_target( PC), true);

        return BPInterface( PC, false, PC + 4, false);
    }

    virtual ~BaseBP() = default;
    BaseBP& operator=( const BaseBP&) = default;
    BaseBP& operator=( BaseBP&&) = default;
    
    static std::unique_ptr<BaseBP> create_bp(const std::string& name, const std::string& lru,
                uint32 size_in_entries, uint32 ways, uint32 branch_ip_size_in_bits);
    static std::unique_ptr<BaseBP> create_configured_bp();
};

#endif
