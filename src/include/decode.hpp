#ifndef DECODE_HPP_
#define DECODE_HPP_

#include "sim_cfg.hpp"
#include "instructions.hpp"

namespace sim {

DecodedInstr Decode(Register enc_instr);

}; // namespace sim

#endif // DECODE_HPP_
