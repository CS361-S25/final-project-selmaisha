# Host–Parasite Coevolution Simulation

This project is a digital evolution simulation inspired by *Zaman et al. (2014)*, modeling the ecological and evolutionary dynamics between **hosts** and **parasites** in a structured grid world. Both organisms are capable of solving Boolean logic tasks using virtual CPUs, and parasites evolve to exploit hosts by mimicking their task solutions.

The simulation is web-based and interactive, allowing users to visualize coevolution in real-time and adjust parameters through a dynamic config panel.

---

##  Simulation Overview

- **Hosts** live in grid cells, reproduce asexually, and solve logic tasks (e.g., `NOT`, `AND`, `EQU`) to earn points and survive.
- **Parasites** infect hosts, gain points by solving the same task as their host, and reduce the host's points based on a configurable virulence value.
- **Bonus period**: After injection, parasites are temporarily boosted to help them establish a population.
- Organisms age, die, and mutate based on configurable lifespan and mutation rates.

---

##  Web Interface Features

- **Config Panel**: Adjust parameters such as mutation rate, parasite virulence, lifespan, number of parasites, and more.
- **Live Grid Visualization**: Hosts are shown as colored squares; parasites appear as red-outlined circles.
- **Readout Panel**: Real-time statistics including task solvers, parasite counts, and update step.
- **Explanations and Results**: Includes descriptions of simulation rules and summaries from related academic research.

---

##  Recommended Settings

You can adjust the simulation using the panel on the web interface. For best results and a stable coevolutionary dynamic, we recommend the following values:

| Setting                   | Value   | Explanation                                                                 |
|---------------------------|---------|-----------------------------------------------------------------------------|
| `Num Start`              | `10`    | Initial number of host organisms                                           |
| `Mutation Rate`          | `0.02`  | Mutation rate for host reproduction                                        |
| `Parasite Mutation Rate` | `0.04`  | Mutation rate for parasite reproduction                                    |
| `Num Updates`            | `4000`  | How many updates the simulation will run                                   |
| `Reward`                 | `20`    | Points earned for solving a logic task                                     |
| `Num Boxes`              | `20`    | Grid dimension (20 × 20 world size)                                        |
| `Life Span`              | `30`    | Max age before organism dies                                               |
| `Virulence`              | `0.8`   | How harmful parasites are to hosts when solving the same task              |
| `Num Parasites`          | `30`    | Number of parasites injected into the world                                |
| `Inject Parasites At`    | `1500`  | Update step at which parasites are introduced                              |
| `Bonus Update Limit`     | `2000`  | Duration for which parasite bonus features (e.g., protection) are enabled  |

---

## 📚 Reference

This simulation is based on the methods described in:

> Zaman, Luis, Suhas Devangam, and Charles Ofria. “Rapid Host-Parasite Coevolution Drives the Production and Maintenance of Diversity in Digital Organisms” 176 (July 12, 2011): 219–26. https://doi.org/10.1145/2001576.2001607.

---

