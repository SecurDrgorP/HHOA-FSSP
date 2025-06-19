# Horse Herd Optimization Algorithm for Flow Shop Scheduling Problem (HHOA-FSSP)

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com)
[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

## Overview
This project implements the **Horse Herd Optimization Algorithm (HHOA)** to solve the **Flow Shop Scheduling Problem (FSSP)**. HHOA is a nature-inspired metaheuristic algorithm that mimics the social behavior of horses in a herd, providing an effective approach for combinatorial optimization problems.

The **Flow Shop Scheduling Problem** is a classical NP-hard scheduling optimization problem where n jobs must be processed on m machines in the same order, with the objective of minimizing the total completion time (makespan).

## 🏗️ Project Structure
```
HHOA-FSSP/
├── src/
│   ├── core/                     # Core problem representation
│   │   ├── ProblemInstance.h/.cpp # FSSP instance management
│   │   └── Solution.h/.cpp        # Solution representation & evaluation
│   ├── algorithm/                # HHOA implementation
│   │   ├── HHOA.h/.cpp           # Main optimization algorithm
│   │   ├── Horse.h/.cpp          # Individual horse with behaviors
│   │   └── HorseHerd.h/.cpp      # Population management
│   ├── utils/                    # Utility classes
│   │   ├── Random.h/.cpp         # Thread-safe random generation
│   │   ├── Timer.h/.cpp          # Performance measurement
│   │   └── Logger.h/.cpp         # Multi-level logging system
│   └── main.cpp                  # Command-line interface
├── data/
│   ├── instances/                # Test problem instances
│   │   ├── test_6x5.txt         # Small test case (6 jobs, 5 machines)
│   │   ├── test_10x10.txt       # Medium test case (10 jobs, 10 machines)
│   │   └── test_20x5.txt        # Large test case (20 jobs, 5 machines)
│   └── results/                 # Algorithm execution logs
├── tests/                       # Unit testing framework
├── docs/                        # Comprehensive documentation
├── build/                       # Compiled binaries
├── CMakeLists.txt              # Cross-platform build system
├── benchmark.sh                # Performance benchmarking script
└── PROJECT_SUMMARY.md          # Detailed project completion report
```

## 🐎 Algorithm Description
The Horse Herd Optimization Algorithm (HHOA) is a nature-inspired metaheuristic algorithm that mimics the social behavior of horses in a herd. The algorithm consists of six key behavioral phases:

### Core Behaviors
- **🌱 Grazing behavior**: Local search around current positions using 2-opt and insertion operations
- **🚶 Roaming behavior**: Exploration of new solution areas to maintain diversity
- **👑 Following behavior**: Non-leader horses learn from the best horse (leader)
- **💕 Mating behavior**: Crossover operations to combine good solutions
- **🎲 Mutation**: Random changes to prevent premature convergence
- **⏰ Aging**: Population renewal mechanism to maintain dynamics

### Key Features
- **Early termination** when no improvement is found for 100 iterations
- **Mixed initialization** with 80% random and 20% greedy solutions
- **Adaptive parameters** that adjust based on search progress
- **Elite preservation** to maintain best solutions found

## 📋 FSSP Problem
The Flow Shop Scheduling Problem involves:
- **n jobs** to be processed sequentially
- **m machines** in a fixed processing order (Machine 1 → 2 → ... → m)
- Each job has **specific processing times** on each machine
- **Objective**: Minimize makespan (total completion time)

## 🛠️ Build Instructions
### Prerequisites
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.12 or higher
- Make or Ninja build system

### Compilation
```bash
# Clone and navigate to project directory
cd HHOA-FSSP

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Alternative: Build with specific number of cores
make -j$(nproc)
```

### Verification
```bash
# Run unit tests
./bin/hhoa_fssp_tests

# Test basic functionality
./bin/hhoa_fssp --iterations 100
```

## 🚀 Usage
### Basic Execution
```bash
# Run with default parameters (30 horses, 1000 iterations)
./bin/hhoa_fssp

# Specify custom number of iterations
./bin/hhoa_fssp --iterations 500

# Adjust population size
./bin/hhoa_fssp --population 50

# Load custom problem instance
./bin/hhoa_fssp --file ../data/instances/test_20x5.txt

# Combine multiple options
./bin/hhoa_fssp --iterations 2000 --population 40
```

### Command Line Options
| Option | Description | Default |
|--------|-------------|---------|
| `--iterations N` | Maximum number of iterations | 1000 |
| `--population N` | Population size (number of horses) | 30 |
| `--file PATH` | Load problem instance from file | Built-in 10x10 |
| `--help` | Show usage information | - |

### Running Tests
```bash
# Execute unit tests
./bin/hhoa_fssp_tests

# Run benchmark script
../benchmark.sh
```

## 📊 Performance Results
The algorithm demonstrates excellent optimization performance:

### Test Instance (10×10 Jobs×Machines)
- **Initial makespan**: ~1266-1300 (varies with random initialization)
- **Final makespan**: ~1214-1230 (typical improvement of 3-7%)
- **Execution time**: ~200ms for complete optimization
- **Convergence**: Usually within 100-200 iterations

### Sample Output
```
=========================================
  Horse Herd Optimization Algorithm
  for Flow Shop Scheduling Problem
=========================================

Problem Instance: TestInstance_10x10
Jobs: 10, Machines: 10

Starting HHOA optimization...
Population: 30, Iterations: 1000

=== OPTIMIZATION RESULTS ===
Best Makespan: 1214
Execution Time: 192.986 ms
Iterations: 103
Total Improvements: 4

Best Solution:
Job Sequence: J5 -> J8 -> J7 -> J1 -> J9 -> J10 -> J3 -> J4 -> J2 -> J6
Makespan: 1214
```

## 📈 Algorithm Parameters
The following parameters control the algorithm behavior:

### Default Configuration
- **Population Size**: 30 horses (24 random + 6 greedy initialization)
- **Grazing Intensity**: 0.8 (high local search intensity)
- **Roaming Rate**: 0.3 (30% of horses roam per iteration)
- **Exploration Rate**: 0.5 (moderate exploration)
- **Following Rate**: 0.7 (high learning from leader)
- **Crossover Rate**: 0.8 (high reproduction rate)
- **Mutation Rate**: 0.2 (moderate mutation)
- **Early Termination**: 100 iterations without improvement

### Parameter Tuning Guidelines
For **small problems** (< 10 jobs):
- Reduce population to 15-20
- Increase mutation rate to 0.3
- Use fewer iterations (100-300)

For **large problems** (> 20 jobs):
- Increase population to 50-100
- Reduce mutation rate to 0.1
- Use more iterations (1000-5000)

## 🧪 Testing Framework
The project includes comprehensive testing:

```bash
# Core functionality tests
./bin/hhoa_fssp_tests

# Performance benchmarking
../benchmark.sh

# Manual testing with different instances
./bin/hhoa_fssp --file ../data/instances/test_6x5.txt
./bin/hhoa_fssp --file ../data/instances/test_20x5.txt
```

## 📚 Documentation
- **Algorithm Description**: [docs/algorithm_description.md](docs/algorithm_description.md)
- **Project Summary**: [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)
- **Code Documentation**: Inline comments and Doxygen-style headers

## 🔧 Extending the Project
The modular design allows for easy extensions:

1. **New Problem Types**: Implement additional scheduling variants
2. **Algorithm Variants**: Modify behavioral parameters or add new behaviors
3. **Performance Optimizations**: Add parallel processing or GPU acceleration
4. **Visualization**: Create graphical representations of solutions
5. **Machine Learning**: Integrate adaptive parameter tuning

## 🤝 Contributing
This project demonstrates:
- Modern C++ programming practices (C++17)
- Professional software engineering principles
- Metaheuristic algorithm implementation
- Performance optimization techniques
- Comprehensive testing and documentation

## 📄 License
This project is available under the MIT License.

## 👨‍💻 Authors
- **Developer**: Implementation of HHOA-FSSP
- **Date**: June 2025
- **Course**: Metaheuristic Algorithms

---

*This implementation serves as both a practical optimization tool and an educational resource for understanding nature-inspired algorithms and their application to combinatorial optimization problems.*
