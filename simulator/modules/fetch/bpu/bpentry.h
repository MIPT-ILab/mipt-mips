/*
 * bpentry.h - the branch prediction unit entries for MIPS
 * @author George Korepanov <gkorepanov.gk@gmail.com>
 * Copyright 2017 MIPT-MIPS
 */

 #ifndef BRANCH_PREDICTION_ENTRY
 #define BRANCH_PREDICTION_ENTRY

#include <array>

#include <infra/macro.h>
#include <infra/types.h>

/* TODO: move implemetations to .cpp file */

/* each inherited class has to implement at least these methods:
 * 1. bool is_taken( Addr PC, Addr target);
 * 2. void update( bool is_taken)
 */

template<bool DIRECTION>
class BPEntryAlwaysOneDirection final
{
public:
    bool is_taken( Addr /* unused */, Addr /* target */) const { return DIRECTION; }
    void update( bool /* is_taken */) { }
    void reset() { }
};

using BPEntryAlwaysTaken = BPEntryAlwaysOneDirection<true>;
using BPEntryAlwaysNotTaken = BPEntryAlwaysOneDirection<false>;

class BPEntryBackwardJumps final
{
public:
    /* prediction */
    static bool is_taken( Addr PC, Addr target) { return target < PC ; }
    void update( bool /* is_taken */) { }
    void reset() { }
};

/* dynamic predictors */

class BPEntryOneBit final
{
private:
    enum class State
    {
        NT = 0,
        T = 1
    };
    static const State default_state = State::NT;

    State state = default_state;

public:
    /* prediction */
    bool is_taken( Addr /* unused */, Addr /* target */) const { return state == State::T; }

    /* update */
    void update( bool is_taken)
    {
        state = static_cast<State>( is_taken);
    }

    void reset() { state = default_state; }
};

class BPEntryTwoBit
{
public:
    class State
    {
        enum class StateValue
        {
            NT  = 0, // NOT TAKEN
            WNT = 1, // WEAKLY NOT TAKEN
            WT  = 2, // WEAKLY TAKEN
            T   = 3  // TAKEN
        };

        static const StateValue default_value = StateValue::WNT;
        StateValue value = default_value;

    public:
        /* updating state */
        void update ( bool is_taken)
        {
            if ( is_taken)
            {
                switch ( value)
                {
                    case StateValue::NT:  value = StateValue::WNT; break;
                    case StateValue::WNT: value = StateValue::WT;  break;
                    case StateValue::WT:  /* fallthrough */
                    case StateValue::T:   value = StateValue::T;   break; // saturation
                }
            }
            else
            {
                switch ( value)
                {
                    case StateValue::NT:  /* fallthrough, saturation */
                    case StateValue::WNT: value = StateValue::NT;  break;
                    case StateValue::WT:  value = StateValue::WNT; break;
                    case StateValue::T:   value = StateValue::WT;  break;
                }
            }
        }

        /* casting to result */
        bool is_taken() const
        {
            return value == StateValue::WT || value == StateValue::T;
        }

        void reset() { value = default_value; }
    };

private:
    State state = {};

public:
    /* prediction */
    bool is_taken( Addr /* unused */, Addr /* target */) const
    {
        return state.is_taken();
    }

    /* update */
    void update( bool is_taken)
    {
        state.update( is_taken);
    }

    void reset()
    {
        state.reset();
    }
};


/* adaptive predictors */
template<size_t DEPTH>
class BPEntryAdaptive final
{
    static const constexpr uint32 default_pattern = 0;
    /* The index is a pattern, and the value is prediction state,
     * so the table might look like this:
     * ---------
     * 00 -- NT
     * 01 -- T
     * 10 -- WNT
     * 11 -- WNT
     * ---------
     */
    std::array<BPEntryTwoBit::State, bitmask<size_t>(DEPTH) + 1> state_table = {{}};

    /* two-level predictor */
    class PredictionPattern
    {
        static const constexpr uint32 pattern_mask = bitmask<uint32>(DEPTH);
        static const constexpr uint32 default_pattern = 0;
        uint32 value = default_pattern;

    public:
        /* for vector indexing */
        size_t get_value() const { return value; }

        void update( bool is_taken)
        {
            value <<= 1U;
            value &= pattern_mask;
            if ( is_taken)
                value |= 1U;
        }

        void reset() { value = default_pattern; }
    };

    PredictionPattern current_pattern = {};

public:
    BPEntryAdaptive() = default;

    /* prediction */
    bool is_taken( Addr /* unused */, Addr /* target */) const
    {
        return state_table.at( current_pattern.get_value()).is_taken();
    }

    /* update */
    void update( bool is_taken)
    {
        state_table.at( current_pattern.get_value()).update( is_taken);
        current_pattern.update( is_taken);
    }

    void reset()
    {
        for(auto& elem : state_table)
            elem.reset();

        current_pattern.reset();
    }
};

#endif
