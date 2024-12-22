#ifndef SIM_CFG_HPP_
#define SIM_CFG_HPP_

#include <cstddef>
#include <cstdint>

namespace sim {
    const size_t kMemorySize = 0x80000;
    
    using Register = uint32_t;
    using IRegister = int32_t;
    using Address = uint32_t;
}

#endif // SIM_CFG_HPP_
