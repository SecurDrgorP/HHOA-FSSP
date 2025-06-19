#ifndef HHOA_H
#define HHOA_H

#include "HorseHerd.h"
#include "../utils/Timer.h"
#include "../utils/Logger.h"
#include <string>
#include <vector>
#include <functional>

/**
 * @brief Parameters for the Horse Herd Optimization Algorithm
 */
struct HHOAParameters {
    int population_size = 30;           // Size of the horse herd
    int max_iterations = 1000;          // Maximum number of iterations
    double grazing_intensity = 0.5;     // Intensity of grazing behavior
    double roaming_rate = 0.3;          // Rate of roaming behavior
    double exploration_rate = 0.3;      // Exploration rate for roaming
    double following_rate = 0.7;        // Rate of following the leader
    double mating_rate = 0.4;           // Rate of mating behavior
    double crossover_rate = 0.8;        // Crossover rate for mating
    double mutation_rate = 0.1;         // Mutation rate
    double replacement_rate = 0.1;      // Rate of replacing weak horses
    int max_stagnation = 20;            // Maximum stagnation before rejuvenation
    int elite_improvement_freq = 10;    // Frequency of elite improvement
    int elite_count = 3;                // Number of elite horses to improve
    double diversity_threshold = 0.01;  // Minimum diversity threshold
    bool adaptive_parameters = true;    // Use adaptive parameter control
    int termination_patience = 100;     // Iterations without improvement for early termination
    
    /**
     * @brief Print parameters
     */
    void print() const;
    
    /**
     * @brief Validate parameters
     * @return True if valid, false otherwise
     */
    bool isValid() const;
};

/**
 * @brief Statistics for algorithm execution
 */
struct HHOAStatistics {
    int iterations_executed = 0;
    int total_improvements = 0;
    int leader_changes = 0;
    int rejuvenations = 0;
    int replacements = 0;
    double execution_time_ms = 0.0;
    std::vector<int> best_makespan_history;
    std::vector<double> diversity_history;
    std::vector<double> average_fitness_history;
    
    /**
     * @brief Print statistics
     */
    void print() const;
    
    /**
     * @brief Save statistics to file
     * @param filename File to save statistics
     * @return True if successful
     */
    bool saveToFile(const std::string& filename) const;
};

/**
 * @brief Main Horse Herd Optimization Algorithm class
 */
class HHOA {
private:
    std::shared_ptr<ProblemInstance> instance_;
    HHOAParameters parameters_;
    HHOAStatistics statistics_;
    std::unique_ptr<HorseHerd> herd_;
    Timer timer_;
    
    // Callback functions
    std::function<void(int, const Solution&, const HHOAStatistics&)> iteration_callback_;
    std::function<bool(int, const Solution&)> termination_callback_;

public:
    /**
     * @brief Constructor
     * @param instance Problem instance to solve
     * @param parameters Algorithm parameters
     */
    HHOA(std::shared_ptr<ProblemInstance> instance, const HHOAParameters& parameters = HHOAParameters{});

    /**
     * @brief Destructor
     */
    ~HHOA() = default;

    // Getters
    const HHOAParameters& getParameters() const { return parameters_; }
    const HHOAStatistics& getStatistics() const { return statistics_; }
    const HorseHerd* getHerd() const { return herd_.get(); }

    // Setters
    void setParameters(const HHOAParameters& parameters);
    void setIterationCallback(std::function<void(int, const Solution&, const HHOAStatistics&)> callback);
    void setTerminationCallback(std::function<bool(int, const Solution&)> callback);

    /**
     * @brief Run the optimization algorithm
     * @return Best solution found
     */
    Solution optimize();

    /**
     * @brief Run the algorithm for a specific number of iterations
     * @param iterations Number of iterations to run
     * @return Best solution found
     */
    Solution optimize(int iterations);

    /**
     * @brief Run the algorithm until a target makespan is reached
     * @param target_makespan Target makespan to achieve
     * @param max_iterations Maximum iterations allowed
     * @return Best solution found
     */
    Solution optimizeToTarget(int target_makespan, int max_iterations = -1);

    /**
     * @brief Reset the algorithm state
     */
    void reset();

    /**
     * @brief Get the current best solution
     * @return Current best solution
     */
    Solution getBestSolution() const;

    /**
     * @brief Get the current best makespan
     * @return Current best makespan
     */
    int getBestMakespan() const;

    /**
     * @brief Print algorithm information
     */
    void print() const;

    /**
     * @brief Print detailed algorithm state
     */
    void printDetailed() const;

    /**
     * @brief Save results to file
     * @param filename File to save results
     * @return True if successful
     */
    bool saveResults(const std::string& filename) const;

private:
    /**
     * @brief Execute a single iteration of the algorithm
     * @param iteration Current iteration number
     * @return True if improvement was found
     */
    bool executeIteration(int iteration);

    /**
     * @brief Check termination conditions
     * @param iteration Current iteration
     * @param stagnation_count Current stagnation count
     * @return True if algorithm should terminate
     */
    bool shouldTerminate(int iteration, int stagnation_count) const;

    /**
     * @brief Update adaptive parameters based on current state
     * @param iteration Current iteration
     * @param diversity Current diversity
     * @param stagnation_count Stagnation count
     */
    void updateAdaptiveParameters(int iteration, double diversity, int stagnation_count);

    /**
     * @brief Apply diversity preservation mechanisms
     * @param diversity Current diversity
     * @return True if diversity mechanisms were applied
     */
    bool applyDiversityPreservation(double diversity);

    /**
     * @brief Record iteration statistics
     * @param iteration Current iteration
     * @param improved Whether improvement was found
     */
    void recordStatistics(int iteration, bool improved);

    /**
     * @brief Initialize algorithm state
     */
    void initialize();

    /**
     * @brief Finalize algorithm execution
     */
    void finalize();
};

#endif // HHOA_H
