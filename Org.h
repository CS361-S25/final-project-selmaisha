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
  Organism(emp::Ptr<OrgWorld> world, double points = 0.0, bool isParasite = false) : cpu(world) {
    SetPoints(points);
    cpu.state.age = 0.0;  // Initialize the last input index
    cpu.state.isParasite = isParasite;  // Set if this organism is a parasite
  }

  virtual ~Organism() = default;

  /**
   * input: none
   * output: Copy of internal CPU
   * purpose: Access the organism's CPU object
   */
  CPU GetCPU() { return cpu; }
  const CPU& GetCPU() const { return cpu; }

  // In Organism, Host, and Parasite classes
  CPU & GetCPURef() { return cpu; }
  const CPU & GetCPURef() const { return cpu; }

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

  bool IsDead(double max_age) { //might have to add virtual here
    return (cpu.state.age > max_age || cpu.state.points < 0.0);
  }

  void ResetAge() {
    cpu.state.age = 0;
  }

  virtual std::unique_ptr<Organism> Clone() const {
    return std::make_unique<Organism>(*this);
}

  void ClearTaskFlags() {
    cpu.state.completed_NOT = false;
    cpu.state.completed_NAND = false;
    cpu.state.completed_AND = false;
    cpu.state.completed_ORN = false;
    cpu.state.completed_OR = false;
    cpu.state.completed_ANDN = false;
    cpu.state.completed_NOR = false;
    cpu.state.completed_XOR = false;
    cpu.state.completed_EQU = false;
  }

  bool canSolveTask() const {
    return cpu.state.completed_NOT || cpu.state.completed_NAND || cpu.state.completed_AND ||
           cpu.state.completed_ORN || cpu.state.completed_OR || cpu.state.completed_ANDN ||
           cpu.state.completed_NOR || cpu.state.completed_XOR || cpu.state.completed_EQU;
  }

  /**
   * input: double mutation_rate
   * output: optional<Organism> (offspring)
   * purpose: Attempt to create a mutated offspring of this organism
   */
  std::optional<std::unique_ptr<Organism>> CheckReproduction(double mutation_rate) {
    auto offspring = Clone();        
    offspring->Reset();
    offspring->Mutate(mutation_rate);
    return offspring;
}

  /* std::optional<Organism> CheckReproduction(double mutation_rate) {
    Organism offspring = *this;
    offspring.Reset();
    offspring.Mutate(mutation_rate);
    return offspring;
    return {};
  } */

  std::string GetTaskColor() const { // changed it to prioritize based on task difficulty
    if (cpu.state.completed_EQU) return "brown";
    if (cpu.state.completed_XOR) return "cyan";
    if (cpu.state.completed_NOR) return "pink";
    if (cpu.state.completed_ANDN) return "orange";
    if (cpu.state.completed_OR) return "purple";
    if (cpu.state.completed_ORN) return "yellow";
    if (cpu.state.completed_AND) return "green";
    if (cpu.state.completed_NAND) return "red";
    if (cpu.state.completed_NOT) return "blue";
    return "black";
  }

  //assuming for now that only one task can be solved at a time
  //come back to this later if needed
  /**
   * input: none
   * output: string (name of the solved task)
   * purpose: Retrieve the name of the task that this organism has completed
   */
  std::string GetSolvedTask() const {
  if (cpu.state.completed_NOT) return "NOT";
  if (cpu.state.completed_NAND) return "NAND";
  if (cpu.state.completed_AND) return "AND";
  if (cpu.state.completed_ORN) return "ORN";
  if (cpu.state.completed_OR) return "OR";
  if (cpu.state.completed_ANDN) return "ANDN";
  if (cpu.state.completed_NOR) return "NOR";
  if (cpu.state.completed_XOR) return "XOR";
  if (cpu.state.completed_EQU) return "EQU";
  return "";
}

  /**
   * input: emp::WorldPosition current_location
   * output: none
   * purpose: Run a CPU step to process tasks at the given world location
   */
  // void Process(emp::WorldPosition current_location) {
  virtual void Process(OrgWorld& world, size_t id) {
    // cpu.state.current_location = current_location;
    cpu.state.current_location = emp::WorldPosition(id);
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
