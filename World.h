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
  emp::vector<emp::WorldPosition> parasite_reproduce_queue;

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

  // Parasite task counters
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_NOT_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_NAND_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_AND_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_ORN_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_OR_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_ANDN_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_NOR_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_XOR_count;
  emp::Ptr<emp::DataMonitor<int>> data_node_parasite_EQU_count;


  int died_of_old_age = 0; //just for testing purposes
  size_t update_num = 0; //just for testing purposes - something is off with this->update
  size_t BONUS_UPDATE_LIMIT = 1000;
  //DO THE SAME FOR INJECT PARASITES AT
  size_t INJECT_PARASITES_AT = 1500; //update at which parasites are injected into the world

  //adaptations to help parasites form a stable population
  //"first stage" refers to the first [BONUS_UPDATE_LIMIT] updates after their injection
  bool parasite_life_bonus = true; //whether parasites should have a life bonus in the first stage
  bool parasite_task_bonus = true; //whether they get a bonus for solving tasks in the first stage
  bool parasite_reproduce_bonus = true; //whether they get a bonus for reproduction in the first stage
  bool host_protection = true; //whether hosts are protected from losing points to parasites in the first stage
  bool use_parasite_task_flags = false; //whether to use permanent task-flags or just the last solved task for parasites
  bool custom_birth = true; //whether to allow custom birth strategies for hosts do reduce the probability of overwriting task solvers
  bool parasite_multiple_reproduction = true; //whether parasites can reproduce multiple times

  bool allow_all_hosts = false;


public:

  /**
   * input: _random (random number generator), _config (world configuration)
   * output: none
   * purpose: Constructor that initializes the world with configuration and RNG.
   */
  OrgWorld(emp::Random &_random, WorldConfig *_config) 
  : emp::World<Host>(_random), config(_config){
    BONUS_UPDATE_LIMIT = config->BONUS_UPDATE_LIMIT();
    INJECT_PARASITES_AT = config->INJECT_PARASITES_AT();
   }

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
    
    if(data_node_parasite_count) data_node_parasite_count.Delete();

    if(data_node_parasite_NOT_count) data_node_parasite_NOT_count.Delete();
    if(data_node_parasite_NAND_count) data_node_parasite_NAND_count.Delete();
    if(data_node_parasite_AND_count) data_node_parasite_AND_count.Delete();
    if(data_node_parasite_ORN_count) data_node_parasite_ORN_count.Delete();
    if(data_node_parasite_OR_count) data_node_parasite_OR_count.Delete();
    if(data_node_parasite_ANDN_count) data_node_parasite_ANDN_count.Delete();
    if(data_node_parasite_NOR_count) data_node_parasite_NOR_count.Delete();
    if(data_node_parasite_XOR_count) data_node_parasite_XOR_count.Delete();
    if(data_node_parasite_EQU_count) data_node_parasite_EQU_count.Delete();
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
  double GetHostMutationRate() {
    return config->MUTATION_RATE();
  }
  /**
   * input: none
   * output: double (mutation rate)
   * purpose: Retrieve the configured parasite mutation rate.
   */
  double GetParasiteMutationRate() {
    return config->PARASITE_MUTATION_RATE();
  }

  double GetParasiteVirulence() {
    return config->VIRULENCE();
  }

  size_t GetParasiteInjectionUpdate(){
    return config->INJECT_PARASITES_AT();
  }

  bool IsParasite(size_t id) const {
    return IsOccupied(id) && pop[id]->HasParasite();
  }

  void InjectParasite(emp::Ptr<Parasite> parasite) {
    // Find an empty slot in the population
    // for (size_t i = 0; i < pop.size(); ++i) {
    //   if (IsOccupied(i)) {
    //     if (!pop[i]->HasParasite()) {
    //       // If the slot is occupied by a host, infect it with the parasite
    //       pop[i]->SetParasite(parasite);
    //       return;
    //     }
    //   }
    // }
    auto shuffled_ids = emp::GetPermutation(GetRandom(), pop.size());

    for (size_t i : shuffled_ids) {
      if (IsOccupied(i) && !pop[i]->HasParasite()) {
        pop[i]->SetParasite(parasite);
        return;
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
        if (org->GetCPURef().state.completed_NOT) task_solvers.NOT_count++;
        if (org->GetCPURef().state.completed_NAND) task_solvers.NAND_count++;
        if (org->GetCPURef().state.completed_AND) task_solvers.AND_count++;
        if (org->GetCPURef().state.completed_ORN) task_solvers.ORN_count++;
        if (org->GetCPURef().state.completed_OR) task_solvers.OR_count++;
        if (org->GetCPURef().state.completed_ANDN) task_solvers.ANDN_count++;
        if (org->GetCPURef().state.completed_NOR) task_solvers.NOR_count++;
        if (org->GetCPURef().state.completed_XOR) task_solvers.XOR_count++;
        if (org->GetCPURef().state.completed_EQU) task_solvers.EQU_count++;
      }
    }
    return task_solvers;
  }

  TaskSolverCounts GetParasiteTaskSolvers() {
    TaskSolverCounts counts;
    for (size_t i = 0; i < pop.size(); ++i) {
      if (IsOccupied(i) && pop[i]->HasParasite()) {
        const auto& state = pop[i]->GetParasite()->GetCPURef().state;
        if (state.completed_NOT) counts.NOT_count++;
        if (state.completed_NAND) counts.NAND_count++;
        if (state.completed_AND) counts.AND_count++;
        if (state.completed_ORN) counts.ORN_count++;
        if (state.completed_OR) counts.OR_count++;
        if (state.completed_ANDN) counts.ANDN_count++;
        if (state.completed_NOR) counts.NOR_count++;
        if (state.completed_XOR) counts.XOR_count++;
        if (state.completed_EQU) counts.EQU_count++;
      }
    }
    return counts;
  }

  emp::DataMonitor<int>& GetParasiteNOTCountDataNode() {
    if (!data_node_parasite_NOT_count) {
      data_node_parasite_NOT_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_NOT_count->Reset();
        data_node_parasite_NOT_count->AddDatum(GetParasiteTaskSolvers().NOT_count);
      });
    }
    return *data_node_parasite_NOT_count;
  }

  emp::DataMonitor<int>& GetParasiteNANDCountDataNode() {
    if (!data_node_parasite_NAND_count) {
      data_node_parasite_NAND_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_NAND_count->Reset();
        data_node_parasite_NAND_count->AddDatum(GetParasiteTaskSolvers().NAND_count);
      });
    }
    return *data_node_parasite_NAND_count;
  }

  emp::DataMonitor<int>& GetParasiteANDCountDataNode() {
    if (!data_node_parasite_AND_count) {
      data_node_parasite_AND_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_AND_count->Reset();
        data_node_parasite_AND_count->AddDatum(GetParasiteTaskSolvers().AND_count);
      });
    }
    return *data_node_parasite_AND_count;
  }

  emp::DataMonitor<int>& GetParasiteORNCountDataNode() {
    if (!data_node_parasite_ORN_count) {
      data_node_parasite_ORN_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_ORN_count->Reset();
        data_node_parasite_ORN_count->AddDatum(GetParasiteTaskSolvers().ORN_count);
      });
    }
    return *data_node_parasite_ORN_count;
  }

  emp::DataMonitor<int>& GetParasiteORCountDataNode() {
    if (!data_node_parasite_OR_count) {
      data_node_parasite_OR_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_OR_count->Reset();
        data_node_parasite_OR_count->AddDatum(GetParasiteTaskSolvers().OR_count);
      });
    }
    return *data_node_parasite_OR_count;
  }

  emp::DataMonitor<int>& GetParasiteANDNCountDataNode() {
    if (!data_node_parasite_ANDN_count) {
      data_node_parasite_ANDN_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_ANDN_count->Reset();
        data_node_parasite_ANDN_count->AddDatum(GetParasiteTaskSolvers().ANDN_count);
      });
    }
    return *data_node_parasite_ANDN_count;
  }

  emp::DataMonitor<int>& GetParasiteNORCountDataNode() {
    if (!data_node_parasite_NOR_count) {
      data_node_parasite_NOR_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_NOR_count->Reset();
        data_node_parasite_NOR_count->AddDatum(GetParasiteTaskSolvers().NOR_count);
      });
    }
    return *data_node_parasite_NOR_count;
  }

  emp::DataMonitor<int>& GetParasiteXORCountDataNode() {
    if (!data_node_parasite_XOR_count) {
      data_node_parasite_XOR_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_XOR_count->Reset();
        data_node_parasite_XOR_count->AddDatum(GetParasiteTaskSolvers().XOR_count);
      });
    }
    return *data_node_parasite_XOR_count;
  }

  emp::DataMonitor<int>& GetParasiteEQUCountDataNode() {
    if (!data_node_parasite_EQU_count) {
      data_node_parasite_EQU_count.New();
      OnUpdate([this](size_t) {
        data_node_parasite_EQU_count->Reset();
        data_node_parasite_EQU_count->AddDatum(GetParasiteTaskSolvers().EQU_count);
      });
    }
    return *data_node_parasite_EQU_count;
  }





  //////////////
  // emp::DataMonitor<std::string>& GetParasiteTasksDataNode() {
  //   if (!data_node_parasite_tasks) {
  //     data_node_parasite_tasks.New();
  //   }
  //   return *data_node_parasite_tasks;
  // }

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
    auto & paranode1 = GetParasiteNOTCountDataNode();
    auto & paranode2 = GetParasiteNANDCountDataNode();
    auto & paranode3 = GetParasiteANDCountDataNode();
    auto & paranode4 = GetParasiteORNCountDataNode();
    auto & paranode5 = GetParasiteORCountDataNode();
    auto & paranode6 = GetParasiteANDNCountDataNode();
    auto & paranode7 = GetParasiteNORCountDataNode();
    auto & paranode8 = GetParasiteXORCountDataNode();
    auto & paranode9 = GetParasiteEQUCountDataNode();

    
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
    file.AddTotal(paranode1, "parasite_not", "Number of parasites solving not");
    file.AddTotal(paranode2, "parasite_nand", "Number of parasites solving nand");
    file.AddTotal(paranode3, "parasite_and", "Number of parasites solving and");
    file.AddTotal(paranode4, "parasite_orn", "Number of parasites solving orn");
    file.AddTotal(paranode5, "parasite_or", "Number of parasites solving or");
    file.AddTotal(paranode6, "parasite_andn", "Number of parasites solving andn");
    file.AddTotal(paranode7, "parasite_nor", "Number of parasites solving nor");
    file.AddTotal(paranode8, "parasite_xor", "Number of parasites solving xor");
    file.AddTotal(paranode9, "parasite_equ", "Number of parasites solving equ");

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
      const OrgState & host_state = org->GetCPURef().state;
      const OrgState & parasite_state = parasite->GetCPURef().state;

      if (use_parasite_task_flags) { //use task flags
        if (parasite_state.completed_NOT && host_state.completed_NOT)  return true;
        else if (parasite_state.completed_NAND && host_state.completed_NAND) return true;
        else if (parasite_state.completed_AND && host_state.completed_AND)  return true; 
        else if (parasite_state.completed_ORN && host_state.completed_ORN)  return true;
        else if (parasite_state.completed_OR && host_state.completed_OR)   return true;
        else if (parasite_state.completed_ANDN && host_state.completed_ANDN) return true;
        else if (parasite_state.completed_NOR && host_state.completed_NOR)  return true;
        else if (parasite_state.completed_XOR && host_state.completed_XOR)  return true;
        else if (parasite_state.completed_EQU && host_state.completed_EQU)  return true;
        else return false;
      } else { //use last_solved_tasks
        for (const std::string& task : parasite_state.last_solved_tasks) {
          if      (task == "NOT"  && host_state.completed_NOT)  return true;
          else if (task == "NAND" && host_state.completed_NAND) return true;
          else if (task == "AND"  && host_state.completed_AND)  return true;
          else if (task == "ORN"  && host_state.completed_ORN)  return true;
          else if (task == "OR"   && host_state.completed_OR)   return true;
          else if (task == "ANDN" && host_state.completed_ANDN) return true;
          else if (task == "NOR"  && host_state.completed_NOR)  return true;
          else if (task == "XOR"  && host_state.completed_XOR)  return true;
          else if (task == "EQU"  && host_state.completed_EQU)  return true;
        }
        return false;
      }
    }

  /**
   * Input: none
   * Output: none
   * Purpose: Update the world by running each organism and handling reproduction
   */
  void Update() {
    update_num++;
    emp::World<Host>::Update();
    double host_mutation_rate = GetHostMutationRate();
    double parasite_mutation_rate = GetParasiteMutationRate();
    died_of_old_age = 0; //reset the counter for each update
    size_t INJECT_PARASITES_AT = GetParasiteInjectionUpdate();

    /* // Count how many parasites have at least one "Reproduce" instruction in their genome
    int parasites_with_reproduce = 0;
    for (size_t i = 0; i < pop.size(); ++i) {
      if (IsOccupied(i) && pop[i]->HasParasite()) {
        emp::Ptr<Parasite> parasite = pop[i]->GetParasite();
        if (!parasite) continue;
        auto cpu = parasite->GetCPURef(); // Defensive: GetCPURef() returns by value
        const auto& program = cpu.GetProgram();
        int reproduce_count = 0;
        for (size_t j = 0; j < program.size(); ++j) {
          if (program[j].GetOpName() == "Reproduce") {
            ++reproduce_count;
          }
        }
        if (reproduce_count > 0) {
          ++parasites_with_reproduce;
        }
        // // Optional: print per-parasite info
        // std::cout << "Parasite at index " << i
        //           << " has " << reproduce_count
        //           << " REPRODUCE instructions in its genome." << std::endl;
      }
    }
    std::cout << "Total parasites with at least one REPRODUCE instruction: "
          << parasites_with_reproduce << std::endl; */


    //start with clearing parasite's last solved tasks
    for (size_t i = 0; i < pop.size(); ++i) {
      if (IsOccupied(i) && pop[i]->HasParasite()) {
        // std::cout << "parasite at index " << i 
        //           << " has points: " << pop[i]->GetParasite()->GetPoints() 
        //           << std::endl;
        //pop[i]->GetParasite()->ClearTaskFlags();
        pop[i]->GetParasite()->GetCPURef().state.last_solved_tasks.clear();
        auto & cpu = pop[i]->GetParasite()->GetCPURef();
        cpu.state.updates_since_task++;

        //if any task flags are true, add to parasite reproduction queue
        if (pop[i]->GetParasite()->GetCPURef().state.completed_NOT ||
            pop[i]->GetParasite()->GetCPURef().state.completed_NAND ||
            pop[i]->GetParasite()->GetCPURef().state.completed_AND ||
            pop[i]->GetParasite()->GetCPURef().state.completed_ORN ||
            pop[i]->GetParasite()->GetCPURef().state.completed_OR ||
            pop[i]->GetParasite()->GetCPURef().state.completed_ANDN ||
            pop[i]->GetParasite()->GetCPURef().state.completed_NOR ||
            pop[i]->GetParasite()->GetCPURef().state.completed_XOR ||
            pop[i]->GetParasite()->GetCPURef().state.completed_EQU) {
          pop[i]->GetParasite()->AddPoints(1); // Parasite gets a small bonus for solving a task
          parasite_reproduce_queue.push_back(emp::WorldPosition(i));
        }
      }
    }

    //Process each organism
    emp::vector<size_t> schedule = emp::GetPermutation(GetRandom(), GetSize());
    
    for (int i : schedule) {
      if (!IsOccupied(i)) { continue; }
      //pop[i]->ClearTaskFlags();  
      pop[i]->Process(*this, i);
      if (this->update_num <= 100) { //give the first 5 updates a bonus - set this as a variable
          pop[i]->AddPoints(5); 
        }

      if (pop[i]->HasParasite()) {
        emp::Ptr<Parasite> parasite = pop[i]->GetParasite();
        //parasite->ClearTaskFlags(); // Clear parasite flags before processing
        parasite->Process(*this, i);
        if ( ((this->update_num)-INJECT_PARASITES_AT) < BONUS_UPDATE_LIMIT/2 && parasite_life_bonus) {
          // std::cout << "Parasite at index " << i 
          //           << " received mini bonus: " << parasite->GetPoints() 
          //           << std::endl;
          parasite->AddPoints(3);
        }
      }

      //check if parasite survived the cycle
      if (pop[i]->HasParasite()) {
        double points = pop[i]->GetParasite()->GetPoints();
        if (points <= -1.0) { //or should it be 0
          // Parasite is dead, remove it
          std::cout << "Parasite at index " << i << " died with points: " << points << std::endl;
          pop[i]->RemoveParasite();
          //hypothesis: parasites are dying because their hosts are overwritten
          //parasites need to be better at reproducing I think
        }
        auto & cpu = pop[i]->GetParasite()->GetCPURef();
        auto & state = cpu.state;
        if (state.updates_since_task > 100) {
          std::cout << "Parasite at index " << i << " removed after " 
                    << state.updates_since_task << " updates without solving a task." << std::endl;
          pop[i]->RemoveParasite();
        }
      }
    }

    //check for host death before parasite steals points
    for (int i : schedule) {
      if (!IsOccupied(i)) { continue; }
      emp::WorldPosition current_location(i);
      auto & org = pop[i];
      if (org->IsDead(config->LIFE_SPAN())) {
        if (org->HasParasite()) {
          // Parasite dies with the host
          std::cout << "Parasite at index " << i 
                    << " died with host at index " << i 
                    << " after reaching lifespan." << std::endl;
          org->RemoveParasite();
        }
        //std::cout << "Organism at index " << i << " died of old age." << std::endl;
        RemoveOrgAt(current_location);
        died_of_old_age++;
      }
    }

    // Handle points for parasites and hosts that solved the same task
    for (int i : schedule){
      if (!IsOccupied(i)) { continue; }
      emp::Ptr<Host> host = pop[i];
      if (!host->HasParasite()) { continue; } // Skip if no parasite
      emp::Ptr<Parasite> parasite = host->GetParasite();
      //check if parasite deserves points
      if (SolvedSameTask(host, parasite)) {
        parasite->AddPoints(3 * GetVirulence() * GetReward());
        if (this->update_num-INJECT_PARASITES_AT > (BONUS_UPDATE_LIMIT/2) || !host_protection) { //only punish host after a while
          host->AddPoints(-GetVirulence() * GetReward()); // Host loses points
        }
        std::cout << "Parasite at index " << i 
                  << " solved the same task as host"
                  << " and has now points: " << parasite->GetPoints() 
                  << " (host points: " << host->GetPoints() << ")"
                  << " adding to reproduction queue."
                  << std::endl;
        parasite_reproduce_queue.push_back(emp::WorldPosition(i)); // Add parasite to reproduction queue
      }
    }



    //Time to allow reproduction for any organisms that ran the reproduce instruction
    for (emp::WorldPosition location : reproduce_queue) {
      if (!IsOccupied(location)) continue; // Skip if the location is not occupied
      emp::Ptr<Host> org = pop[location.GetIndex()];
      std::optional<std::unique_ptr<Organism>> offspring =
          org->CheckReproduction(host_mutation_rate);
          
      if (offspring.has_value()) {
        emp::Ptr<Host> host_offspring(dynamic_cast<Host*>(offspring.value().release()));
        if (host_offspring) {
          if (custom_birth){
            customDoBirth(host_offspring, location.GetIndex());
          } else {
            DoBirth(*host_offspring, location.GetIndex());
          }
          org->AddPoints(-1); // Deduct points for reproduction
          org->ResetAge();
        }
      }
    }
    reproduce_queue.clear();

    // Handle reproduction for parasites
    // this is last to ensure that parasites are injected into surviving host
    
    // Build a list of viable hosts (alive, not already infected)
    std::vector<size_t> viable_hosts;
    for (size_t i = 0; i < pop.size(); ++i) {
      if (IsOccupied(i) && !pop[i]->HasParasite()) {
        emp::Ptr<Host> host = pop[i];
        //initially allow all hosts to have parasites
        if (this->update-INJECT_PARASITES_AT < BONUS_UPDATE_LIMIT && allow_all_hosts) {
          viable_hosts.push_back(i);
        //ensure the host can solve tasks
        } else if (host->canSolveTask()) {
          viable_hosts.push_back(i);
        }
      }
    }

    // For each parasite queued for reproduction, create offspring and inject into a random viable host
    emp::Random& rnd = GetRandom();
    std::cout << "parasite reproduction queue: " << parasite_reproduce_queue.size() << std::endl;
    for (emp::WorldPosition location : parasite_reproduce_queue) {

      //check that location has host and parasite
      if (!IsOccupied(location)) continue;
      emp::Ptr<Host> host = pop[location.GetIndex()];
      if (!host->HasParasite()) continue;
      emp::Ptr<Parasite> parasite = host->GetParasite();
      std::cout << "Parasite at index " << location.GetIndex() 
                << " has " << parasite->GetPoints() << " points and is entering reproduction loop." 
                << std::endl;
      //try to reproduce the parasite until it can't anymore
      while (parasite->GetPoints() >= 1 && !viable_hosts.empty() && parasite_multiple_reproduction) {
        std::cout << "Parasite at index " << location.GetIndex() 
                  << " has " << parasite->GetPoints() << " points and will try to reproduce."
                  << std::endl;

        std::optional<std::unique_ptr<Organism>> offspring =
            parasite->CheckReproduction(parasite_mutation_rate);
        if (offspring.has_value() && !viable_hosts.empty()) {
          size_t idx = rnd.GetUInt(viable_hosts.size());
          size_t host_pos = viable_hosts[idx];
          viable_hosts.erase(viable_hosts.begin() + idx); // Don't reuse

          // Move the unique_ptr into emp::Ptr<Parasite>
          Parasite* parasite_offspring = dynamic_cast<Parasite*>(offspring.value().release());
          pop[host_pos]->SetParasite(emp::Ptr<Parasite>(parasite_offspring));
          // std::cout << "Parasite at index " << location.GetIndex() 
          //           << " reproduced and infected host at index " << host_pos << std::endl;
        } else if (offspring.has_value()) {
          std::cout << "Parasite at index " << location.GetIndex() 
                    << " attempted to reproduce but no viable hosts available." << std::endl;
          // If no viable hosts, offspring is lost
          // Properly delete the unique_ptr
          offspring.value().reset();
        }
        parasite->AddPoints(-1); // Deduct points for reproduction
      } if (viable_hosts.empty()) {
        std::cout << "No viable hosts left for parasite reproduction at index " 
                  << location.GetIndex() << std::endl;
      }
    } // End of parasite reproduction loop

    /* //loop over parasites and print their genome
    for (size_t i = 0; i < pop.size(); ++i) {
      if (IsOccupied(i) && pop[i]->HasParasite()) {
        emp::Ptr<Parasite> parasite = pop[i]->GetParasite();
        std::cout << "Parasite at index " << i << " genome: " << parasite->PrintGenome() << std::endl;
      }
    } */

    parasite_reproduce_queue.clear();


  }

/*   bool IsDeadOrg(emp::WorldPosition location) {
    return pop[location.GetIndex()]->IsDead(config->LIFE_SPAN());
  } */

  double GetVirulence() const { return config->VIRULENCE(); }


  /**
   * Input: float output, OrgState &state
   * Output: none
   * Purpose: Check if an organism's output solves any task and assign points
   */
  void CheckOutput(float output, OrgState &state) {
    for (Task *task : tasks) {
      bool success = task->CheckOutput(output, state.last_inputs);
      if (!success) continue;
      double newPoints = config->REWARD();

      if (!state.isParasite) {
        // Host: add points directly
        state.points += newPoints;
      } else {
        std::cout << "[PARASITE] at " << state.current_location.GetIndex()
                  << " Solved task: " << task->name() << std::endl;
        // Track all solved tasks this update
        state.last_solved_tasks.push_back(task->name());
        state.updates_since_task = 0;
        // Give bonus points and queue for reproduction if within bonus window
        if (this->update_num - INJECT_PARASITES_AT < BONUS_UPDATE_LIMIT*2 && parasite_task_bonus) {
          state.points += newPoints*2;
          std::cout << "gave parasite bonus task points and added it to reproduction queue" << std::endl;
          parasite_reproduce_queue.push_back(state.current_location);
        }
      }
      SetTaskVars(task, state);
    }
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

  //unsure if this introduces new bugs, cause DoBirth does a lot of stuff
  emp::WorldPosition customDoBirth(emp::Ptr<Host> &offspring, size_t parent_pos) {
    // Create a new organism at the specified location
    // Find random slot in the population
    size_t rand_idx = GetRandom().GetUInt(GetSize());
    emp::WorldPosition pos(rand_idx);
    //check if the organism at pos is solving tasks
    if (IsOccupied(pos) && pop[pos.GetIndex()]->canSolveTask()) {
      //try one more time - will somewhat lower the probability of overwriting a task solver
      rand_idx = GetRandom().GetUInt(GetSize());
      pos = emp::WorldPosition(rand_idx);
    }
    if (pos.IsValid()) AddOrgAt(offspring, pos, parent_pos);
    else {
      offspring.Delete();
      offspring = nullptr;
    }

    return pos;
  }

  /**
   * Input: emp::WorldPosition location
   * Output: none
   * Purpose: Queue the organism for reproduction at the end of the update cycle
   */
  void ReproduceOrg(emp::WorldPosition location) {
    reproduce_queue.push_back(location);
  }

  void ReproduceParasite(emp::WorldPosition location) {
    parasite_reproduce_queue.push_back(location);
  }
};

#endif