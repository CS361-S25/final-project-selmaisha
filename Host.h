#ifndef HOST_H
#define HOST_H

#include "CPU.h"
#include "OrgState.h"
#include "emp/Evolve/World_structure.hpp"
#include "Org.h"
#include "Parasite.h"


/**
 * Host
 * Represents a host organism that can carry a single parasite.
 */
class Host : public Organism {
    private:
    emp::Ptr<Parasite> parasite = nullptr;  // Parasite that interacts with the host

    public:
    /**
     * input:  world, double points
     * output: none
     * purpose: Construct a Host organism with an optional starting point value.
     */
    Host(emp::Ptr<OrgWorld> world, double points = 0.0)
        : Organism(world, points) { };

    ~Host() {
        delete parasite;
        parasite = nullptr;
    }

    /**
    * input: parasite 
    * output: none
    * purpose: Attach a parasite to this host.
    */
    void SetParasite(emp::Ptr<Parasite> p) {
        parasite = p;  // Set the parasite for this host
    }

   /**
    * input: none
    * output: emp::Ptr<Parasite>
    * purpose: Retrieve the parasite currently associated with this host.
    */
    emp::Ptr<Parasite> GetParasite() const {
        return parasite;  
    }

    /**
   * input: none
   * output: bool
   * purpose: Check if the host currently has a parasite attached.
   */
  bool HasParasite() const {
    return parasite != nullptr;
  }

  /**
   * input: none
   * output: none
   * purpose: Detach and delete the parasite from the host.
   */
  void RemoveParasite() {
    parasite.Delete();
    parasite = nullptr;
  }

  /**
   * input: none
   * output: std::unique_ptr<Organism>
   * purpose: Clone the host organism without cloning its parasite.
   */
  std::unique_ptr<Organism> Clone() const override {
    auto new_host = std::make_unique<Host>(*this);
    new_host->parasite = nullptr;
    return new_host;
  }
};

#endif