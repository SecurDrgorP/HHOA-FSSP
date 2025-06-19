#include "HorseHerd.h"
#include "../utils/Random.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <iomanip>
#include <sstream>

HorseHerd::HorseHerd(std::shared_ptr<ProblemInstance> instance, int herd_size)
    : instance_(instance), leader_(instance), herd_size_(herd_size), diversity_(0.0), generation_(0) {
    if (herd_size <= 0) {
        throw std::invalid_argument("Herd size must be positive");
    }
    horses_.reserve(herd_size);
}

const Horse& HorseHerd::getBestHorse() const {
    if (horses_.empty()) {
        throw std::runtime_error("Herd is empty");
    }
    
    auto best_it = std::max_element(horses_.begin(), horses_.end(),
        [](const Horse& a, const Horse& b) {
            return a.getBestFitness() < b.getBestFitness();
        });
    
    return *best_it;
}

void HorseHerd::initialize(double random_ratio) {
    if (random_ratio < 0.0 || random_ratio > 1.0) {
        throw std::invalid_argument("Random ratio must be between 0.0 and 1.0");
    }
    
    horses_.clear();
    Random& rng = Random::getInstance();
    
    int num_random = static_cast<int>(herd_size_ * random_ratio);
    int num_greedy = herd_size_ - num_random;
    
    LOG_INFO("Initializing herd with " + std::to_string(num_random) + 
             " random and " + std::to_string(num_greedy) + " greedy horses");
    
    // Create random horses
    for (int i = 0; i < num_random; ++i) {
        Horse horse(instance_);
        horse.initializeRandom();
        horses_.push_back(horse);
    }
    
    // Create greedy horses with some variation
    for (int i = 0; i < num_greedy; ++i) {
        Horse horse(instance_);
        horse.initializeGreedy();
        
        // Add some variation to greedy solutions
        if (i > 0) {
            horse.mutate(0.1 * i);  // Increasing mutation rate
        }
        
        horses_.push_back(horse);
    }
    
    // Update leader
    updateLeader();
    calculateDiversity();
    
    LOG_INFO("Herd initialized. Best makespan: " + std::to_string(getBestHorse().getBestMakespan()));
}

bool HorseHerd::updateLeader() {
    if (horses_.empty()) {
        return false;
    }
    
    const Horse& best_horse = getBestHorse();
    bool leader_changed = false;
    
    if (best_horse.getBestFitness() > leader_.getBestFitness()) {
        leader_ = best_horse;
        leader_.setLeader(true);
        leader_changed = true;
        
        LOG_DEBUG("New leader found with makespan: " + std::to_string(leader_.getBestMakespan()));
    }
    
    // Update leader status for all horses
    for (auto& horse : horses_) {
        horse.setLeader(false);
    }
    
    // Find and mark the current leader in the herd
    for (auto& horse : horses_) {
        if (horse.getBestMakespan() == leader_.getBestMakespan()) {
            horse.setLeader(true);
            break;
        }
    }
    
    return leader_changed;
}

double HorseHerd::calculateDiversity() {
    if (horses_.size() < 2) {
        diversity_ = 0.0;
        return diversity_;
    }
    
    double total_distance = 0.0;
    int comparisons = 0;
    
    for (size_t i = 0; i < horses_.size(); ++i) {
        for (size_t j = i + 1; j < horses_.size(); ++j) {
            total_distance += calculateDistance(horses_[i].getSolution(), horses_[j].getSolution());
            comparisons++;
        }
    }
    
    diversity_ = comparisons > 0 ? total_distance / comparisons : 0.0;
    return diversity_;
}

int HorseHerd::performGrazing(double intensity) {
    int improved_count = 0;
    
    for (auto& horse : horses_) {
        if (horse.graze(intensity)) {
            improved_count++;
        }
    }
    
    if (improved_count > 0) {
        updateLeader();
        LOG_DEBUG("Grazing improved " + std::to_string(improved_count) + " horses");
    }
    
    return improved_count;
}

int HorseHerd::performRoaming(double roaming_rate, double exploration_rate) {
    Random& rng = Random::getInstance();
    int roamed_count = 0;
    
    for (auto& horse : horses_) {
        if (rng.randDouble() < roaming_rate) {
            Solution new_solution = horse.roam(exploration_rate);
            
            if (new_solution.getMakespan() < horse.getSolution().getMakespan()) {
                horse.setSolution(new_solution);
                roamed_count++;
            }
        }
    }
    
    if (roamed_count > 0) {
        updateLeader();
        LOG_DEBUG("Roaming improved " + std::to_string(roamed_count) + " horses");
    }
    
    return roamed_count;
}

int HorseHerd::performFollowing(double following_rate) {
    int followed_count = 0;
    
    for (auto& horse : horses_) {
        if (!horse.isLeader()) {
            Solution new_solution = horse.followLeader(leader_, following_rate);
            
            if (new_solution.getMakespan() < horse.getSolution().getMakespan()) {
                horse.setSolution(new_solution);
                followed_count++;
            }
        }
    }
    
    if (followed_count > 0) {
        updateLeader();
        LOG_DEBUG("Following improved " + std::to_string(followed_count) + " horses");
    }
    
    return followed_count;
}

int HorseHerd::performMating(double mating_rate, double crossover_rate) {
    Random& rng = Random::getInstance();
    int offspring_count = 0;
    
    int num_matings = static_cast<int>(horses_.size() * mating_rate / 2);
    
    for (int i = 0; i < num_matings; ++i) {
        // Select two parents using tournament selection
        int parent1_idx = tournamentSelection();
        int parent2_idx = tournamentSelection();
        
        // Ensure different parents
        while (parent2_idx == parent1_idx && horses_.size() > 1) {
            parent2_idx = tournamentSelection();
        }
        
        // Create offspring
        Solution offspring = horses_[parent1_idx].mateWith(horses_[parent2_idx], crossover_rate);
        
        // Replace a weak horse with offspring if offspring is better
        auto weak_indices = selectForReplacement(1);
        if (!weak_indices.empty()) {
            int weak_idx = weak_indices[0];
            if (offspring.getMakespan() < horses_[weak_idx].getSolution().getMakespan()) {
                horses_[weak_idx].setSolution(offspring);
                offspring_count++;
            }
        }
    }
    
    if (offspring_count > 0) {
        updateLeader();
        LOG_DEBUG("Mating created " + std::to_string(offspring_count) + " improved offspring");
    }
    
    return offspring_count;
}

int HorseHerd::performMutation(double mutation_rate) {
    int mutated_count = 0;
    
    for (auto& horse : horses_) {
        int old_makespan = horse.getSolution().getMakespan();
        horse.mutate(mutation_rate);
        
        if (horse.getSolution().getMakespan() < old_makespan) {
            mutated_count++;
        }
    }
    
    if (mutated_count > 0) {
        updateLeader();
        LOG_DEBUG("Mutation improved " + std::to_string(mutated_count) + " horses");
    }
    
    return mutated_count;
}

void HorseHerd::ageHorses() {
    for (auto& horse : horses_) {
        horse.increaseAge();
    }
}

int HorseHerd::replaceWeakHorses(double replacement_rate) {
    int num_replacements = static_cast<int>(horses_.size() * replacement_rate);
    if (num_replacements == 0) return 0;
    
    auto weak_indices = selectForReplacement(num_replacements);
    
    for (int idx : weak_indices) {
        horses_[idx] = createRandomHorse();
    }
    
    updateLeader();
    LOG_DEBUG("Replaced " + std::to_string(num_replacements) + " weak horses");
    
    return num_replacements;
}

int HorseHerd::rejuvenateStagnantHorses(int max_stagnation) {
    int rejuvenated_count = 0;
    
    for (auto& horse : horses_) {
        if (horse.isStagnant(max_stagnation)) {
            horse.rejuvenate();
            horse.initializeRandom();
            rejuvenated_count++;
        }
    }
    
    if (rejuvenated_count > 0) {
        updateLeader();
        LOG_DEBUG("Rejuvenated " + std::to_string(rejuvenated_count) + " stagnant horses");
    }
    
    return rejuvenated_count;
}

int HorseHerd::improveElite(int num_horses) {
    sortByFitness();
    int improved_count = 0;
    
    int elite_count = std::min(num_horses, static_cast<int>(horses_.size()));
    
    for (int i = 0; i < elite_count; ++i) {
        int old_makespan = horses_[i].getBestMakespan();
        
        // Apply intensive local search
        horses_[i].graze(0.9);  // High intensity grazing
        
        if (horses_[i].getBestMakespan() < old_makespan) {
            improved_count++;
        }
    }
    
    if (improved_count > 0) {
        updateLeader();
        LOG_DEBUG("Elite improvement enhanced " + std::to_string(improved_count) + " horses");
    }
    
    return improved_count;
}

std::string HorseHerd::getStatistics() const {
    std::ostringstream oss;
    
    oss << "Generation: " << generation_ << "\n";
    oss << "Herd Size: " << horses_.size() << "\n";
    oss << "Best Makespan: " << getBestHorse().getBestMakespan() << "\n";
    oss << "Average Fitness: " << std::fixed << std::setprecision(2) << getAverageFitness() << "\n";
    oss << "Worst Fitness: " << std::fixed << std::setprecision(2) << getWorstFitness() << "\n";
    oss << "Diversity: " << std::fixed << std::setprecision(4) << diversity_ << "\n";
    oss << "Leader Makespan: " << leader_.getBestMakespan();
    
    return oss.str();
}

void HorseHerd::print() const {
    std::cout << getStatistics() << std::endl;
}

void HorseHerd::printDetailed() const {
    print();
    
    std::cout << "\nHorse Details:" << std::endl;
    std::cout << std::setw(5) << "ID" << std::setw(12) << "Makespan" 
              << std::setw(15) << "Best Makespan" << std::setw(8) << "Age"
              << std::setw(10) << "Leader" << std::setw(12) << "Stagnation" << std::endl;
    
    for (size_t i = 0; i < horses_.size(); ++i) {
        const Horse& horse = horses_[i];
        std::cout << std::setw(5) << i
                  << std::setw(12) << horse.getMakespan()
                  << std::setw(15) << horse.getBestMakespan()
                  << std::setw(8) << std::fixed << std::setprecision(1) << horse.getAge()
                  << std::setw(10) << (horse.isLeader() ? "Yes" : "No")
                  << std::setw(12) << horse.getStagnationCounter() << std::endl;
    }
}

void HorseHerd::sortByFitness() {
    std::sort(horses_.begin(), horses_.end(), std::greater<Horse>());
}

Solution HorseHerd::getBestSolution() const {
    return getBestHorse().getBestSolution();
}

double HorseHerd::getAverageFitness() const {
    if (horses_.empty()) return 0.0;
    
    double sum = std::accumulate(horses_.begin(), horses_.end(), 0.0,
        [](double acc, const Horse& horse) {
            return acc + horse.getBestFitness();
        });
    
    return sum / horses_.size();
}

double HorseHerd::getWorstFitness() const {
    if (horses_.empty()) return 0.0;
    
    auto worst_it = std::min_element(horses_.begin(), horses_.end(),
        [](const Horse& a, const Horse& b) {
            return a.getBestFitness() < b.getBestFitness();
        });
    
    return worst_it->getBestFitness();
}

int HorseHerd::tournamentSelection(int tournament_size) {
    Random& rng = Random::getInstance();
    tournament_size = std::min(tournament_size, static_cast<int>(horses_.size()));
    
    std::vector<int> candidates = rng.sample(horses_.size(), tournament_size);
    
    int best_idx = candidates[0];
    double best_fitness = horses_[best_idx].getBestFitness();
    
    for (int idx : candidates) {
        if (horses_[idx].getBestFitness() > best_fitness) {
            best_fitness = horses_[idx].getBestFitness();
            best_idx = idx;
        }
    }
    
    return best_idx;
}

std::vector<int> HorseHerd::selectForReplacement(int num_horses) {
    std::vector<std::pair<double, int>> fitness_indices;
    
    for (size_t i = 0; i < horses_.size(); ++i) {
        fitness_indices.emplace_back(horses_[i].getBestFitness(), i);
    }
    
    // Sort by fitness (worst first)
    std::sort(fitness_indices.begin(), fitness_indices.end());
    
    std::vector<int> indices;
    for (int i = 0; i < std::min(num_horses, static_cast<int>(fitness_indices.size())); ++i) {
        indices.push_back(fitness_indices[i].second);
    }
    
    return indices;
}

Horse HorseHerd::createRandomHorse() {
    Horse horse(instance_);
    horse.initializeRandom();
    return horse;
}

double HorseHerd::calculateDistance(const Solution& sol1, const Solution& sol2) const {
    return static_cast<double>(sol1.distanceTo(sol2)) / sol1.getNumJobs();
}
