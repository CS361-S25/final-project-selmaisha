#ifndef PARASITE_H
#define PARASITE_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"
#include "Org.h"

class Parasite : public Organism {
    private:
    double virulence = 0.8;  // Default virulence value, can be adjusted
    //make constructor that is the same as Organism but with a different name and initialize points to -1.0
public:
    Parasite(emp::Ptr<OrgWorld> world, double points = -1.0)
        : Organism(world, points, true) { }

    bool IsDead() {
        return GetPoints() < -1.0;  // Parasite is considered dead if points are less than -1
    }

    double getVirulence() {
        return virulence;  // Return the virulence value
    }

    double setVirulence(double new_virulence) {
        virulence = new_virulence;  // Set a new virulence value
    }
};


#endif