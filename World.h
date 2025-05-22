#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/math/Random.hpp"
#include "Org.h"
#include "ConfigSetup.h"
#include <numeric>
#include <fstream>

class OrgWorld : public emp::World<Organism> {
private:
  BaselineConfig& config;
  emp::Random random;
  

public:
  OrgWorld(BaselineConfig& cfg)
    : config(cfg),
        random((cfg.SEED() == -1)
                ? static_cast<uint32_t>(std::time(nullptr))
                : static_cast<uint32_t>(cfg.SEED()))
    {
    SetPopStruct_Mixed(config.POP_SIZE());
    // Resize(config.POP_SIZE());
    }
//     : emp::World<Organism>(
//       (cfg.SEED() == -1)
//         ? static_cast<uint32_t>(std::time(nullptr))
//         : static_cast<uint32_t>(cfg.SEED())
//     ),
//     config(cfg),
//     random(GetRandom())
//   {
//     SetPopStruct_Mixed(config.POP_SIZE());
//   }

  void InitPopulation() {
    // Resize(config.POP_SIZE()); 
    for (size_t i = 0; i < config.POP_SIZE(); ++i) {
      emp::vector<int> genome(config.PROG_LENGTH());
      for (int& gene : genome) gene = random.GetUInt(config.INSTRUCTION_CNT());
      Inject(genome);
        // Inject(Organism(genome));
    //   AddOrg(Organism(genome));
        // Organism org(genome);
        // SetupOrg(i, Organism(genome));
        // Inject(Organism(genome));
        // InjectOrg(std::make_shared<Organism>(genome));
        // SetOrg(i, std::make_shared<Organism>(genome));
        // AddOrgAt(std::make_shared<Organism>(genome), i);




    }
  }

  void Update() {
    emp::vector<size_t> schedule = emp::GetPermutation(random, GetSize());

    for (size_t idx : schedule) {
      if (!IsOccupied(idx)) continue;

      auto& org = *pop[idx];
      org.ProcessStep(config.TASK_REWARD());

      auto offspring = org.TryReproduce(
        config.REPRO_COST(), config.REPRO_LIFESPAN(), random,
        config.MUT_RATE(), config.INSTRUCTION_CNT()
      );

      if (offspring.has_value()) {
        // DoBirth(offspring.value(), idx);
        // Inject(offspring.value());
        // this->SetupOrg(idx, offspring.value());
        SetupOrg(idx, offspring.value());
      } else if (org.IsDead(config.REPRO_LIFESPAN(), config.REPRO_COST())) {
        RemoveOrgAt(idx);
      }
    }
  }

  const emp::vector<emp::Ptr<Organism>>& GetPopulation() const {
    return pop;
}

  void LogStats(size_t update, std::ostream& os) {
    double total = 0.0;
    size_t count = 0;
    for (const auto& org_ptr : pop) {
      if (org_ptr) {
        total += std::accumulate(org_ptr->GetGenome().begin(), org_ptr->GetGenome().end(), 0.0);
        ++count;
      }
    }
    double avg = count ? total / count : 0.0;
    os << update << "," << avg << "\n";
  }
};

#endif // WORLD_H