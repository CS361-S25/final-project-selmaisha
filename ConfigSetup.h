#ifndef CONFIGSETUP_H
#define CONFIGSETUP_H

#include "emp/config/ArgManager.hpp"

EMP_BUILD_CONFIG(WorldConfig,
    VALUE(SEED, int, 3, "What value should the random seed be?"), 
    VALUE(NUM_START, double, 10, "How many starting organisms?"),
    VALUE(MUTATION_RATE, double, 0.02, "What is the mutation rate?"),
    VALUE(NUM_UPDATES, int, 4000, "How many updates to run?"),
    VALUE(REWARD, double, 20.0, "What is the reward for completing a task?"),
    VALUE(NUM_BOXES, int, 20, "How many boxes is on one side the world?"),
    VALUE(LIFE_SPAN, int, 30, "How many updates without reproduction before an organism dies?"),
    VALUE(PARASITE_VIRULENCE, double, 0.5, "Fraction of host reward stolen by matching parasites"),

  )

#endif