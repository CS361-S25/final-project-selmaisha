#include "World.h"
#include "ConfigSetup.h"
#include <iostream>
#include <fstream>
#include "emp/config/config.hpp"
#include "emp/config/ArgManager.hpp"

int main(int argc, char* argv[]) {
  BaselineConfig config;


  // Register all config fields with ArgManager
  auto specs = emp::ArgManager::make_builtin_specs(&config);
  emp::ArgManager am(argc, argv, specs);

  // Read from file and URL
  config.Read("baseline_config.cfg");
  am.UseCallbacks();

  if (am.HasUnused()) {
    std::cerr << "Unused config parameters!" << std::endl;
    std::exit(EXIT_FAILURE);
  }

  // Create and run simulation
  OrgWorld world(config);
  world.InitPopulation();

  std::ofstream ofs(config.OUTPUT_DIR() + "baseline_data.csv");
  ofs << "update,avg_genome_sum\n";

  for (size_t update = 0; update < config.RUN_TIME(); ++update) {
    world.Update();
    if (update % 100 == 0) {
      world.LogStats(update, ofs);
      std::cout << "Update: " << update << std::endl;
    }
  }

  std::cout << "Simulation complete. Data written to baseline_data.csv\n";
  return 0;
}
