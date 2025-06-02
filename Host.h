#ifndef HOST_H
#define HOST_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"
#include "Org.h"
#include "Parasite.h"

class Host : public Organism {
    private:
    Parasite* parasite = nullptr;  // Parasite that interacts with the host

    public:
    Host(emp::Ptr<OrgWorld> world, double points = 0.0)
        : Organism(world, points) { };

    void SetParasite(Parasite *p) {
        parasite = p;  // Set the parasite for this host
    }

    Parasite GetParasite() const {
        return parasite;  // Get the parasite associated with this host
    }

    bool HasParasite() const {
        return parasite != nullptr;  // Check if the host has a parasite
    }

    void RemoveParasite() {
        parasite = nullptr;  // Remove the parasite from this host
    }

};
#endif