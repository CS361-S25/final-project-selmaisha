#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/data/DataFile.hpp"

#include "Org.h"
#include "Parasite.h"
#include "Host.h"
#include "Task.h"
#include "ConfigSetup.h"


/**
 * A custom world that manages a population of Organism instances and tracks task-solving performance.
 *
 * This class extends emp::World to model an evolutionary simulation in which organisms can solve tasks.
 * It tracks and records data about which tasks have been solved and how many organisms are present.
 */
class OrgWorld : public emp::World<Host> {
  //emp::Random &random;
  emp::vector<emp::WorldPosition> reproduce_queue;
  //emp::vector<std::shared_ptr<Parasite>> parasite_pop;

  WorldConfig *config;
  /// List of task pointers that organisms can solve.
  std::vector<Task*> tasks{
    new NOTTask(),
    new NANDTask(),
    new ANDTask(),
    new ORNTask(),
    new ORTask(),
    new ANDNTask(),
    new NORTask(),
    new XORTask(),
    new EQUTask()
  };

  // Pointers to data monitors for task-solvers
  emp::Ptr<emp::DataMonitor<int>> data_node_org_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_NOT_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_NAND_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_AND_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_ORN_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_OR_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_ANDN_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_NOR_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_XOR_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_EQU_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_DEAD_count;

  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_count;


  int died_of_old_age = 0; //just for testing purposes


public:

  /**
   * input: _random (random number generator), _config (world configuration)
   * output: none
   * purpose: Constructor that initializes the world with configuration and RNG.
   */
  OrgWorld(emp::Random &_random, WorldConfig *_config) 
  : emp::World<Host>(_random), config(_config){  }

  /**
   * input: none
   * output: none
   * purpose: Destructor that cleans up allocated data monitors.
   */
  ~OrgWorld() { 
    if(data_node_org_count) data_node_org_count.Delete();
    if(data_node_NOT_count) data_node_NOT_count.Delete();
    if(data_node_NAND_count) data_node_NAND_count.Delete();
    if(data_node_AND_count) data_node_AND_count.Delete();
    if(data_node_ORN_count) data_node_ORN_count.Delete();
    if(data_node_OR_count) data_node_OR_count.Delete();
    if(data_node_ANDN_count) data_node_ANDN_count.Delete();
    if(data_node_NOR_count) data_node_NOR_count.Delete();
    if(data_node_XOR_count) data_node_XOR_count.Delete();
    if(data_node_EQU_count) data_node_EQU_count.Delete();
    if(data_node_DEAD_count) data_node_DEAD_count.Delete();
  }

  /**
   * input: none
   * output: double (reward value)
   * purpose: Retrieve the configured reward value.
   */
  double GetReward() {
    return config->REWARD();
  }

  /**
   * input: none
   * output: double (mutation rate)
   * purpose: Retrieve the configured mutation rate.
   */
  double GetMutationRate() {
    return config->MUTATION_RATE();
  }

  bool IsParasite(size_t id) const {
    return IsOccupied(id) && pop[id]->HasParasite();
  }

  void InjectParasite(emp::Ptr<Parasite> parasite) {
    // Find an empty slot in the population
    for (size_t i = 0; i < pop.size(); ++i) {
      if (IsOccupied(i)) {
        if (!pop[i]->HasParasite()) {
          // If the slot is occupied by a host, infect it with the parasite
          pop[i]->SetParasite(parasite);
          return;
        }
      }
    }
  }

  //a simple struct to store the number of organisms solving each task
  struct TaskSolverCounts {
    int NOT_count = 0;
    int NAND_count = 0;
    int AND_count = 0;
    int ORN_count = 0;
    int OR_count = 0;
    int ANDN_count = 0;
    int NOR_count = 0;
    int XOR_count = 0;
    int EQU_count = 0;
  };


  /**
   * Input: none  
   * Output: TaskSolvers struct with solver counts for each task
   * Purpose: Count how many organisms in the population have completed each task
   */
  TaskSolverCounts GetAllTaskSolvers() {
    TaskSolverCounts task_solvers;
    for (size_t i = 0; i < pop.size(); ++i) {
      if (IsOccupied(i)) {
        auto& org = pop[i];
        if (org->GetCPU().state.completed_NOT) task_solvers.NOT_count++;
        if (org->GetCPU().state.completed_NAND) task_solvers.NAND_count++;
        if (org->GetCPU().state.completed_AND) task_solvers.AND_count++;
        if (org->GetCPU().state.completed_ORN) task_solvers.ORN_count++;
        if (org->GetCPU().state.completed_OR) task_solvers.OR_count++;
        if (org->GetCPU().state.completed_ANDN) task_solvers.ANDN_count++;
        if (org->GetCPU().state.completed_NOR) task_solvers.NOR_count++;
        if (org->GetCPU().state.completed_XOR) task_solvers.XOR_count++;
        if (org->GetCPU().state.completed_EQU) task_solvers.EQU_count++;
      }
    }
    return task_solvers;
  }

  emp::DataMonitor<int>& GetParasiteCountDataNode() {
    if (!data_node_parasite_count) {
      data_node_parasite_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_count->Reset();
        int count = 0;
        for (size_t i = 0; i < pop.size(); ++i) {
          if (IsOccupied(i) && pop[i]->HasParasite()) {
            ++count;
          }
        }
        data_node_parasite_count->AddDatum(count);
      });
    }
    return *data_node_parasite_count;
  }

  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of organisms in the world
   */
  emp::DataMonitor<int>& GetOrgCountDataNode(){
    if (!data_node_org_count) {
      data_node_org_count.New();
      OnUpdate([this](size_t) {
        data_node_org_count->Reset();
        data_node_org_count->AddDatum(GetNumOrgs());
      });
    }
    return *data_node_org_count;
  }
  
  emp::DataMonitor<int>& GetDeathCountDataNode(){
    if (!data_node_DEAD_count) {
      data_node_DEAD_count.New();
      OnUpdate([this](size_t) {
        data_node_DEAD_count->Reset();
        data_node_DEAD_count->AddDatum(died_of_old_age);
      });
    }
    return *data_node_DEAD_count;
  }

  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of NOT task solvers in the world
   */
  emp::DataMonitor<int>& GetNOTCountDataNode(){
    if (!data_node_NOT_count) {
      data_node_NOT_count.New();
      OnUpdate([this](size_t) {
        data_node_NOT_count->Reset();
        data_node_NOT_count->AddDatum(GetAllTaskSolvers().NOT_count);
      });
    }
    return *data_node_NOT_count;
  }
  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of NAND task solvers in the world
   */
  emp::DataMonitor<int>& GetNANDCountDataNode(){
    if (!data_node_NAND_count) {
      data_node_NAND_count.New();
      OnUpdate([this](size_t) {
        data_node_NAND_count->Reset();
        data_node_NAND_count->AddDatum(GetAllTaskSolvers().NAND_count);
      });
    }
    return *data_node_NAND_count;
  }

  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of AND task solvers in the world
   */
  emp::DataMonitor<int>& GetANDCountDataNode(){
    if (!data_node_AND_count) {
      data_node_AND_count.New();
      OnUpdate([this](size_t) {
        data_node_AND_count->Reset();
        data_node_AND_count->AddDatum(GetAllTaskSolvers().AND_count);
      });
    }
    return *data_node_AND_count;
  }
  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of ORN task solvers in the world
   */
  emp::DataMonitor<int>& GetORNCountDataNode(){
    if (!data_node_ORN_count) {
      data_node_ORN_count.New();
      OnUpdate([this](size_t) {
        data_node_ORN_count->Reset();
        data_node_ORN_count->AddDatum(GetAllTaskSolvers().ORN_count);
      });
    }
    return *data_node_ORN_count;
  }
  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of OR task solvers in the world
   */
  emp::DataMonitor<int>& GetORCountDataNode(){
    if (!data_node_OR_count) {
      data_node_OR_count.New();
      OnUpdate([this](size_t) {
        data_node_OR_count->Reset();
        data_node_OR_count->AddDatum(GetAllTaskSolvers().OR_count);
      });
    }
    return *data_node_OR_count;
  }
  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of ANDN task solvers in the world
   */
  emp::DataMonitor<int>& GetANDNCountDataNode(){
    if (!data_node_ANDN_count) {
      data_node_ANDN_count.New();
      OnUpdate([this](size_t) {
        data_node_ANDN_count->Reset();
        data_node_ANDN_count->AddDatum(GetAllTaskSolvers().ANDN_count);
      });
    }
    return *data_node_ANDN_count;
  }
  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of NOR task solvers in the world
   */
  emp::DataMonitor<int>& GetNORCountDataNode(){
    if (!data_node_NOR_count) {
      data_node_NOR_count.New();
      OnUpdate([this](size_t) {
        data_node_NOR_count->Reset();
        data_node_NOR_count->AddDatum(GetAllTaskSolvers().NOR_count);
      });
    }
    return *data_node_NOR_count;
  }
  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of XOR task solvers in the world
   */
  emp::DataMonitor<int>& GetXORCountDataNode(){
    if (!data_node_XOR_count) {
      data_node_XOR_count.New();
      OnUpdate([this](size_t) {
        data_node_XOR_count->Reset();
        data_node_XOR_count->AddDatum(GetAllTaskSolvers().XOR_count);
      });
    }
    return *data_node_XOR_count;
  }
  /**
   *  Input: none
   * Output: Reference to emp::DataMonitor<int>
   * Purpose: Track the number of EQU task solvers in the world
   */
  emp::DataMonitor<int>& GetEQUCountDataNode(){
    if (!data_node_EQU_count) {
      data_node_EQU_count.New();
      OnUpdate([this](size_t) {
        data_node_EQU_count->Reset();
        data_node_EQU_count->AddDatum(GetAllTaskSolvers().EQU_count);
      });
    }
    return *data_node_EQU_count;
  }

  /** 
   * Input: optional filename (default "worlddata.csv")
   * Output: Reference to emp::DataFile
   * Purpose: Set up a data file that logs task-solving and population statistics over time
   */
  emp::DataFile & SetupOrgFile(const std::string & filename="data/worlddata.csv") {
    auto & file = SetupFile(filename);
    auto & node1 = GetOrgCountDataNode();
    auto & node2 = GetNOTCountDataNode();
    auto & node3 = GetNANDCountDataNode();
    auto & node4 = GetANDCountDataNode();
    auto & node5 = GetORNCountDataNode();
    auto & node6 = GetORCountDataNode();
    auto & node7 = GetANDNCountDataNode();
    auto & node8 = GetNORCountDataNode();
    auto & node9 = GetXORCountDataNode();
    auto & node10 = GetEQUCountDataNode();
    auto & node11 = GetDeathCountDataNode();
    auto & node12 = GetParasiteCountDataNode();

    
    file.AddVar(update, "update", "Update");
    file.AddTotal(node1, "org", "Total number of organisms");
    file.AddTotal(node2, "not", "Number of orgs solving not");
    file.AddTotal(node3, "nand", "Number of orgs solving nand");
    file.AddTotal(node4, "and", "Number of orgs solving and");
    file.AddTotal(node5, "orn", "Number of orgs solving orn");
    file.AddTotal(node6, "or", "Number of orgs solving or");
    file.AddTotal(node7, "andn", "Number of orgs solving andn");
    file.AddTotal(node8, "nor", "Number of orgs solving nor");
    file.AddTotal(node9, "xor", "Number of orgs solving xor");
    file.AddTotal(node10, "equ", "Number of orgs solving equ");
    file.AddTotal(node11, "dead", "Number of orgs that died of old age");
    file.AddTotal(node12, "parasites", "Number of parasites present");

    file.PrintHeaderKeys();

    return file;
  }

  /**
   * Input: none
   * Output: Const reference to organism population
   * Purpose: Provide access to the current population vector
   */
  const pop_t &GetPopulation() { return pop; }

    bool SolvedSameTask(emp::Ptr<Host> org, emp::Ptr<Parasite> parasite) {
      const OrgState & host_state = org->GetCPU().state;
      const OrgState & parasite_state = parasite->GetCPU().state;
      // const OrgState & parasite_state = parasite->GetCPU().GetState();


      bool result = false;

      if (host_state.completed_NOT && parasite_state.completed_NOT) {
        result = true;
      } else if (host_state.completed_NAND && parasite_state.completed_NAND) {
        result = true;
      } else if (host_state.completed_AND && parasite_state.completed_AND) {
        result = true;
      } else if (host_state.completed_ORN && parasite_state.completed_ORN) {
        result = true;
      } else if (host_state.completed_OR && parasite_state.completed_OR) {
        result = true;
      } else if (host_state.completed_ANDN && parasite_state.completed_ANDN) {
        result = true;
      } else if (host_state.completed_NOR && parasite_state.completed_NOR) {
        result = true;
      } else if (host_state.completed_XOR && parasite_state.completed_XOR) {
        result = true;
      } else if (host_state.completed_EQU && parasite_state.completed_EQU) {
        result = true;
      }
      return result;
    }

  /**
   * Input: none
   * Output: none
   * Purpose: Update the world by running each organism and handling reproduction
   */
  void Update() {
    emp::World<Host>::Update();
    double mutation_rate = config->MUTATION_RATE();

    //Process each organism
    emp::vector<size_t> schedule = emp::GetPermutation(GetRandom(), GetSize());
    
    for (int i : schedule) {
      if (!IsOccupied(i)) { continue; }
      pop[i]->Process(*this, i);

      //check if parasite survived the cycle
      if (pop[i]->HasParasite()) {
        double points = pop[i]->GetParasite()->GetPoints();
        if (points < -5.0) { //or should it be 0
          // Parasite is dead, remove it
          pop[i]->RemoveParasite();
        }
      }
    }

    for (int i : schedule){
      if (!IsOccupied(i)) { continue; }
      emp::Ptr<Host> host = pop[i];
      if (!host->HasParasite()) { continue; } // Skip if no parasite
      emp::Ptr<Parasite> parasite = host->GetParasite();
      if (SolvedSameTask(host, parasite)) {
        parasite->AddPoints(GetVirulence() * GetReward());
        host->AddPoints(-GetVirulence() * GetReward()); // Host loses points
      }
    }



    //Time to allow reproduction for any organisms that ran the reproduce instruction
    for (emp::WorldPosition location : reproduce_queue) {
      if (!IsOccupied(location)) {
        return;
      }
      emp::Ptr<Host> org = pop[location.GetIndex()];
      std::optional<std::unique_ptr<Organism>> offspring =
          org->CheckReproduction(mutation_rate);
      if (offspring.has_value()) {
        Host* host_offspring = dynamic_cast<Host*>(offspring.value().get());
        if (host_offspring != nullptr) {
          DoBirth(*host_offspring, location.GetIndex());
          org->ResetAge();
        }
      }
    }
    reproduce_queue.clear();

    //check for death after reproduction
    for (int i : schedule) {
      if (!IsOccupied(i)) { continue; }
      emp::WorldPosition current_location(i);
      auto & org = pop[i];
      if (org->IsDead(config->LIFE_SPAN())) {
        RemoveOrgAt(current_location);
        died_of_old_age++;
      }
    }
  }

  bool IsDeadOrg(emp::WorldPosition location) {
    return pop[location.GetIndex()]->IsDead(config->LIFE_SPAN());
  }

  double GetVirulence() const { return config->VIRULENCE(); }


  /**
   * Input: float output, OrgState &state
   * Output: none
   * Purpose: Check if an organism's output solves any task and assign points
   */
  //DO WE WANT TO HANDLE ALL POINTS IN THE UPDATE, AND JUST CHECK OUTPUTS HERE?
  //OR CAN WE MAKE THIS APPLY TO ONLY ORGS AND NOT PARASITES?
  void CheckOutput(float output, OrgState &state) {
    for (Task *task : tasks) {
      bool success = task->CheckOutput(output, state.last_inputs);
      if (!success) {
        //state.points += 1;
        continue;
      }
        std::cout << "Org at: " << state.current_location.GetIndex() << std::endl;
/*         std::cout << "input1: " << state.last_inputs[0] << std::endl;
        std::cout << "input2: " << state.last_inputs[1] << std::endl;
        std::cout << "input3: " << state.last_inputs[2] << std::endl;
        std::cout << "input4: " << state.last_inputs[3] << std::endl; */
        std::cout << "output: " << output << std::endl;
        std::cout << "Task solved: " << task->name() << std::endl;
      //double newPoints =  config->TASK_REWARD();
      double newPoints = config->REWARD();
      if (!state.isParasite){
        //if it is an organism, add points
        //parasite points are handled later
        state.points += newPoints;
      }
      SetTaskVars(task, state);
    }
    //std::cout << "Points after task check: " << state.points << std::endl;
  }

  /**
   * Input: Task pointer, OrgState reference
   * Output: none
   * Purpose: Mark completion flags in the organism’s state based on the task type
   */
  void SetTaskVars(Task *task, OrgState &state) {
    if (task->name() == "NOT") {
      state.completed_NOT = true;
    } else if (task->name() == "NAND") {
      state.completed_NAND = true;
    } else if (task->name() == "AND") {
      state.completed_AND = true;
    } else if (task->name() == "ORN") {
      state.completed_ORN = true;
    } else if (task->name() == "OR") {
      state.completed_OR = true;
    } else if (task->name() == "ANDN") {
      state.completed_ANDN = true;
    } else if (task->name() == "NOR") {
      state.completed_NOR = true;
    } else if (task->name() == "XOR") {
      state.completed_XOR = true;
    } else if (task->name() == "EQU") {
      state.completed_EQU = true;
    }
  }

  /**
   * Input: emp::WorldPosition location
   * Output: none
   * Purpose: Queue the organism for reproduction at the end of the update cycle
   */
  void ReproduceOrg(emp::WorldPosition location) {
    // Wait until after all organisms have been processed to perform
    // reproduction. If reproduction happened immediately then the child could
    // ovewrite the parent, and then we would be running the code of a deleted
    // organism
    reproduce_queue.push_back(location);
  }
};

#endif