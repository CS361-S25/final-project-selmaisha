#include "World.h"
#include "ConfigSetup.h"
#include <iostream>

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





  // Create and run simulation

  emp::Random random(config.SEED());
  OrgWorld world(random, &config);
  world.InitPopulation(); 


  // again adapted to be closer to what we have done before - something wasn't working with the OUTPUT_DIR
  world.SetupOrgFile("worlddata.csv").SetTimingRepeat(100);

  // Run the simulation for the specified number of updates
  for (int update = 0; update < config.RUN_TIME(); update++) {
    std::cout << "Calling update " << update << std::endl;
    world.Update();
    
  }

  std::cout << "Simulation complete.\n";
  std::cout << "some text\n";
  return 0;
}