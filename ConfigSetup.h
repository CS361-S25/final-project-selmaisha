#ifndef CONFIGSETUP_H
#define CONFIGSETUP_H

#include "emp/config/ArgManager.hpp"

/**
 * WorldConfig
 * input: none
 * output: Named configuration values
 * purpose: Define all tunable parameters for the simulation, such as population size,
 *          mutation rates, task rewards, parasite behavior, and runtime settings.
 */
EMP_BUILD_CONFIG(WorldConfig,
    VALUE(SEED, int, 3, "What value should the random seed be?"), 
    VALUE(NUM_START, double, 10, "How many starting organisms?"),
    VALUE(MUTATION_RATE, double, 0.02, "What is the mutation rate?"),
    VALUE(PARASITE_MUTATION_RATE, double, 0.04, "What is the mutation rate for parasites?"),
    VALUE(NUM_UPDATES, int, 4000, "How many updates to run?"),
    VALUE(REWARD, double, 20.0, "What is the reward for completing a task?"),
    VALUE(NUM_BOXES, int, 20, "How many boxes is on one side the world?"),
    VALUE(LIFE_SPAN, int, 30, "How many updates without reproduction before an organism dies?"),
    VALUE(VIRULENCE, double, 0.8, "Fraction of host reward stolen by matching parasites"),
    VALUE(NUM_PARASITES, int, 30, "How many parasites to inject into the world?"),
    VALUE(INJECT_PARASITES_AT, size_t, 1500, "At which update should parasites be injected?"),
    VALUE(BONUS_UPDATE_LIMIT, int, 2000, "After how many updates should the host be punished for parasite reproduction?"),
  )

#endif