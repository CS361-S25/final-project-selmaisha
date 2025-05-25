#ifndef ORG_H
#define ORG_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"

/**
 * Class representing a digital organism with an internal CPU for behavior,
 * capable of processing tasks, accumulating points, mutating, and reproducing.
 */
class Organism {
  CPU cpu;  // Internal CPU object handling behavior and state

public:
  /**
   * Constructor
   * input: Pointer to OrgWorld (world), optional initial points
   * output: Organism instance
   * purpose: Initialize organism with a CPU and optional starting points
   */
  Organism(emp::Ptr<OrgWorld> world, double points = 0.0) : cpu(world) {
    SetPoints(points);
  }

  /**
   * input: none
   * output: Copy of internal CPU
   * purpose: Access the organism's CPU object
   */
  CPU GetCPU() { return cpu; }

  /**
   * input: double _in (points)
   * output: none
   * purpose: Set the organism's points directly
   */
  void SetPoints(double _in) { cpu.state.points = _in; }

  /**
   * input: double _in (points)
   * output: none
   * purpose: Add to the organism's current point total
   */
  void AddPoints(double _in) { cpu.state.points += _in; }

  /**
   * input: none
   * output: double (current points)
   * purpose: Retrieve the organism's current point total
   */
  double GetPoints() { return cpu.state.points; }

  /**
   * input: none
   * output: none
   * purpose: Reset the CPU to its initial state
   */
  void Reset() { cpu.Reset(); }

  /**
   * input: double mutation_rate
   * output: none
   * purpose: Apply mutation to the organism's genome
   */
  void Mutate(double mutation_rate) { cpu.Mutate(mutation_rate); }

  bool IsDead(double max_age) {
    return cpu.state.age > max_age;
  }

  void ResetAge() {
    cpu.state.age = 0;
  }

  /**
   * input: double mutation_rate
   * output: optional<Organism> (offspring)
   * purpose: Attempt to create a mutated offspring of this organism
   */
  std::optional<Organism> CheckReproduction(double mutation_rate) {
    Organism offspring = *this;
    offspring.Reset();
    offspring.Mutate(mutation_rate);
    return offspring;
    return {};
  }

  std::string GetTaskColor() const {
    if (cpu.state.completed_NOT) return "blue";
    if (cpu.state.completed_NAND) return "red";
    if (cpu.state.completed_AND) return "green";
    if (cpu.state.completed_ORN) return "yellow";
    if (cpu.state.completed_OR) return "purple";
    if (cpu.state.completed_ANDN) return "orange";
    if (cpu.state.completed_NOR) return "pink";
    if (cpu.state.completed_XOR) return "cyan";
    if (cpu.state.completed_EQU) return "brown";
    return "black";
  }


  /**
   * input: emp::WorldPosition current_location
   * output: none
   * purpose: Run a CPU step to process tasks at the given world location
   */
  void Process(emp::WorldPosition current_location) {
    cpu.state.current_location = current_location;
    cpu.state.age++;
    cpu.RunCPUStep(10);
  }

  /**
   * input: none
   * output: Printed genome to stdout
   * purpose: Display the organism's genome for debugging or inspection
   */
  void PrintGenome() {
    std::cout << "program ------------" << std::endl;
    cpu.PrintGenome();
    std::cout << "end ---------------" << std::endl;
  }
};

#endif
