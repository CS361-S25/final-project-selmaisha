// Compile with `c++ -std=c++17 -Isignalgp-lite/include native.cpp`

#include <iostream>
#include "World.h"
#include "ConfigSetup.h"



// const std::string nand_prog_src = R"(
// NAND 0 1 2
// IO 0
// REPRODUCE
// )";

// sgpl::Program<Spec> BuildNANDProgram() {
//    sgpl::Program<Spec> prog(nand_prog_src);
//    return prog;
// }

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
  // Ensure SignalGP-Lite uses the same seed
  sgpl::tlrand.Get().ResetSeed(config.SEED());

  size_t INJECT_PARASITES_UPDATE = config.INJECT_PARASITES_AT();

  

  // Inject starting organisms into the world
  //world.SetPopStruct_Grid(config.NUM_BOXES(), config.NUM_BOXES());
  world.SetPopStruct_Mixed();
  for (int i = 0; i < config.NUM_START(); i++) {
    emp::Ptr<Host> new_org = new Host(&world, 0);
    if (random.P(0.95)) {
      new_org->GetCPU().LoadProgram(BuildANDNProgram());
    }

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
  
  //world.SetupOrgFile("worlddata.csv").SetTimingRepeat(config.NUM_UPDATES()/100);
  world.SetupOrgFile("worlddata.csv").SetTimingRepeat(1);

/*   //HAVE SOMETHING LIKE THIS TO INITIALIZE THEM WITH A TASK
  // Pseudocode
  sgpl::Program<Spec> nand_prog;
  // Fill nand_prog with instructions that solve NAND
  parasite->GetCPU().SetProgram(nand_prog); */

  

  // Run the simulation for the specified number of updates
  for (int update = 0; update < config.NUM_UPDATES(); update++) {
    std::cout << "Calling update " << update << std::endl;
    world.Update();
    if (update == INJECT_PARASITES_UPDATE) {
      std::cout << "Injecting parasites" << std::endl;
      // Inject parasites into the world

      //make this into a fcn of world, since we use it here and in parasite reproduction logic
      // Find all eligible hosts that have solved at least one task
      std::vector<size_t> eligible_hosts;
      for (size_t i = 0; i < world.GetSize(); ++i) {
        if (!world.IsOccupied(i)) continue;
        auto& host = world.GetOrg(i);
        if (!host.HasParasite()){
          //if (!host) continue; //this caused an error and I don't think its needed with the check above

          // Check if the host has solved at least one task
          if (host.canSolveTask()) {
            eligible_hosts.push_back(i);
          }
          //eligible_hosts.push_back(i); // For now, consider all hosts 
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
          //world.AddParasiteToTracking(new_parasite); // if needed
        } else {
          delete new_parasite; // prevent memory leak
        }
      }
      // for (int i = 0; i < config.NUM_PARASITES(); i++) {
      //   Parasite* new_parasite = new Parasite(&world, -1.0);
      //   new_parasite->GetCPU().LoadProgram(BuildNANDProgram());
      //   world.InjectParasite(new_parasite);
      //   // Set the parasite's virulence
      //   new_parasite->setVirulence(config.VIRULENCE());
      // }
    }
  }

  //access worlddate.csv and creat a plot of each of the columns over time
  // Note: The data file is automatically written to by the world during updates.
  // You can use tools like Python's pandas or matplotlib to visualize the data.
  std::cout << "Simulation complete. Data written to worlddata.csv." << std::endl;
  //access file and create plot

  // Debug info if needed
  // std::cout << "Num orgs: " << world.GetNumOrgs() << std::endl;
}
