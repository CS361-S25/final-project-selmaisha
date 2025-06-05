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
  //world.SetPopStruct_Grid(config.NUM_BOXES(), config.NUM_BOXES());
  world.SetPopStruct_Mixed();
  for (int i = 0; i < config.NUM_START(); i++) {
    Host* new_org = new Host(&world, 0);
    world.Inject(*new_org);
    //initialize without parasites - they must be added later
  }
  //WHEN I INITIATE LIKE ABOVE IT SURVIVES; IF I DO LIKE BELOW IT GOES EXTINCT...
/*   for (int i = 0; i < config.NUM_START(); ++i) {
    size_t pos = world.GetRandom().GetUInt(world.GetSize());
    Host* host = new Host(&world, 0);
    world.InjectAt(*host, pos);
  } */

  // Set up the world grid and data output
  world.Resize(config.NUM_BOXES(), config.NUM_BOXES());
  
  world.SetupOrgFile("worlddata.csv").SetTimingRepeat(config.NUM_UPDATES()/100);
  //world.SetupOrgFile("worlddata.csv").SetTimingRepeat(40);

/*   //HAVE SOMETHING LIKE THIS TO INITIALIZE THEM WITH A TASK
  // Pseudocode
  sgpl::Program<Spec> nand_prog;
  // Fill nand_prog with instructions that solve NAND
  parasite->GetCPU().SetProgram(nand_prog); */

  // Run the simulation for the specified number of updates
  for (int update = 0; update < config.NUM_UPDATES(); update++) {
    std::cout << "Calling update " << update << std::endl;
    world.Update();
    if (update == 3000){
      std::cout << "Injecting parasites" << std::endl;
      // Inject parasites into the world
      for (int i = 0; i < config.NUM_PARASITES(); i++) {
        Parasite* new_parasite = new Parasite(&world, -1.0);
        world.InjectParasite(new_parasite);
        // Set the parasite's virulence
        new_parasite->setVirulence(config.VIRULENCE());
      }
    }
  }

  // Debug info if needed
  // std::cout << "Num orgs: " << world.GetNumOrgs() << std::endl;
}
