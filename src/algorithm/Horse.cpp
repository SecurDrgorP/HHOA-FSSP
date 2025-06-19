#include "Horse.h"
#include "../utils/Random.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <iostream>
#include <climits>

Horse::Horse(std::shared_ptr<ProblemInstance> instance)
    : solution_(instance), best_solution_(instance), fitness_(0.0), best_fitness_(0.0),
      age_(0.0), grazing_ability_(0.8), stamina_(1.0), is_leader_(false), stagnation_counter_(0) {
    initializeRandom();
}

Horse::Horse(const Solution& solution)
    : solution_(solution), best_solution_(solution), age_(0.0), grazing_ability_(0.8),
      stamina_(1.0), is_leader_(false), stagnation_counter_(0) {
    updateFitness();
    best_fitness_ = fitness_;
}

Horse::Horse(const Horse& other)
    : solution_(other.solution_), best_solution_(other.best_solution_),
      fitness_(other.fitness_), best_fitness_(other.best_fitness_),
      age_(other.age_), grazing_ability_(other.grazing_ability_),
      stamina_(other.stamina_), is_leader_(other.is_leader_),
      stagnation_counter_(other.stagnation_counter_) {}

Horse& Horse::operator=(const Horse& other) {
    if (this != &other) {
        solution_ = other.solution_;
        best_solution_ = other.best_solution_;
        fitness_ = other.fitness_;
        best_fitness_ = other.best_fitness_;
        age_ = other.age_;
        grazing_ability_ = other.grazing_ability_;
        stamina_ = other.stamina_;
        is_leader_ = other.is_leader_;
        stagnation_counter_ = other.stagnation_counter_;
    }
    return *this;
}

void Horse::setSolution(const Solution& solution) {
    solution_ = solution;
    updateFitness();
    if (updateBest()) {
        resetStagnation();
    } else {
        stagnation_counter_++;
    }
}

void Horse::initializeRandom() {
    solution_.initializeRandom();
    updateFitness();
    best_solution_ = solution_;
    best_fitness_ = fitness_;
}

void Horse::initializeGreedy() {
    solution_.initializeGreedy();
    updateFitness();
    best_solution_ = solution_;
    best_fitness_ = fitness_;
}

void Horse::updateFitness() {
    fitness_ = calculateFitness(solution_.getMakespan());
}

bool Horse::updateBest() {
    if (fitness_ > best_fitness_) {
        best_solution_ = solution_;
        best_fitness_ = fitness_;
        return true;
    }
    return false;
}

bool Horse::graze(double intensity) {
    if (intensity <= 0.0 || intensity > 1.0) {
        throw std::invalid_argument("Intensity must be between 0.0 and 1.0");
    }
    
    bool improved = false;
    Random& rng = Random::getInstance();
    
    // Apply local search based on grazing ability and intensity
    double effective_intensity = intensity * grazing_ability_ * stamina_;
    
    if (rng.randDouble() < effective_intensity) {
        // Try 2-opt local search
        if (apply2OptSearch()) {
            improved = true;
            LOG_DEBUG("Horse improved through 2-opt grazing");
        }
    }
    
    if (rng.randDouble() < effective_intensity * 0.7) {
        // Try insertion-based local search
        if (applyInsertionSearch()) {
            improved = true;
            LOG_DEBUG("Horse improved through insertion grazing");
        }
    }
    
    if (improved) {
        updateFitness();
        updateBest();
    }
    
    return improved;
}

Solution Horse::roam(double exploration_rate) {
    if (exploration_rate < 0.0 || exploration_rate > 1.0) {
        throw std::invalid_argument("Exploration rate must be between 0.0 and 1.0");
    }
    
    Random& rng = Random::getInstance();
    Solution new_solution = solution_;
    
    // Number of moves based on exploration rate
    int num_moves = static_cast<int>(exploration_rate * solution_.getNumJobs() * 0.5);
    num_moves = std::max(1, num_moves);
    
    for (int i = 0; i < num_moves; ++i) {
        if (rng.randBool(0.5)) {
            // Swap move
            new_solution = new_solution.createSwapNeighbor();
        } else {
            // Insert move
            new_solution = new_solution.createInsertNeighbor();
        }
    }
    
    return new_solution;
}

Solution Horse::followLeader(const Horse& leader, double following_rate) {
    if (following_rate < 0.0 || following_rate > 1.0) {
        throw std::invalid_argument("Following rate must be between 0.0 and 1.0");
    }
    
    Random& rng = Random::getInstance();
    
    // Blend current solution with leader's solution
    if (rng.randDouble() < following_rate) {
        return orderCrossover(solution_, leader.getBestSolution());
    } else {
        return partiallyMappedCrossover(solution_, leader.getBestSolution());
    }
}

Solution Horse::mateWith(const Horse& mate, double crossover_rate) {
    if (crossover_rate < 0.0 || crossover_rate > 1.0) {
        throw std::invalid_argument("Crossover rate must be between 0.0 and 1.0");
    }
    
    Random& rng = Random::getInstance();
    
    if (rng.randDouble() < crossover_rate) {
        if (rng.randBool()) {
            return orderCrossover(best_solution_, mate.getBestSolution());
        } else {
            return partiallyMappedCrossover(best_solution_, mate.getBestSolution());
        }
    } else {
        // Return one of the parents
        return rng.randBool() ? best_solution_ : mate.getBestSolution();
    }
}

void Horse::mutate(double mutation_rate) {
    if (mutation_rate < 0.0 || mutation_rate > 1.0) {
        throw std::invalid_argument("Mutation rate must be between 0.0 and 1.0");
    }
    
    Random& rng = Random::getInstance();
    
    if (rng.randDouble() < mutation_rate) {
        if (rng.randBool()) {
            // Swap mutation
            solution_ = solution_.createSwapNeighbor();
        } else {
            // Insert mutation
            solution_ = solution_.createInsertNeighbor();
        }
        
        updateFitness();
        updateBest();
    }
}

void Horse::increaseAge() {
    age_ += 1.0;
    
    // Decrease abilities with age
    grazing_ability_ *= 0.995;  // Slight decrease
    stamina_ *= 0.998;          // Slight decrease
    
    // Ensure abilities don't go below minimum thresholds
    grazing_ability_ = std::max(0.1, grazing_ability_);
    stamina_ = std::max(0.1, stamina_);
}

void Horse::rejuvenate() {
    age_ = 0.0;
    grazing_ability_ = 0.8 + Random::getInstance().randDouble() * 0.2;  // 0.8 to 1.0
    stamina_ = 0.8 + Random::getInstance().randDouble() * 0.2;          // 0.8 to 1.0
    stagnation_counter_ = 0;
}

bool Horse::isStagnant(int max_stagnation) const {
    return stagnation_counter_ >= max_stagnation;
}

bool Horse::operator<(const Horse& other) const {
    return fitness_ < other.fitness_;
}

bool Horse::operator>(const Horse& other) const {
    return fitness_ > other.fitness_;
}

bool Horse::operator==(const Horse& other) const {
    return solution_ == other.solution_;
}

void Horse::print() const {
    std::cout << "Horse - Fitness: " << fitness_ << ", Best Fitness: " << best_fitness_
              << ", Makespan: " << getMakespan() << ", Best Makespan: " << getBestMakespan()
              << ", Age: " << age_ << ", Leader: " << (is_leader_ ? "Yes" : "No") << std::endl;
}

double Horse::calculateFitness(int makespan) const {
    // Use negative makespan as fitness (higher fitness = better solution)
    return makespan > 0 ? -static_cast<double>(makespan) : -1000000.0;
}

Solution Horse::orderCrossover(const Solution& parent1, const Solution& parent2) const {
    Random& rng = Random::getInstance();
    int size = parent1.getNumJobs();
    
    // Select two random crossover points
    int point1 = rng.randInt(0, size - 1);
    int point2 = rng.randInt(0, size - 1);
    if (point1 > point2) std::swap(point1, point2);
    
    std::vector<int> offspring(size, -1);
    std::vector<bool> used(size, false);
    
    // Copy segment from parent1
    for (int i = point1; i <= point2; ++i) {
        int job = parent1.getJobAt(i);
        if (job >= 0 && job < size) {  // Validate job index
            offspring[i] = job;
            used[job] = true;
        }
    }
    
    // Fill remaining positions with jobs from parent2 in order
    int current_pos = 0;
    // Find first empty position
    while (current_pos < size && offspring[current_pos] != -1) {
        current_pos++;
    }
    
    for (int i = 0; i < size && current_pos < size; ++i) {
        int job = parent2.getJobAt(i);
        
        if (job >= 0 && job < size && !used[job]) {
            offspring[current_pos] = job;
            used[job] = true;
            
            // Find next empty position
            current_pos++;
            while (current_pos < size && offspring[current_pos] != -1) {
                current_pos++;
            }
        }
    }
    
    // Fill any remaining gaps with unused jobs
    for (int i = 0; i < size; ++i) {
        if (!used[i]) {
            // Find first empty position
            for (int j = 0; j < size; ++j) {
                if (offspring[j] == -1) {
                    offspring[j] = i;
                    used[i] = true;
                    break;
                }
            }
        }
    }
    
    return Solution(offspring, parent1.getInstance());
}

Solution Horse::partiallyMappedCrossover(const Solution& parent1, const Solution& parent2) const {
    Random& rng = Random::getInstance();
    int size = parent1.getNumJobs();
    
    // Simply return a copy of parent1 for now to avoid complexity
    // This is a safer approach until we can implement PMX properly
    std::vector<int> offspring = parent1.getJobSequence();
    
    // Apply a simple modification: swap a few random positions with parent2
    int num_swaps = rng.randInt(1, std::min(3, size/2));
    for (int i = 0; i < num_swaps; ++i) {
        int pos1 = rng.randInt(0, size - 1);
        int pos2 = rng.randInt(0, size - 1);
        
        // Find the job from parent2 at pos1 in offspring and swap with pos2
        int job_from_parent2 = parent2.getJobAt(pos1);
        
        // Find where this job is in offspring
        for (int j = 0; j < size; ++j) {
            if (offspring[j] == job_from_parent2) {
                std::swap(offspring[j], offspring[pos2]);
                break;
            }
        }
    }
    
    return Solution(offspring, parent1.getInstance());
}

bool Horse::apply2OptSearch() {
    bool improved = false;
    int current_makespan = solution_.getMakespan();
    
    for (int i = 0; i < solution_.getNumJobs() - 1; ++i) {
        for (int j = i + 1; j < solution_.getNumJobs(); ++j) {
            solution_.swapJobs(i, j);
            
            int new_makespan = solution_.getMakespan();
            if (new_makespan < current_makespan) {
                current_makespan = new_makespan;
                improved = true;
                break;  // Take first improvement
            } else {
                solution_.swapJobs(i, j);  // Revert
            }
        }
        if (improved) break;
    }
    
    return improved;
}

bool Horse::applyInsertionSearch() {
    bool improved = false;
    int current_makespan = solution_.getMakespan();
    
    auto job_sequence = solution_.getJobSequence();
    
    for (int i = 0; i < static_cast<int>(job_sequence.size()); ++i) {
        for (int j = 0; j < static_cast<int>(job_sequence.size()); ++j) {
            if (i == j) continue;
            
            // Remove job from position i and insert at position j
            int job = job_sequence[i];
            auto temp_sequence = job_sequence;
            temp_sequence.erase(temp_sequence.begin() + i);
            temp_sequence.insert(temp_sequence.begin() + (j > i ? j - 1 : j), job);
            
            Solution temp_solution(temp_sequence, solution_.getInstance());
            int new_makespan = temp_solution.getMakespan();
            
            if (new_makespan < current_makespan) {
                solution_ = temp_solution;
                current_makespan = new_makespan;
                improved = true;
                return true;  // Take first improvement
            }
        }
    }
    
    return improved;
}
