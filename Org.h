#ifndef ORG_H
#define ORG_H

#include <iostream>
#include "CPU.h"
#include "World.h"
#include "ConfigSetup.h"
#include "emp/Evolve/World_structure.hpp"

class Organism {
private:
   CPU cpu;                          // CPU object for executing instructions

public:
  Organism(emp::Ptr<OrgWorld> world, double points = 0.0) : cpu(world) {
    SetPoints(points);
  }

  //all this is "new" or from previous starter code
  CPU& GetCPU()  { return cpu; }

  void SetPoints(double _in) { cpu.state.points = _in; }
  void AddPoints(double _in) { cpu.state.points += _in; }
  double GetPoints() { return cpu.state.points; }
  void Reset() { cpu.Reset(); }
  void Mutate(double mutation_rate) { cpu.Mutate(mutation_rate); }

  void Process(emp::WorldPosition current_location) {
    cpu.state.current_location = current_location;
    cpu.RunCPUStep(10);
    cpu.state.age_since_reproduction++;

    //  auto world = cpu.state.world;
    // if (cpu.state.points >= world->GetConfig().REPRO_COST() &&
    //     cpu.state.age_since_reproduction >= 5) {
    //   world->ReproduceOrg(current_location);
    // }
  }


  std::optional<Organism> CheckReproduction(double mutation_rate, double repro_cost, int max_lifespan) {

if (cpu.state.points >= 5 && cpu.state.age_since_reproduction >= 5) {
    Organism offspring = *this;
    offspring.Reset();
    offspring.Mutate(mutation_rate);
    cpu.state.points -= repro_cost;
    cpu.state.age_since_reproduction = 0;
    return offspring;
  }
  return std::nullopt;
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



  bool IsDead(int max_lifespan) const {
    return cpu.state.age_since_reproduction >= max_lifespan;
  }

  void PrintGenome() {
    std::cout << "program ------------" << std::endl;
    cpu.PrintGenome();
    std::cout << "end ---------------" << std::endl;
  }

};

#endif // ORG_H