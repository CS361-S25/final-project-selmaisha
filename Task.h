#ifndef TASK_H
#define TASK_H

#include <cmath>
#include <string>
#include <iostream>

/**
 * The interface for a task that organisms can complete.
 */
class Task {
public:
  double reward = 5.0;

  // Constructor that sets the reward for completing the task
  Task(double _reward) : reward(_reward) {}

  void SetReward(double _reward) {
    reward = _reward;
  }

  // Pure virtual method that checks if the task is completed correctly
  virtual double CheckOutput(uint32_t output, uint32_t inputs[4]) = 0;

  // Returns the name of the task
  virtual std::string name() const = 0;
};

class NOTTask : public Task {
    public:
    NOTTask(double _reward) : Task(_reward) {}
  
    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; i++) {
        if (~inputs[i] == output) {
            return reward;
        }
        }
        return 0.0;
    }

    std::string name() const override {
        return "NOT";
    }
};

class NANDTask : public Task {
public:
    NANDTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && ~(inputs[i] & inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "NAND"; }
};

class ANDTask : public Task {
public:
    ANDTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && (inputs[i] & inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "AND"; }
};

class ORNTask : public Task {
public:
    ORNTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && (inputs[i] | ~inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "ORN"; }
};

class ORTask : public Task {
public:
    ORTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && (inputs[i] | inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "OR"; }
};

class ANDNTask : public Task {
public:
    ANDNTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && (inputs[i] & ~inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "ANDN"; }
};

class NORTask : public Task {
public:
    NORTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && ~(inputs[i] | inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "NOR"; }
};

class XORTask : public Task {
public:
    XORTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && (inputs[i] ^ inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "XOR"; }
};

class EQUTask : public Task {
public:
    EQUTask(double _reward) : Task(_reward) {}

    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (i != j && ~(inputs[i] ^ inputs[j]) == output) {
                    return reward;
                }
            }
        }
        return 0.0;
    }

    std::string name() const override { return "EQU"; }
};


#endif
