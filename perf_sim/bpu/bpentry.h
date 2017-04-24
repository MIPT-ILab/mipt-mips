/*
 * bpentry.h - the branch prediction unit entries for MIPS
 * @author George Korepanov <gkorepanov.gk@gmail.com>
 * Copyright 2017 MIPT-MIPS
 */

 #ifndef BRANCH_PREDICTION_ENTRY
 #define BRANCH_PREDICTION_ENTRY

/* TODO: move implemetations to .cpp file */

/* each inherited class has to implement three methods:
 * 1. constructor();
 * 2. bool isTaken( PC);
 * 3. void update( bool is_taken, Addr target)
 */
class BPEntry
{
protected:
    Addr _target;

public:
    Addr getTarget() const { return _target; }
};


/* static predictors */

class BPEntryStatic : public BPEntry
{
public:
    /* update */
    void update( Addr target) { _target = target; }
};

class BPEntryAlwaysTaken final : public BPEntryStatic
{
public:
    /* prediction */
    bool isTaken() const { return true; }
};

class BPEntryBackwardJumps final : public BPEntryStatic
{
public:
    /* prediction */
    bool isTaken( Addr PC) const { return _target < PC ; }
};


/* dynamic predictors */

class BPEntryOneBit final : public BPEntry
{
private:
    enum State
    {
        NT = 0,
        T = 1
    };
    static const State default_state = NT;

    State state = default_state;

public:
    /* prediction */
    bool isTaken() const { return state; }

    /* update */
    void update( bool is_taken, Addr target)
    {
        if ( is_taken && _target != target)
        {
            /* reset */
            state = default_state;
            _target = target;
        }

        state = static_cast<State>( is_taken);
    }
};

class BPEntryTwoBit : public BPEntry
{
protected:
    class State
    {
        enum StateValue
        {
            NT  = 0, // NOT TAKEN
            WNT = 1, // WEAKLY NOT TAKEN
            WT  = 2, // WEAKLY TAKEN
            T   = 3  // TAKEN
        };

        static const StateValue default_value = WNT;
        StateValue value = default_value;

    public:
        /* updating state */
        void update ( bool is_taken)
        {
            if ( is_taken)
            {
                switch ( value)
                {
                    case NT:  value = WNT; break;
                    case WNT: value = WT;  break;
                    case WT:  value = T;   break;
                    case T:   value = T;   break; // saturation
                }
            }
            else
            {
                switch ( value)
                {
                    case NT:  value = NT;  break; // saturation
                    case WNT: value = NT;  break;
                    case WT:  value = WNT; break;
                    case T:   value = WT;  break;
                }
            }
        }

        /* casting to result */
        operator const bool() const
        {
            switch ( value)
            {
                case NT:
                case WNT: return false;

                case WT:
                case T: return true;
            }
        }

        void reset() { value = default_value; }
    };

private:
    State state;

public:
    /* prediction */
    bool isTaken() const
    {
        return state;
    }

    /* update */
    void update( bool is_taken, Addr target)
    {
        if ( is_taken && _target != target)
        {
            state.reset();
            _target = target;
        }

        state.update( is_taken);
    }
};


/* adaptive predictors */

class BPEntryAdaptive final : public BPEntryTwoBit
{
    static const unsigned int default_pattern = 0;

    /* The index is a pattern, and the value is prediction state,
     * so the table might look like this:
     * ---------
     * 00 -- NT
     * 01 -- T
     * 10 -- WNT
     * 11 -- WNT
     * ---------
     */
    std::vector<State> state_table;

    /* two-level predictor */
    static const unsigned int pattern_depth = 2;
    class PredictionPattern
    {
        static const unsigned int pattern_mask = ( 1ull << 2) - 1;

        static const unsigned int default_pattern = 0;
        unsigned int value = default_pattern;

    public:
        /* for vector indexing */
        operator const size_t() const { return value; }

        void update( bool is_taken)
        {
            /* updating pattern, simulating shift register */
            value <<= 1;
            value += static_cast<unsigned int>( is_taken);
            value &= pattern_mask;
        }

        void reset() { value = default_pattern; }
    };

    PredictionPattern current_pattern;

public:
    BPEntryAdaptive() :
        state_table( 1ull << pattern_depth, State())
    {}

    /* prediction */
    bool isTaken() const
    {
        return state_table[ current_pattern];
    }

    /* update */
    void update( bool is_taken, Addr target)
    {
        if ( is_taken && _target != target)
        {
            for(auto& elem : state_table)
                elem.reset();

            current_pattern.reset();
            _target = target;
        }

        state_table[ current_pattern].update( is_taken);
        current_pattern.update( is_taken);
    }
};

#endif
