#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "OrgState.h"
#include "sgpl/library/OpLibraryCoupler.hpp"
#include "sgpl/library/prefab/ArithmeticOpLibrary.hpp"
#include "sgpl/library/prefab/NopOpLibrary.hpp"
#include "sgpl/operations/flow_global/Anchor.hpp"
#include "sgpl/program/Instruction.hpp"
#include "sgpl/program/Program.hpp"
#include "sgpl/spec/Spec.hpp"

/**
 * Custom instruction for I/O behavior.
 * Outputs a register value to the world as a potential task solution,
 * then fetches a new random input and stores it in the same register.
 */
struct IOInstruction {
  template <typename Spec>
  static void run(sgpl::Core<Spec> &core, const sgpl::Instruction<Spec> &inst,
                  const sgpl::Program<Spec> &,
                  typename Spec::peripheral_t &state) noexcept {
    uint32_t output = core.registers[inst.args[0]];
    state.world->CheckOutput(output, state);

    uint32_t input = sgpl::tlrand.Get().GetUInt();
    core.registers[inst.args[0]] = input;
    state.add_input(input);
  }

  static std::string name() { return "IO"; }
  static size_t prevalence() { return 3; }
  
};


/**
 * Custom bitwise NAND instruction.
 * Stores the NAND of two registers into a target register.
 */
struct NandInstruction {
  template <typename Spec>
  static void run(sgpl::Core<Spec> &core, const sgpl::Instruction<Spec> &inst,
                  const sgpl::Program<Spec> &,
                  typename Spec::peripheral_t &state) noexcept {
                    uint32_t reg_b = core.registers[inst.args[1]];
                    uint32_t reg_c = core.registers[inst.args[2]];
                    uint32_t nand_val = ~(reg_b & reg_c);

                    core.registers[inst.args[0]] = nand_val;
                  }
  static std::string name() { return "Nand"; }
  static size_t prevalence() { return 5; }
};

/**
 * Custom ANDN instruction.
 * Computes A = B & ~C and stores result in target register.
 */
struct AndnInstruction {
  template <typename Spec>
  static void run(sgpl::Core<Spec> &core, const sgpl::Instruction<Spec> &inst,
                  const sgpl::Program<Spec> &,
                  typename Spec::peripheral_t &state) noexcept {
    uint32_t reg_b = core.registers[inst.args[1]];
    uint32_t reg_c = core.registers[inst.args[2]];
    uint32_t andn_val = reg_b & (~reg_c);
    core.registers[inst.args[0]] = andn_val;
  }
  static std::string name() { return "Andn"; }
  static size_t prevalence() { return 5; }
};



/**
 * A custom instruction that attempts to reproduce and produce a child organism,
 * if this organism has enough points.
 */
struct ReproduceInstruction {
  template <typename Spec>
  static void run(sgpl::Core<Spec> &core, const sgpl::Instruction<Spec> &inst,
                  const sgpl::Program<Spec> &,
                  typename Spec::peripheral_t &state) noexcept {
    if (state.points >= 20) {
      if (state.isParasite) {
        state.world->ReproduceParasite(state.current_location);
      } else {
        state.world->ReproduceOrg(state.current_location);
      }
      state.points -= 0; //points are reset elsewhere
    }
    
  }

  static std::string name() { return "Reproduce"; }
  static size_t prevalence() { return 1; }
};


// Coupled instruction library combining built-in and custom operations
using Library =
    sgpl::OpLibraryCoupler<sgpl::NopOpLibrary, sgpl::BitwiseShift, sgpl::Increment, sgpl::Decrement,
    sgpl::Add, sgpl::Subtract, sgpl::global::JumpIfNot, sgpl::local::JumpIfNot, sgpl::global::Anchor, IOInstruction, NandInstruction, AndnInstruction,
    ReproduceInstruction>;

using Spec = sgpl::Spec<Library, OrgState>;

#endif