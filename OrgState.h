#ifndef ORGSTATE_H
#define ORGSTATE_H

#include "emp/Evolve/World_structure.hpp"
#include <cstddef>

// This forward declaration is necessary since the world contains organisms,
// which contain CPUs, which contain the state. If OrgState included the full
// definition of OrgWorld, it would cause a circular dependency.
class OrgWorld;

/**
 * Struct representing the state of a digital organism's CPU.
 * Stores world reference, inputs, position, and task completion flags.
 */
struct OrgState {
  emp::Ptr<OrgWorld> world;  ///< Pointer to the world the organism belongs to

  uint32_t last_inputs[4];   ///< Array for tracking the four most recent input values
  size_t last_input_idx;     ///< Index of the most recent input in the circular buffer
  double points;             ///< Accumulated points used for reproduction decisions
  double age;           ///< Number of updates since last reproduction
  emp::WorldPosition current_location; ///< Current location of the organism in the world
  bool isParasite = false; ///< Flag indicating if the organism is a parasite

  // Task completion flags
  bool completed_NOT = false;
  bool completed_NAND = false;
  bool completed_AND = false;
  bool completed_ORN = false;
  bool completed_OR = false;
  bool completed_ANDN = false;
  bool completed_NOR = false;
  bool completed_XOR = false;
  bool completed_EQU = false;

  std::vector<std::string> last_solved_tasks;

  // In OrgState.h
  bool HasCompletedAnyTask() const {
    return completed_NOT || completed_NAND || completed_AND ||
          completed_ORN || completed_OR || completed_ANDN ||
          completed_NOR || completed_XOR || completed_EQU;
  }


  /**
   * input: uint32_t input (new input value)
   * output: none
   * purpose: Store the new input value in a circular buffer of recent inputs
   */
  void add_input(uint32_t input) {
    last_inputs[last_input_idx] = input;
    last_input_idx = (last_input_idx + 1) % 4;
  }
};

#endif
