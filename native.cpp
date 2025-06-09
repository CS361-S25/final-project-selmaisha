// Compile with `c++ -std=c++17 -Isignalgp-lite/include native.cpp`

#include <iostream>
#include "World.h"
#include "ConfigSetup.h"


/**
 * Build a simple ANDN-solving organism program.
 * input: none
 * output: Program<Spec>
 * purpose: Returns a short program that solves ANDN, performs IO, and tries to reproduce.
 */
sgpl::Program<Spec> BuildANDNProgram() {
  const std::string andn_prog_src = R"(
  ANDN 0 1 2
  IO 0
  REPRODUCE
  )";
  return sgpl::Program<Spec>(andn_prog_src); // This parses the string
}



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
  sgpl::tlrand.Get().ResetSeed(config.SEED());

  // Inject initial population (hosts only, no parasites yet)
  size_t INJECT_PARASITES_UPDATE = config.INJECT_PARASITES_AT();

  world.SetPopStruct_Mixed();
  for (int i = 0; i < config.NUM_START(); i++) {
    emp::Ptr<Host> new_org = new Host(&world, 0);
    if (random.P(0.95)) {
      new_org->GetCPU().LoadProgram(BuildANDNProgram());
    }

    world.Inject(*new_org); // inject by dereferencing pointer
  }


  // Set up the world grid and data output
  world.Resize(config.NUM_BOXES(), config.NUM_BOXES());
  world.SetupOrgFile("worlddata.csv").SetTimingRepeat(1);


  // Run the simulation for the specified number of updates
  for (int update = 0; update < config.NUM_UPDATES(); update++) {
    std::cout << "Calling update " << update << std::endl;
    world.Update();

    // Inject parasites at the configured update point
    if (update == INJECT_PARASITES_UPDATE) {
      std::cout << "Injecting parasites" << std::endl;

      // Find all eligible hosts that have solved at least one task
      std::vector<size_t> eligible_hosts;
      for (size_t i = 0; i < world.GetSize(); ++i) {
        if (!world.IsOccupied(i)) continue;
        auto& host = world.GetOrg(i);
        if (!host.HasParasite()){

          // Check if the host has solved at least one task
          if (host.canSolveTask()) {
            eligible_hosts.push_back(i);
          }
        }
      }

      // Inject parasites into eligible hosts
      emp::Random& rnd = world.GetRandom();
      for (int i = 0; i < config.NUM_PARASITES() && !eligible_hosts.empty(); ++i) {
        size_t index = rnd.GetUInt(eligible_hosts.size());
        size_t host_pos = eligible_hosts[index];
        eligible_hosts.erase(eligible_hosts.begin() + index); // avoid re-using

        emp::Ptr<Parasite> new_parasite = new Parasite(&world, -1.0);
        new_parasite->GetCPU().LoadProgram(BuildANDNProgram());
        new_parasite->setVirulence(config.VIRULENCE());

        // Inject directly into the host
        emp::Ptr<Host> host = world.GetOrgPtr(host_pos);
        if (host && !host->HasParasite()) {
          host->SetParasite(new_parasite);
        } else {
          delete new_parasite; // prevent memory leak
        }
      }
    }
  }

  
  std::cout << "Simulation complete. Data written to worlddata.csv." << std::endl;

}
