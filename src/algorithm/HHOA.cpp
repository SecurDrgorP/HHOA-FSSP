#include "HHOA.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>

void HHOAParameters::print() const {
    std::cout << "HHOA Parameters:" << std::endl;
    std::cout << "  Population Size: " << population_size << std::endl;
    std::cout << "  Max Iterations: " << max_iterations << std::endl;
    std::cout << "  Grazing Intensity: " << grazing_intensity << std::endl;
    std::cout << "  Roaming Rate: " << roaming_rate << std::endl;
    std::cout << "  Exploration Rate: " << exploration_rate << std::endl;
    std::cout << "  Following Rate: " << following_rate << std::endl;
    std::cout << "  Mating Rate: " << mating_rate << std::endl;
    std::cout << "  Crossover Rate: " << crossover_rate << std::endl;
    std::cout << "  Mutation Rate: " << mutation_rate << std::endl;
    std::cout << "  Replacement Rate: " << replacement_rate << std::endl;
    std::cout << "  Max Stagnation: " << max_stagnation << std::endl;
    std::cout << "  Elite Count: " << elite_count << std::endl;
    std::cout << "  Adaptive Parameters: " << (adaptive_parameters ? "Yes" : "No") << std::endl;
    std::cout << "  Termination Patience: " << termination_patience << std::endl;
}

bool HHOAParameters::isValid() const {
    return population_size > 0 && max_iterations > 0 &&
           grazing_intensity >= 0.0 && grazing_intensity <= 1.0 &&
           roaming_rate >= 0.0 && roaming_rate <= 1.0 &&
           exploration_rate >= 0.0 && exploration_rate <= 1.0 &&
           following_rate >= 0.0 && following_rate <= 1.0 &&
           mating_rate >= 0.0 && mating_rate <= 1.0 &&
           crossover_rate >= 0.0 && crossover_rate <= 1.0 &&
           mutation_rate >= 0.0 && mutation_rate <= 1.0 &&
           replacement_rate >= 0.0 && replacement_rate <= 1.0 &&
           max_stagnation > 0 && elite_count >= 0 &&
           termination_patience > 0;
}

void HHOAStatistics::print() const {
    std::cout << "HHOA Statistics:" << std::endl;
    std::cout << "  Iterations Executed: " << iterations_executed << std::endl;
    std::cout << "  Total Improvements: " << total_improvements << std::endl;
    std::cout << "  Leader Changes: " << leader_changes << std::endl;
    std::cout << "  Rejuvenations: " << rejuvenations << std::endl;
    std::cout << "  Replacements: " << replacements << std::endl;
    std::cout << "  Execution Time: " << std::fixed << std::setprecision(2) 
              << execution_time_ms << " ms" << std::endl;
    
    if (!best_makespan_history.empty()) {
        std::cout << "  Best Makespan: " << *std::min_element(best_makespan_history.begin(), 
                                                             best_makespan_history.end()) << std::endl;
        std::cout << "  Final Makespan: " << best_makespan_history.back() << std::endl;
    }
}

bool HHOAStatistics::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create statistics file " << filename << std::endl;
        return false;
    }

    file << "Iteration,BestMakespan,Diversity,AverageFitness" << std::endl;
    
    for (size_t i = 0; i < best_makespan_history.size(); ++i) {
        file << i << "," << best_makespan_history[i];
        
        if (i < diversity_history.size()) {
            file << "," << diversity_history[i];
        } else {
            file << ",0";
        }
        
        if (i < average_fitness_history.size()) {
            file << "," << average_fitness_history[i];
        } else {
            file << ",0";
        }
        
        file << std::endl;
    }

    file.close();
    return true;
}

HHOA::HHOA(std::shared_ptr<ProblemInstance> instance, const HHOAParameters& parameters)
    : instance_(instance), parameters_(parameters), timer_("HHOA") {
    
    if (!instance || !instance->isValid()) {
        throw std::invalid_argument("Invalid problem instance");
    }
    
    if (!parameters_.isValid()) {
        throw std::invalid_argument("Invalid HHOA parameters");
    }
    
    herd_ = std::make_unique<HorseHerd>(instance_, parameters_.population_size);
}

void HHOA::setParameters(const HHOAParameters& parameters) {
    if (!parameters.isValid()) {
        throw std::invalid_argument("Invalid HHOA parameters");
    }
    parameters_ = parameters;
    
    // Recreate herd if population size changed
    if (!herd_ || herd_->getHerdSize() != parameters_.population_size) {
        herd_ = std::make_unique<HorseHerd>(instance_, parameters_.population_size);
    }
}

void HHOA::setIterationCallback(std::function<void(int, const Solution&, const HHOAStatistics&)> callback) {
    iteration_callback_ = callback;
}

void HHOA::setTerminationCallback(std::function<bool(int, const Solution&)> callback) {
    termination_callback_ = callback;
}

Solution HHOA::optimize() {
    return optimize(parameters_.max_iterations);
}

Solution HHOA::optimize(int iterations) {
    LOG_INFO("Starting HHOA optimization for " + std::to_string(iterations) + " iterations");
    
    initialize();
    
    int stagnation_count = 0;
    int best_makespan = herd_->getBestSolution().getMakespan();
    
    for (int iteration = 0; iteration < iterations; ++iteration) {
        // Print progress every 10 iterations
        if (iteration % 10 == 0) {
            std::cout << "Iteration " << iteration << "/" << iterations << 
                         " - Best makespan: " << best_makespan << std::endl;
        }
        
        bool improved = executeIteration(iteration);
        recordStatistics(iteration, improved);
        
        // Check for improvement
        int current_makespan = herd_->getBestSolution().getMakespan();
        if (current_makespan < best_makespan) {
            best_makespan = current_makespan;
            stagnation_count = 0;
            statistics_.total_improvements++;
            std::cout << "Improvement found at iteration " << iteration << 
                         ": " << current_makespan << std::endl;
        } else {
            stagnation_count++;
        }
        
        // Update adaptive parameters
        if (parameters_.adaptive_parameters) {
            updateAdaptiveParameters(iteration, herd_->getDiversity(), stagnation_count);
        }
        
        // Call iteration callback
        if (iteration_callback_) {
            iteration_callback_(iteration, herd_->getBestSolution(), statistics_);
        }
        
        // Check termination conditions
        if (shouldTerminate(iteration, stagnation_count)) {
            LOG_INFO("Early termination at iteration " + std::to_string(iteration));
            break;
        }
        
        herd_->nextGeneration();
        statistics_.iterations_executed = iteration + 1;
    }
    
    finalize();
    
    LOG_INFO("HHOA optimization completed. Best makespan: " + 
             std::to_string(getBestMakespan()));
    
    return getBestSolution();
}

Solution HHOA::optimizeToTarget(int target_makespan, int max_iterations) {
    if (max_iterations <= 0) {
        max_iterations = parameters_.max_iterations;
    }
    
    LOG_INFO("Starting HHOA optimization to target makespan: " + std::to_string(target_makespan));
    
    initialize();
    
    for (int iteration = 0; iteration < max_iterations; ++iteration) {
        bool improved = executeIteration(iteration);
        recordStatistics(iteration, improved);
        
        // Check if target is reached
        if (herd_->getBestSolution().getMakespan() <= target_makespan) {
            LOG_INFO("Target makespan reached at iteration " + std::to_string(iteration));
            break;
        }
        
        // Call iteration callback
        if (iteration_callback_) {
            iteration_callback_(iteration, herd_->getBestSolution(), statistics_);
        }
        
        herd_->nextGeneration();
        statistics_.iterations_executed = iteration + 1;
    }
    
    finalize();
    
    return getBestSolution();
}

void HHOA::reset() {
    statistics_ = HHOAStatistics{};
    if (herd_) {
        herd_ = std::make_unique<HorseHerd>(instance_, parameters_.population_size);
    }
}

Solution HHOA::getBestSolution() const {
    if (!herd_) {
        throw std::runtime_error("Algorithm not initialized");
    }
    return herd_->getBestSolution();
}

int HHOA::getBestMakespan() const {
    if (!herd_) {
        throw std::runtime_error("Algorithm not initialized");
    }
    return herd_->getBestSolution().getMakespan();
}

void HHOA::print() const {
    std::cout << "=== HHOA Algorithm ===" << std::endl;
    parameters_.print();
    std::cout << std::endl;
    
    if (herd_) {
        herd_->print();
        std::cout << std::endl;
    }
    
    statistics_.print();
}

void HHOA::printDetailed() const {
    print();
    
    if (herd_) {
        std::cout << "\n=== Detailed Herd Information ===" << std::endl;
        herd_->printDetailed();
    }
}

bool HHOA::saveResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create results file " << filename << std::endl;
        return false;
    }

    file << "HHOA Results for " << instance_->getInstanceName() << std::endl;
    file << "Problem Size: " << instance_->getNumJobs() << " jobs, " 
         << instance_->getNumMachines() << " machines" << std::endl;
    file << "Best Makespan: " << getBestMakespan() << std::endl;
    file << "Iterations: " << statistics_.iterations_executed << std::endl;
    file << "Execution Time: " << statistics_.execution_time_ms << " ms" << std::endl;
    file << std::endl;

    file << "Best Solution Sequence:" << std::endl;
    const auto& sequence = getBestSolution().getJobSequence();
    for (size_t i = 0; i < sequence.size(); ++i) {
        file << "J" << (sequence[i] + 1);
        if (i < sequence.size() - 1) file << " -> ";
    }
    file << std::endl;

    file.close();
    return true;
}

bool HHOA::executeIteration(int iteration) {
    bool improved = false;
    
    // Debug output for first few iterations
    if (iteration < 5) {
        std::cout << "Starting iteration " << iteration << std::endl;
    }
    
    // Phase 1: Grazing (local search)
    if (iteration < 5) std::cout << "  Phase 1: Grazing..." << std::endl;
    int grazing_improvements = herd_->performGrazing(parameters_.grazing_intensity);
    if (grazing_improvements > 0) improved = true;
    
    // Phase 2: Roaming (exploration)
    if (iteration < 5) std::cout << "  Phase 2: Roaming..." << std::endl;
    int roaming_improvements = herd_->performRoaming(parameters_.roaming_rate, 
                                                    parameters_.exploration_rate);
    if (roaming_improvements > 0) improved = true;
    
    // Phase 3: Following the leader
    if (iteration < 5) std::cout << "  Phase 3: Following..." << std::endl;
    int following_improvements = herd_->performFollowing(parameters_.following_rate);
    if (following_improvements > 0) improved = true;
    
    // Phase 4: Mating
    if (iteration < 5) std::cout << "  Phase 4: Mating..." << std::endl;
    int mating_improvements = herd_->performMating(parameters_.mating_rate, 
                                                  parameters_.crossover_rate);
    if (mating_improvements > 0) improved = true;
    
    // Phase 5: Mutation
    if (iteration < 5) std::cout << "  Phase 5: Mutation..." << std::endl;
    int mutation_improvements = herd_->performMutation(parameters_.mutation_rate);
    if (mutation_improvements > 0) improved = true;
    
    // Phase 6: Age horses
    if (iteration < 5) std::cout << "  Phase 6: Aging..." << std::endl;
    herd_->ageHorses();
    
    // Phase 7: Replace weak horses
    if (iteration % 10 == 0) {
        int replacements = herd_->replaceWeakHorses(parameters_.replacement_rate);
        statistics_.replacements += replacements;
    }
    
    // Phase 8: Rejuvenate stagnant horses
    if (iteration % parameters_.max_stagnation == 0) {
        int rejuvenations = herd_->rejuvenateStagnantHorses(parameters_.max_stagnation);
        statistics_.rejuvenations += rejuvenations;
    }
    
    // Phase 9: Elite improvement
    if (iteration % parameters_.elite_improvement_freq == 0) {
        int elite_improvements = herd_->improveElite(parameters_.elite_count);
        if (elite_improvements > 0) improved = true;
    }
    
    // Phase 10: Update leader and diversity
    if (herd_->updateLeader()) {
        statistics_.leader_changes++;
    }
    
    herd_->calculateDiversity();
    
    // Apply diversity preservation if needed
    if (herd_->getDiversity() < parameters_.diversity_threshold) {
        applyDiversityPreservation(herd_->getDiversity());
    }
    
    return improved;
}

bool HHOA::shouldTerminate(int iteration, int stagnation_count) const {
    // Check custom termination callback
    if (termination_callback_) {
        return termination_callback_(iteration, herd_->getBestSolution());
    }
    
    // Check maximum iterations
    if (iteration >= parameters_.max_iterations - 1) {
        return true;
    }
    
    // Check stagnation-based early termination
    if (stagnation_count >= parameters_.termination_patience) {
        return true;
    }
    
    return false;
}

void HHOA::updateAdaptiveParameters(int iteration, double diversity, int stagnation_count) {
    double progress = static_cast<double>(iteration) / parameters_.max_iterations;
    
    // Adapt exploration vs exploitation based on progress
    if (progress < 0.3) {
        // Early stage: more exploration
        parameters_.roaming_rate = std::min(0.5, parameters_.roaming_rate * 1.1);
        parameters_.exploration_rate = std::min(0.5, parameters_.exploration_rate * 1.1);
    } else if (progress > 0.7) {
        // Late stage: more exploitation
        parameters_.grazing_intensity = std::min(0.9, parameters_.grazing_intensity * 1.05);
        parameters_.following_rate = std::min(0.9, parameters_.following_rate * 1.05);
    }
    
    // Adapt based on diversity
    if (diversity < parameters_.diversity_threshold) {
        // Low diversity: increase exploration
        parameters_.mutation_rate = std::min(0.3, parameters_.mutation_rate * 1.2);
        parameters_.replacement_rate = std::min(0.2, parameters_.replacement_rate * 1.1);
    } else if (diversity > 0.1) {
        // High diversity: increase exploitation
        parameters_.grazing_intensity = std::min(0.9, parameters_.grazing_intensity * 1.1);
    }
    
    // Adapt based on stagnation
    if (stagnation_count > parameters_.max_stagnation / 2) {
        parameters_.mutation_rate = std::min(0.3, parameters_.mutation_rate * 1.15);
    }
}

bool HHOA::applyDiversityPreservation(double diversity) {
    if (diversity >= parameters_.diversity_threshold) {
        return false;
    }
    
    // Force some diversity by replacing worst horses with random ones
    int num_replacements = std::max(1, static_cast<int>(parameters_.population_size * 0.2));
    herd_->replaceWeakHorses(static_cast<double>(num_replacements) / parameters_.population_size);
    
    // Increase mutation rate temporarily
    parameters_.mutation_rate = std::min(0.4, parameters_.mutation_rate * 1.5);
    
    LOG_DEBUG("Applied diversity preservation mechanisms");
    return true;
}

void HHOA::recordStatistics(int iteration, bool improved) {
    statistics_.best_makespan_history.push_back(herd_->getBestSolution().getMakespan());
    statistics_.diversity_history.push_back(herd_->getDiversity());
    statistics_.average_fitness_history.push_back(herd_->getAverageFitness());
}

void HHOA::initialize() {
    LOG_INFO("Initializing HHOA algorithm");
    
    timer_.start();
    statistics_ = HHOAStatistics{};
    
    // Initialize the herd
    herd_->initialize();
    
    LOG_INFO("Initial best makespan: " + std::to_string(herd_->getBestSolution().getMakespan()));
}

void HHOA::finalize() {
    timer_.stop();
    statistics_.execution_time_ms = timer_.getElapsedMs();
    
    LOG_INFO("HHOA algorithm completed in " + timer_.getFormattedTime());
}
