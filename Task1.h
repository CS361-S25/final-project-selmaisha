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

/**
 * Task: SquareTask
 * Description: Reward is given if the output is the square of any of the inputs.
 */
class SquareTask : public Task {
public:
  SquareTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      uint32_t square = inputs[i] * inputs[i];
      if ((square - output) == 0) {
        return reward;
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "Square";
  }
};

/**
 * Task: DoubleTask
 * Description: Reward is given if the output is double any of the inputs.
 */
class DoubleTask : public Task {
public:
  DoubleTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      uint32_t doub = inputs[i] * 2;
      if ((doub - output) == 0) {
        return reward;
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "Double";
  }
};

/**
 * Task: IncrementTask
 * Description: Reward is given if the output is one more than any input.
 */
class IncrementTask : public Task {
public:
  IncrementTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      if (output == inputs[i] + 1) {
        return reward;
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "Increment";
  }
};

/**
 * Task: DecrementTask
 * Description: Reward is given if the output is one less than any input.
 */
class DecrementTask : public Task {
public:
  DecrementTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      if (output == inputs[i] - 1) {
        return reward;
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "Decrement";
  }
};

/**
 * Task: AddTask
 * Description: Reward is given if the output is the sum of any two distinct inputs.
 */
class AddTask : public Task {
public:
  AddTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (i != j && inputs[i] + inputs[j] == output) {
          return reward;
        }
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "Add";
  }
};

/**
 * Task: AddThreeTask
 * Description: Reward is given if the output is the sum of any three distinct inputs.
 */
class AddThreeTask : public Task {
public:
  AddThreeTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        for (int k = 0; k < 4; k++) {
          if (i != j && j != k && i != k) {
            uint32_t add = inputs[i] + inputs[j] + inputs[k];
            if ((add - output) == 0) {
              return reward;
            }
          }
        }
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "AddThree";
  }
};

/**
 * Task: SubtractTask
 * Description: Reward is given if the output is the difference between any two inputs (input[i] - input[j]) where i != j.
 */
class SubtractTask : public Task {
public:
  SubtractTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      for (int j = 0; j < 4; j++) {
        if (inputs[i] >= inputs[j] && i != j) {
          uint32_t subtracted = inputs[i] - inputs[j];
          if ((subtracted - output) == 0) {
            return reward;
          }
        }
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "Subtract";
  }
};

/**
 * Task: EchoTask
 * Description: Reward is given if the output is equal to any of the inputs.
 */
class EchoTask : public Task {
public:
  EchoTask(double _reward) : Task(_reward) {}

  double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
    for (int i = 0; i < 4; i++) {
      if (inputs[i] == output) {
        return reward;
      }
    }
    return 0.0;
  }

  std::string name() const override {
    return "Echo";
  }
};

/**
 * Task: BitNot
 * Description: Reward is given if the output is the bitwise NOT of any input.
 */
class BitNotTask : public Task {
  public:
  BitNotTask(double _reward) : Task(_reward) {}
  
    double CheckOutput(uint32_t output, uint32_t inputs[4]) override {
      for (int i = 0; i < 4; i++) {
        if (~inputs[i] == output) {
          return reward;
        }
      }
      return 0.0;
    }
  
    std::string name() const override {
      return "BitNot";
    }
  };

#endif
