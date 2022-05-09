/*
* register.h - concept of Register
* @author Pavel Kryukov pavel.kryukov@phystech.edu
* Copyright 2022 MIPT-V
*/

#ifndef REGISTER_H
#define REGISTER_H

#include <infra/types.h>

#include <concepts>
#include <ostream>
#include <string_view>

template<typename R>
concept Register = requires(R reg)
{
    { reg.dump()        }      -> std::same_as<std::string_view>;
    { reg.is_zero()     }      -> std::same_as<bool>;
    { reg.is_mips_hi()  }      -> std::same_as<bool>;
    { reg.is_mips_lo()  }      -> std::same_as<bool>;
    { R::from_cpu_index(0)  }  -> std::same_as<R>;
    { R::from_gdb_index(0)  }  -> std::same_as<R>;
    { R::from_csr_name("")  }  -> std::same_as<R>;
    { R::get_gdb_pc_index()  } -> std::same_as<uint8>;
    { reg.to_rf_index()      } -> std::same_as<std::size_t>;
};

inline std::ostream& operator<<( std::ostream& out, const Register auto& rhs)
{
    return out << rhs.dump();
}

#endif
