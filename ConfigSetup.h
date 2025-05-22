#ifndef CONFIG_SETUP_H
#define CONFIG_SETUP_H

#include "emp/config/config.hpp" 
// #include "emp/config/ArgManager.hpp"
// #include "emp/prefab/ConfigPanel.hpp"
// #include "emp/web/UrlParams.hpp"

EMP_BUILD_CONFIG( BaselineConfig,
  // GROUP(BASE, "Base settings for the absolute baseline run"),
  
  VALUE(RUN_TIME, size_t, 1000, "Number of updates to run the simulation"),
  VALUE(POP_SIZE, size_t, 100, "Number of organisms in the population"),
  VALUE(PROG_LENGTH, size_t, 32, "Length of initial organism programs"),
  VALUE(MUT_RATE, double, 0.02, "Mutation rate per instruction"),
  VALUE(INSTRUCTION_CNT, size_t, 256, "Number of possible instructions"),
  VALUE(SEED, int, -1, "Random number seed (-1 = use time-based seed)"),
  VALUE(OUTPUT_DIR, std::string, "data/", "Directory to save output files"),


  VALUE(REPRO_COST, int, 20, "Energy cost to reproduce"),
  VALUE(REPRO_LIFESPAN, int, 30, "Max updates without reproducing before death"),
  VALUE(TASK_REWARD, int, 5, "Energy reward per time step")
)

// EMP_REGISTER_CONFIG(BaselineConfig, "baseline_config.cfg");
#endif // CONFIG_SETUP_H
