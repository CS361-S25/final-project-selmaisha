#ifndef CPU_H
#define CPU_H

#include "Instructions.h"
#include "sgpl/algorithm/execute_cpu_n_cycles.hpp"
#include "sgpl/hardware/Cpu.hpp"
#include "sgpl/program/Program.hpp"
#include "sgpl/spec/Spec.hpp"

/**
 * CPU
 * Represents the virtual CPU and genome for an organism in the SignalGP-Lite system.
 * Handles task execution, mutation, and internal state management.
 */
class CPU {
  sgpl::Cpu<Spec> cpu;
  sgpl::Program<Spec> program;

  /**
   * Input: None
   *
   * Output: None
   *
   * Purpose: Initializes the jump table and input buffer in the CPUState.
   * Should be called when a new CPU is created or the program is changed.
   */
  void InitializeState() {
    cpu.InitializeAnchors(program);
    for (int i = 0; i < 4; i++) {
      state.last_inputs[i] = sgpl::tlrand.Get().GetUInt();
    }
  }

public:
  OrgState state;

  
  /**
   * CPU
   * input: emp::Ptr<OrgWorld> world
   * output: none
   * purpose: Constructor that initializes a new CPU with a random program.
   */
  CPU(emp::Ptr<OrgWorld> world) : program(100), state{world} {
    InitializeState();
  }

  /**
   * CPU
   * input: emp::Ptr<OrgWorld> world, const sgpl::Program<Spec>& program
   * output: none
   * purpose: Constructor that initializes a new CPU with a specified genome.
   */
  CPU(emp::Ptr<OrgWorld> world, const sgpl::Program<Spec> &program)
      : program(program), state{world} {
    InitializeState();
  }

  /**
   * Input: None
   * Output: None
   * Purpose: Resets the CPU to its initial state.
   */
  void Reset() {
    cpu.Reset();
    state = OrgState{state.world};
    InitializeState();
  }

  /**
   * Input: The number of CPU cycles to run.
   * Output: None
   * Purpose: Steps the CPU forward a certain number of cycles.
   */
  void RunCPUStep(size_t n_cycles) {
    if (!cpu.HasActiveCore()) {
      cpu.TryLaunchCore();
    }

    sgpl::execute_cpu_n_cycles<Spec>(n_cycles, cpu, program, state);
  }

  /**
   * Input: None
   * Output: None
   * Purpose: Mutates the genome code stored in the CPU.
   */
  void Mutate(double mutation_rate) {
    program.ApplyPointMutations(mutation_rate);
    InitializeState();
  }


  /**
   * LoadProgram
   * input: const sgpl::Program<Spec>& prog
   * output: none
   * purpose: Overwrites the current program with a new one and reinitializes CPU state.
   */
  void LoadProgram(const sgpl::Program<Spec>& prog) {
    program = prog;
    InitializeState();  // resets anchors, seeds input, etc.
  }


  /**
   * Input: None
   * Output: Returns the CPU's program
   * Purpose: Get the genome (program) of an Organism from its CPU
   */
  const sgpl::Program<Spec> &GetProgram() const { return program; }
  

private:
  /**
   * Input: The instruction to print, and the context needed to print it.
   * Output: None
   * Purpose: Prints out the human-readable representation of a single
   * instruction.
   */
  void PrintOp(const sgpl::Instruction<Spec> &ins,
               const std::map<std::string, size_t> &arities,
               sgpl::JumpTable<Spec, Spec::global_matching_t> &table,
               std::ostream &out = std::cout) const {
    const std::string &name = ins.GetOpName();
    if (arities.count(name)) {
      // Simple instruction
      out << "    " << emp::to_lower(name);
      for (size_t i = 0; i < 12 - std::min(name.length(), 12ul); i++) {
        out << ' ';
      }
      size_t arity = arities.at(name);
      bool first = true;
      for (size_t i = 0; i < arity; i++) {
        if (!first) {
          out << ", ";
        }
        first = false;
        out << 'r' << (int)ins.args[i];
      }
    } else {
      // Jump or anchor with a tag
      // Match the tag to the correct global anchor, then print it out as a
      // 2-letter code AA, AB, etc.
      auto match = table.MatchRegulated(ins.tag);
      std::string tag_name;
      if (match.size()) {
        size_t tag = match.front();
        tag_name += 'A' + tag / 26;
        tag_name += 'A' + tag % 26;
      } else {
        tag_name = "<nowhere>";
      }

      if (name == "Global Jump If") {
        // "Global Jump If" is too long, just print "jump if"
        out << "    "
            << "jump if";
        for (size_t i = 0; i < 12 - 7; i++) {
          out << ' ';
        }
        out << 'r' << (int)ins.args[0] << ", " << tag_name;
      } else if (name == "Global Anchor") {
        out << tag_name << ':';
      } else {
        out << "<unknown " << name << ">";
      }
    }
    out << '\n';
  }

public:
  /**
   * Input: None
   * Output: None
   * Purpose: Prints out a human-readable representation of the program code of
   * the organism's genome to standard output.
   */
  void PrintGenome(std::ostream &out = std::cout) {
    std::map<std::string, size_t> arities{{"Nand", 3}, {"Add", 3},
                                          {"Subtract", 3}, {"Divide", 3},
                                          {"IO", 1},       {"Reproduce", 0}};

    for (auto i : program) {
      PrintOp(i, arities, cpu.GetActiveCore().GetGlobalJumpTable(), out);
    }
  }
};

#endif