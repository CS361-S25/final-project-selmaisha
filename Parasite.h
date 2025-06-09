#ifndef PARASITE_H
#define PARASITE_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"
#include "Org.h"


/**
 * Parasite
 * Represents a parasite organism with additional virulence behavior and task targeting.
 * Inherits from Organism and sets default points to -1.0 to indicate inactive/dead status.
 */
class Parasite : public Organism {
    private:
    double virulence = 0.8;  // Default virulence value, can be adjusted
    //make constructor that is the same as Organism but with a different name and initialize points to -1.0
    std::string solved_task = "EQU"; // Default to solving EQU

public:
   /**
    * input: world, double points
    * output: none
    * purpose: Construct a parasite with default or given point value (defaults to -1.0).
    */
    Parasite(emp::Ptr<OrgWorld> world, double points = -1.0)
        : Organism(world, points, true) { 
        }

    virtual ~Parasite() = default;

   /**
    * input: none
    * output: bool
    * purpose: Returns true if the parasite is considered dead (-1 points).
    */
    bool IsDead() {
        return GetPoints() < -1.0;  
    }


    /**
    * input: none
    * output: double
    * purpose: Get the current virulence value of the parasite.
    */
    double getVirulence() {
        return virulence;  
    }

    /**
    * input:  new_virulence
    * output: none
    * purpose: Set the parasite's virulence to a new value.
    */
    void setVirulence(double new_virulence) {
        virulence = new_virulence;  // Set a new virulence value
    }


    /**
    * input: none
    * output: string
    * purpose: Get the name of the task this parasite is currently attempting to solve.
    */
    std::string GetSolvedTask() const { return solved_task; }

    /**
    * input:  task_name
    * output: none
    * purpose: Set the target task that the parasite is attempting to solve.
    */
    void SetSolvedTask(const std::string& task_name) {
        solved_task = task_name;
    }

    /**
    * input: none
    * output: clone of parasite
    * purpose: Create a copy of this parasite.
    */
    std::unique_ptr<Organism> Clone() const override {
        return std::make_unique<Parasite>(*this);
    }

    /**
    * input:  world, size_t 
    * output: none
    * purpose: Process one update step for the parasite using the base class logic.
    */
    void Process(OrgWorld& world, size_t id) override {
        Organism::Process(world, id);
    }


};


#endif