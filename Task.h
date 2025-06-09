#ifndef TASK_H
#define TASK_H

#include <cmath>
#include <string>
#include <iostream>

/**
 * Task
 * Abstract interface for logic tasks that organisms attempt to solve.
 * Each task must implement a CheckOutput method and a name identifier.
 */
class Task {
public:
  // Pure virtual method that checks if the task is completed correctly
  virtual bool CheckOutput(uint32_t output, uint32_t inputs[4]) = 0;

  // Returns the name of the task
  virtual std::string name() const = 0;
};

/**
 * NOTTask
 * Solves: output = ~input[i]
 */
class NOTTask : public Task {
public:  
    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; i++) {
        if (~inputs[i] == output) {
            return true;
        }
        }
        return false;
    }

    std::string name() const override {
        return "NOT";
    }
};

/**
 * NANDTask
 * Solves: output = ~(input[i] & input[j])
 */
class NANDTask : public Task {
public:

    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if ((i!=j) && ~(inputs[i] & inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "NAND"; }
};

/**
 * ANDTask
 * Solves: output = input[i] & input[j]
 */
class ANDTask : public Task {
public:

    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if ((i!=j) && (inputs[i] & inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "AND"; }
};

/**
 * ORNTask
 * Solves: output = input[i] | ~input[j]
 */
class ORNTask : public Task {
public:

    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if ((i!=j) && (inputs[i] | ~inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "ORN"; }
};

/**
 * ORTask
 * Solves: output = input[i] | input[j]
 */
class ORTask : public Task {
public:

    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if ((i!=j) && (inputs[i] | inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "OR"; }
};

/**
 * ANDNTask
 * Solves: output = input[i] & ~input[j]
 */
class ANDNTask : public Task {
public:

    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if ((i!=j) && (inputs[i] & ~inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "ANDN"; }
};

/**
 * NORTask
 * Solves: output = ~(input[i] | input[j])
 */
class NORTask : public Task {
public:

    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if ((i!=j) && ~(inputs[i] | inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "NOR"; }
};

/**
 * XORTask
 * Solves: output = input[i] ^ input[j]
 */
class XORTask : public Task {
public:

    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if ((i!=j) && (inputs[i] ^ inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "XOR"; }
};

/**
 * EQUTask
 * Solves: output = ~(input[i] ^ input[j]) → detects equality
 */
class EQUTask : public Task {
public:
    // Checks if the output is the negation of the XOR of any two distinct inputs
    bool CheckOutput(uint32_t output, uint32_t inputs[4]) override {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j){
                if ((i!=j) && ~(inputs[i] ^ inputs[j]) == output) {
                    return true;
                }
            }
        }
        return false;
    }

    std::string name() const override { return "EQU"; }
};

#endif
