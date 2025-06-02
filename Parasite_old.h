

#ifndef PARASITE_H
#define PARASITE_H

#include "OrgState.h"
#include "Org.h"
//#include "World.h"

#include "emp/base/Ptr.hpp"
#include <string>

//class OrgWorld;

/**
 * A standalone class representing a digital parasite.
 * Parasites interact with host organisms by matching tasks and gaining points.
 */
class Parasite {
private:
  OrgState state;                    // Internal state (not currently used)
  double points = -1.0;              
  // std::string last_task;            
  emp::Ptr<OrgWorld> world;         // Pointer to the simulation world

public:
  // Constructor with world pointer and optional starting points
  Parasite(emp::Ptr<OrgWorld> w, double start_points = -1.0)
    : world(w), points(start_points) { }

  // Accessor and mutator methods
  double GetPoints() const { return points; }
  void SetPoints(double value) { points = value; }
  void AddPoints(double delta) { points += delta; }

  bool IsDead() const { return points < -1; }

  void SetLastTask(const std::string & task) { last_task = task; }
  const std::string & GetLastTask() const { return last_task; }

  // Parasite behavior: attempt to match host's task and steal points
  void Process(size_t idx) {
    if (!world->IsOccupied(idx)) return;

    Organism & host = *(world->GetOrgPtr(idx));
    const OrgState & host_state = host.GetCPU().state;
    std::string task_name = host.GetSolvedTask();

    if (world->SolvedSameTask(host_state, task_name)) {
      double reward = world->GetVirulence();
      AddPoints(reward);
      SetLastTask(task_name);
    }
  }
};

#endif
