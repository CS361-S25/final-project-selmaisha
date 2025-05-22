#ifndef ORG_H
#define ORG_H

#include <iostream>
// #include "emp/base/vector.hpp"
// #include "emp/math/random_utils.hpp"
// #include "emp/math/Random.hpp"
#include "CPU.h"
// #include "OrgState.h"
#include "World.h"
#include "ConfigSetup.h"
#include "emp/Evolve/World_structure.hpp"

class Organism {
private:
  emp::vector<int> genome;               // Digital genome (integer instructions)
  int energy = 0;                        // Energy points accumulated
  int age_since_reproduction = 0;        // Time since last reproduction

public:
  Organism() = default;
  Organism(const emp::vector<int>& g) : genome(g) {}

  const emp::vector<int>& GetGenome() const { return genome; }
  int GetEnergy() const { return energy; }  // Added for drawing by energy

  void Mutate(double mut_rate, size_t instruction_count, emp::Random& random) {
    for (int& gene : genome) {
      if (random.GetDouble() < mut_rate) {
        gene = random.GetUInt(instruction_count);
      }
    }
  }

  void ProcessStep(int reward_per_step = 5) {
    energy += reward_per_step;
    ++age_since_reproduction;
  }

  std::optional<Organism> TryReproduce(int reproduction_cost, int max_lifespan, emp::Random& random, double mut_rate, size_t instruction_count) {
    if (energy >= reproduction_cost) {
      Organism offspring = *this;
      offspring.Mutate(mut_rate, instruction_count, random);
      offspring.energy = 0;
      offspring.age_since_reproduction = 0;

      energy -= reproduction_cost;
      age_since_reproduction = 0;

      return offspring;
    }

    if (age_since_reproduction >= max_lifespan) {
      return std::nullopt;
    }

    return std::nullopt;
  }

  bool IsDead(int max_lifespan, int reproduction_cost) const {
    return age_since_reproduction >= max_lifespan && energy < reproduction_cost;
  }
};

#endif // ORG_H