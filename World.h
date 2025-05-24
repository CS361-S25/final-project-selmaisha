#ifndef WORLD_H
#define WORLD_H

#include "emp/Evolve/World.hpp"
#include "emp/math/Random.hpp"
#include "emp/prefab/ReadoutPanel.hpp"

#include "Task.h"
#include "Org.h"
#include "ConfigSetup.h"
#include <numeric>
#include <fstream>

class OrgWorld : public emp::World<Organism> {
private:
  BaselineConfig *config;
  emp::Random random;
  emp::vector<emp::WorldPosition> reproduce_queue;
  //vector to hold tasks
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
  

public:

  OrgWorld(emp::Random& rand, BaselineConfig* cfg)
  : emp::World<Organism>(), config(cfg), random(rand)
    {

    }

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
  }

  //I previously had this in native and web, but I guess it makes sense to have it in world
  void InitPopulation() {
    for (int i = 0; i < config->POP_SIZE(); i++) {
      Organism* new_org = new Organism(this, 0.0);
      Inject(*new_org);
    }
    Resize(std::sqrt(config->POP_SIZE()), std::sqrt(config->POP_SIZE()));

  }

  
  // Some getters we might not need
  double GetReward() {
    return config->TASK_REWARD();
  }
  double GetMutationRate() {
    return config->MUT_RATE();
  }

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

  //can the tasksolvers and datanode getters be moved to a separate file?
  /**
   *  Input: none
   * Output: TaskSolverCounts struct
   * Purpose: Get the number of each type of task solver in the world
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

    file.AddVar(update, "update", "Update number");
    file.AddTotal(node1, "org_count", "Number of organisms");
    file.AddTotal(node2, "NOT_count", "Number of NOT task solvers");
    file.AddTotal(node3, "NAND_count", "Number of NAND task solvers");
    file.AddTotal(node4, "AND_count", "Number of AND task solvers");
    file.AddTotal(node5, "ORN_count", "Number of ORN task solvers");
    file.AddTotal(node6, "OR_count", "Number of OR task solvers");
    file.AddTotal(node7, "ANDN_count", "Number of ANDN task solvers");
    file.AddTotal(node8, "NOR_count", "Number of NOR task solvers");
    file.AddTotal(node9, "XOR_count", "Number of XOR task solvers");
    file.AddTotal(node10, "EQU_count", "Number of EQU task solvers");
    
    file.PrintHeaderKeys();

    return file;
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

  
  void Update() {
    emp::World<Organism>::Update();
    double mutation_rate = config->MUT_RATE(); //come back and see if we need to do this - track the cfg vars thoroughout

    emp::vector<size_t> update_schedule = emp::GetPermutation(random, GetSize());
    for (size_t idx : update_schedule) {
      if (!IsOccupied(idx)) continue;
      //auto& org = *pop[idx];
      //org.ProcessStep(config->TASK_REWARD()); //I set the reward as a propery of the tasks, not orgs, but maybe orgs actually make more sense
      pop[idx]->Process(idx); // Process the organism

    //   if (pop[idx]->GetPoints() >= config->REPRO_COST() &&
    //     pop[idx]->GetCPU().state.age_since_reproduction >= 5) {
    //   ReproduceOrg(idx);  // queue up for reproduction
    // }
    }

    for (emp::WorldPosition location : reproduce_queue) {
      if (!IsOccupied(location)) {
        return;
      }
      auto org = pop[location.GetIndex()];

      std::optional<Organism> offspring = org->CheckReproduction(
            mutation_rate,
            config->REPRO_COST(),
            config->REPRO_LIFESPAN()
        );

      if (offspring.has_value()) {        
        DoBirth(offspring.value(), location.GetIndex());
        org->GetCPU().state.age_since_reproduction = 0;
      }
      //can replace this with a check for death method in the organism class
      if (org->IsDead(config->REPRO_LIFESPAN())) {
        RemoveOrgAt(location);
      }
      //add check for death here - has org exceeded max lifespan?
    }
    reproduce_queue.clear();
    //we put the reproduction on a different schedule to prevent bias
    //emp::vector<size_t> reproduce_schedule = emp::GetPermutation(random, GetSize());
    /* for (emp::WorldPosition idx : reproduce_queue) {
      if (!IsOccupied(idx)) continue;

      auto& org = *pop[idx];
      auto offspring = org.TryReproduce(
        config->REPRO_COST(), config->REPRO_LIFESPAN(), random,
        config->MUT_RATE(), config->INSTRUCTION_CNT()
      );

      if (offspring.has_value()) {
        // DoBirth(offspring.value(), idx);
        // Inject(offspring.value());
        // this->SetupOrg(idx, offspring.value());
        SetupOrg(idx, offspring.value()); //what is the difference between DoBirth and setuporg?
      } else if (org.IsDead(config->REPRO_LIFESPAN(), config->REPRO_COST())) {
        RemoveOrgAt(idx);
      }
    } */
  }



  //do you know why these are different types? I have this fcn from my previous code
  const pop_t &GetPopulation() { return pop; }

  /**
   * Input: float output, OrgState &state
   * Output: none
   * Purpose: Check if an organism's output solves any task and assign points
   */
  void CheckOutput(float output, OrgState &state) {
    for (Task *task : tasks) {
      bool success = task->CheckOutput(output, state.last_inputs);
      if (!success) continue;
      double newPoints =  config->TASK_REWARD();
      state.points += newPoints;
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

  
}; 

#endif // WORLD_H