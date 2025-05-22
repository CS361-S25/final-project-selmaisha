#ifndef CONFIG_SETUP_H
#define CONFIG_SETUP_H

//#include "emp/config/config.hpp" 
#include "emp/config/ArgManager.hpp"


EMP_BUILD_CONFIG(BaselineConfig,
    VALUE(RUN_TIME, int, 1000, "Number of updates to run the simulation"),
    VALUE(POP_SIZE, int, 10, "Number of organisms in the population"),
    VALUE(PROG_LENGTH, size_t, 32, "Length of initial organism programs"),
    VALUE(WORLD_SIZE, int, 15, "Size of the world (number of boxes in each direction)"),
    VALUE(MUT_RATE, double, 0.02, "Mutation rate per instruction"),
    VALUE(INSTRUCTION_CNT, size_t, 256, "Number of possible instructions"),
    VALUE(SEED, int, 3, "Random number seed (-1 = use time-based seed)"),
    VALUE(OUTPUT_DIR, std::string, "data/", "Directory to save output files"),
    VALUE(REPRO_COST, int, 20, "Energy cost to reproduce"),
    VALUE(REPRO_LIFESPAN, int, 30, "Max updates without reproducing before death"),
    VALUE(TASK_REWARD, int, 5, "Energy reward per time step")
  )

#endif // CONFIG_SETUP_H
