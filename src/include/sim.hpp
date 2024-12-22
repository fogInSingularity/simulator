#ifndef SIM_HPP_
#define SIM_HPP_

#include "cpu.hpp"
#include "memory.hpp"
// include jit.hpp
#include "iprogram_loader.hpp"
#include "sim_cfg.hpp"

namespace sim {

class Simulator {
  private:
    Cpu cpu_;
    Memory memory_;
    // jit
  public:
    Simulator(const ploader::IProgramLoader& ploader);
    ~Simulator() = default;

    void Execute();
    Register FetchInstr();
};

} // namespace sim


#endif // SIM_HPP_
