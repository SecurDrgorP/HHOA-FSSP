# Horse Herd Optimization Algorithm (HHOA) for Flow Shop Scheduling Problem (FSSP)

## Algorithm Overview

The Horse Herd Optimization Algorithm (HHOA) is a nature-inspired metaheuristic algorithm that simulates the social behavior of horses in a herd. This implementation applies HHOA to solve the Flow Shop Scheduling Problem (FSSP).

## FSSP Problem Description

The Flow Shop Scheduling Problem (FSSP) is a classical scheduling optimization problem where:
- **n** jobs must be processed on **m** machines
- All jobs follow the same processing order through machines (1 → 2 → ... → m)
- Each job has a specific processing time on each machine
- **Objective**: Minimize the makespan (total completion time)

## HHOA Behaviors

### 1. Grazing Behavior (Local Search)
- Horses search for better solutions in their local neighborhood
- Implements 2-opt and insertion-based local search
- Intensity depends on horse's grazing ability and stamina

### 2. Roaming Behavior (Exploration)
- Horses explore new areas of the solution space
- Creates neighbor solutions through swap and insertion moves
- Rate controlled by exploration parameter

### 3. Following Behavior
- Non-leader horses follow the leader (best horse)
- Uses crossover operators (Order Crossover, Partially Mapped Crossover)
- Balances exploitation of good solutions

### 4. Mating Behavior
- Horses combine their genetic material to create offspring
- Tournament selection for parent selection
- Offspring replace weak horses if they are better

### 5. Aging and Rejuvenation
- Horses age over time, decreasing their abilities
- Stagnant horses are rejuvenated with new random solutions
- Maintains population diversity

## Algorithm Flow

```
1. Initialize horse herd with random and greedy solutions
2. For each iteration:
   a. Grazing: Apply local search to improve solutions
   b. Roaming: Explore new solution areas
   c. Following: Non-leaders follow the leader
   d. Mating: Create offspring through crossover
   e. Mutation: Apply random changes
   f. Aging: Decrease horse abilities
   g. Replacement: Replace weak horses
   h. Rejuvenation: Reset stagnant horses
   i. Elite Improvement: Intensive local search on best horses
   j. Update leader and calculate diversity
3. Return best solution found
```

## Key Features

### Adaptive Parameter Control
- Parameters automatically adjust based on:
  - Search progress (exploration vs exploitation)
  - Population diversity
  - Stagnation level

### Diversity Preservation
- Monitors population diversity
- Applies preservation mechanisms when diversity is low
- Prevents premature convergence

### Multi-level Local Search
- 2-opt moves for adjacent job swaps
- Insertion moves for job reordering
- Elite improvement for best solutions

### Solution Representation
- Permutation encoding: sequence of job indices
- Natural representation for FSSP
- Easy crossover and mutation operations

## Implementation Details

### Classes Structure
```
ProblemInstance: FSSP problem data and operations
Solution: Job sequence and makespan calculation
Horse: Individual solution with behavior methods
HorseHerd: Population management and operations
HHOA: Main algorithm controller
```

### Crossover Operators
1. **Order Crossover (OX)**:
   - Preserves relative order from one parent
   - Fills remaining positions from other parent

2. **Partially Mapped Crossover (PMX)**:
   - Creates mapping between parent segments
   - Resolves conflicts outside mapped region

### Local Search Strategies
1. **2-opt**: Swap two jobs and check improvement
2. **Insertion**: Move job to different position
3. **First Improvement**: Accept first better solution found

## Parameters

| Parameter | Default | Description |
|-----------|---------|-------------|
| Population Size | 30 | Number of horses in herd |
| Max Iterations | 1000 | Maximum algorithm iterations |
| Grazing Intensity | 0.5 | Local search intensity |
| Roaming Rate | 0.3 | Exploration frequency |
| Following Rate | 0.7 | Leader following frequency |
| Mating Rate | 0.4 | Reproduction frequency |
| Crossover Rate | 0.8 | Crossover probability |
| Mutation Rate | 0.1 | Mutation probability |
| Replacement Rate | 0.1 | Weak horse replacement rate |

## Usage Examples

### Basic Usage
```cpp
auto instance = ProblemInstance::loadFromFile("instance.txt");
HHOA algorithm(instance);
Solution best = algorithm.optimize();
```

### Custom Parameters
```cpp
HHOAParameters params;
params.population_size = 50;
params.max_iterations = 2000;
params.adaptive_parameters = true;

HHOA algorithm(instance, params);
Solution best = algorithm.optimize();
```

### With Callbacks
```cpp
algorithm.setIterationCallback([](int iter, const Solution& sol, const HHOAStatistics& stats) {
    std::cout << "Iteration " << iter << ": " << sol.getMakespan() << std::endl;
});

Solution best = algorithm.optimize();
```

## Performance Characteristics

### Time Complexity
- Per iteration: O(n² × m × population_size)
- Total: O(iterations × n² × m × population_size)

### Space Complexity
- O(population_size × n + n × m)

### Convergence
- Balances exploration and exploitation
- Adaptive parameters improve convergence
- Diversity preservation prevents local optima

## Advantages

1. **Global Search Capability**: Explores large solution space
2. **Local Search Integration**: Improves solution quality
3. **Adaptive Behavior**: Adjusts to problem characteristics
4. **Diversity Maintenance**: Avoids premature convergence
5. **Flexible Parameters**: Customizable for different problems

## Applications

- Manufacturing scheduling
- Job shop scheduling
- Resource allocation
- Project scheduling
- Supply chain optimization

## Future Improvements

1. **Problem-Specific Operators**: Design FSSP-specific moves
2. **Parallel Implementation**: Distribute horses across processors
3. **Machine Learning Integration**: Learn parameter settings
4. **Multi-Objective Extension**: Handle multiple criteria
