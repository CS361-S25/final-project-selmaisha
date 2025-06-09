Selma and Isha

We look at parasite stuff

# Host-Parasite Coevolution Simulation

This project simulates digital evolution using hosts and parasites that solve logic tasks to survive and reproduce. Inspired by Zaman et al. (2011), the model explores how coevolution drives phenotypic diversity and innovation in a grid-based digital environment.

This project implements a digital evolution simulation using SignalGP-Lite and the Empirical framework. It explores how host-parasite coevolution impacts phenotypic diversity, based on the experimental setup from Zaman et al. (2011). The system simulates digital organisms (hosts and parasites) evolving in a grid-based world by solving logic tasks to survive and reproduce.

## 💡 Overview

- Hosts and parasites execute logic-based programs using SignalGP-Lite.
- Organisms earn points by solving Boolean logic tasks (e.g., NOT, NAND, AND...).
- Parasites live inside hosts and reproduce only when solving the **same task** as their host.
- The simulation tracks ecological and evolutionary dynamics including task diversity, population sizes, and parasite virulence.

## 🧬 Key Features

- **Task-Based Color Visualization**: Organisms are color-coded by the hardest task they solve.
- **Web + Native Modes**: Run in browser via WebAssembly or as a native C++ simulation.
- **Interactive Config Panel**: Adjust parameters like mutation rate, parasite virulence, injection timing, etc.
- **Bonus Period**: Parasites receive initial bonuses and temporarily don’t harm their host.
- **Diversity Tracking**: Optional data collection for Shannon diversity or task frequency over time.

## 🚀 Getting Started

### Web Version
To compile and run in a browser:
```bash
./compile-run-web.sh


Reference

This simulation is based on:

Zaman, L., et al. (2011). Rapid host-parasite coevolution drives the production and maintenance of diversity in digital organisms. Nature, 459(7245), 274–277.

 Authors

Isha Patel

Selma Vangstein

Developed for CS361: Artificial Life and Digital Evolution, Spring 2025 — Carleton College.