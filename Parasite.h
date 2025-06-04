#ifndef PARASITE_H
#define PARASITE_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"
#include "Org.h"
// OrgState state;  // Add this if it’s missing

class Parasite : public Organism {
    private:
    double virulence = 0.8;  // Default virulence value, can be adjusted
    //make constructor that is the same as Organism but with a different name and initialize points to -1.0
    std::string solved_task = "EQU"; // Default to solving EQU

public:
    Parasite(emp::Ptr<OrgWorld> world, double points = -1.0)
        : Organism(world, points, true) { 
            //   GetCPU().state.isParasite = true;
            //   GetCPU().state.completed_EQU = true;
        }

        // OrgState& GetCPU() { return state; }
        // const OrgState& GetCPU() const { return state; }

    virtual ~Parasite() = default;

    bool IsDead() {
        return GetPoints() < -1.0;  // Parasite is considered dead if points are less than -1
    }

    double getVirulence() {
        return virulence;  // Return the virulence value
    }

    
    void setVirulence(double new_virulence) {
        virulence = new_virulence;  // Set a new virulence value
    }
    std::string GetSolvedTask() const { return solved_task; }

    void SetSolvedTask(const std::string& task_name) {
    solved_task = task_name;
    }


    std::unique_ptr<Organism> Clone() const override {
        return std::make_unique<Parasite>(*this);
    }

};


#endif