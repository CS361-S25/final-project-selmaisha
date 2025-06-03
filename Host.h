#ifndef HOST_H
#define HOST_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"
#include "Org.h"
#include "Parasite.h"

class Host : public Organism {
    private:
    emp::Ptr<Parasite> parasite = nullptr;  // Parasite that interacts with the host

    public:
    Host(emp::Ptr<OrgWorld> world, double points = 0.0)
        : Organism(world, points) { };

    ~Host() {
        delete parasite;
        parasite = nullptr;
    }

    void SetParasite(emp::Ptr<Parasite> p) {
        parasite = p;  // Set the parasite for this host
    }

    emp::Ptr<Parasite> GetParasite() const {
        return parasite;  // Get the parasite associated with this host
    }

    bool HasParasite() const {
        return parasite != nullptr;  // Check if the host has a parasite
    }

    void RemoveParasite() {
        parasite.Delete();
        parasite = nullptr;  // Remove the parasite from this host
    }

    std::unique_ptr<Organism> Clone() const override {
        auto new_host = std::make_unique<Host>(*this);
        new_host->parasite = nullptr;  // Ensure clone starts with no parasite
        return new_host;
    }

};
#endif