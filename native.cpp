// Compile with `c++ -std=c++17 -Isignalgp-lite/include native.cpp`

#include <iostream>
#include "World.h"
#include "ConfigSetup.h"

/**
 * input: argc (argument count), argv (argument values)
 * output: none (program side-effects: config file read/write, organism world initialized and updated)
 * purpose: Entry point for the NATIVE version of the project. Initializes configuration, sets up the world,
 *          injects organisms, and runs updates over the specified number of generations.
 */
int main(int argc, char *argv[]) {
  // Load or initialize configuration
  WorldConfig config;
  bool success = config.Read("MySettings.cfg");
  if (!success) config.Write("MySettings.cfg");

  // Parse command-line arguments and config file
  auto args = emp::cl::ArgManager(argc, argv);
  if (args.ProcessConfigOptions(config, std::cout, "MySettings.cfg") == false) {
    std::cerr << "There was a problem in processing the options file." << std::endl;
    exit(1);
  }
  if (args.TestUnknown() == false) {
    std::cerr << "Leftover args no good." << std::endl;
    exit(1);
  }

  // Initialize random number generators
  emp::Random random(config.SEED());
  OrgWorld world(random, &config);
  // Ensure SignalGP-Lite uses the same seed
  sgpl::tlrand.Get().ResetSeed(config.SEED());

  // Inject starting organisms into the world
  world.SetPopStruct_Grid(config.NUM_BOXES(), config.NUM_BOXES());
  for (int i = 0; i < config.NUM_START(); i++) {
    Organism* new_org = new Organism(&world, 0);
    world.Inject(*new_org);
  }

  // Set up the world grid and data output
  world.Resize(config.NUM_BOXES(), config.NUM_BOXES());
  
  world.SetupOrgFile("worlddata.csv").SetTimingRepeat(config.NUM_UPDATES()/10);
  //world.SetupOrgFile("worlddata.csv").SetTimingRepeat(40);

  // Run the simulation for the specified number of updates
  for (int update = 0; update < config.NUM_UPDATES(); update++) {
    std::cout << "Calling update " << update << std::endl;
    world.Update();
  }

  // Debug info if needed
  // std::cout << "Num orgs: " << world.GetNumOrgs() << std::endl;
}
