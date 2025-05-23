// #include "World.h"
// #include "ConfigSetup.h"
// #include <iostream>
// #include <fstream>
// #include "emp/config/config.hpp"
// #include "emp/config/ArgManager.hpp"

// int main(int argc, char* argv[]) {
//   BaselineConfig config;


//   // Register all config fields with ArgManager
//   auto specs = emp::ArgManager::make_builtin_specs(&config);
//   emp::ArgManager am(argc, argv, specs);

//   // Read from file and URL
//   config.Read("baseline_config.cfg");
//   am.UseCallbacks();

//   if (am.HasUnused()) {
//     std::cerr << "Unused config parameters!" << std::endl;
//     std::exit(EXIT_FAILURE);
//   }

//   // Create and run simulation
//   OrgWorld world(config);
//   world.InitPopulation();

//   std::ofstream ofs(config.OUTPUT_DIR() + "baseline_data.csv");
//   ofs << "update,avg_genome_sum\n";

//   for (size_t update = 0; update < config.RUN_TIME(); ++update) {
//     world.Update();
//     if (update % 100 == 0) {
//       world.LogStats(update, ofs);
//       std::cout << "Update: " << update << std::endl;
//     }
//   }

//   std::cout << "Simulation complete. Data written to baseline_data.csv\n";
//   return 0;
// }
#include "World.h"
#include "ConfigSetup.h"
#include <iostream>
/* #include <fstream>
#include "emp/config/config.hpp"
#include "emp/config/ArgManager.hpp"
 */

int main(int argc, char* argv[]) {
  BaselineConfig config;

  bool success = config.Read("baseline_config.cfg");
  if (!success) config.Write("baseline_config.cfg");

  // Parse command-line arguments and config file
  // I adapted it to be closer to what we have done before
  config.Read("baseline_config.cfg");

  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(config, std::cout, "baseline_config.cfg") == false) {
    std::cerr << "There was a problem in processing the options file." << std::endl;
    exit(1);
  }
  if (args.TestUnknown() == false) {
    std::cerr << "Leftover args no good." << std::endl;
    exit(1);
  }


/*   // Register all config fields with ArgManager
  auto specs = emp::ArgManager::make_builtin_specs(&config);
  emp::ArgManager am(argc, argv, specs);

  // Read from file and URL
  config.Read("baseline_config.cfg");
  am.UseCallbacks();

  if (am.HasUnused()) {
    std::cerr << "Unused config parameters!" << std::endl;
    std::exit(EXIT_FAILURE);
  } */

  // Create and run simulation
  OrgWorld world(&config);
  world.InitPopulation(); 


  // again adapted to be closer to what we have done before - something wasn't working with the OUTPUT_DIR
  world.SetupOrgFile("worlddata.csv").SetTimingRepeat(40);

  // Run the simulation for the specified number of updates
  for (int update = 0; update < config.RUN_TIME(); update++) {
    std::cout << "Calling update " << update << std::endl;
    world.Update();
  }

  /* std::ofstream ofs(config.OUTPUT_DIR() + "baseline_data.csv");
  ofs << "update,avg_genome_sum\n";

  for (size_t update = 0; update < config.RUN_TIME(); ++update) {
    world.Update();
    if (update % 100 == 0) {
      world.LogStats(update, ofs);
      std::cout << "Update: " << update << std::endl;
    }
  } */

  std::cout << "Simulation complete.\n";
  std::cout << "some text\n";
  return 0;
}