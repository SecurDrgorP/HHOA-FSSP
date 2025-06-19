#ifndef HORSE_H
#define HORSE_H

#include "../core/Solution.h"
#include <memory>

/**
 * @brief Represents an individual horse in the Horse Herd Optimization Algorithm
 * 
 * Each horse represents a solution to the FSSP and has associated behavior parameters.
 */
class Horse {
private:
    Solution solution_;              // Current solution (job sequence)
    Solution best_solution_;         // Best solution found by this horse
    double fitness_;                 // Current fitness (negative makespan for maximization)
    double best_fitness_;            // Best fitness achieved
    double age_;                     // Age of the horse (for aging mechanism)
    double grazing_ability_;         // Grazing ability parameter
    double stamina_;                 // Stamina parameter
    bool is_leader_;                 // Whether this horse is the leader
    int stagnation_counter_;         // Counter for stagnation

public:
    /**
     * @brief Constructor
     * @param instance Problem instance
     */
    explicit Horse(std::shared_ptr<ProblemInstance> instance);

    /**
     * @brief Constructor with initial solution
     * @param solution Initial solution
     */
    explicit Horse(const Solution& solution);

    /**
     * @brief Copy constructor
     * @param other Horse to copy
     */
    Horse(const Horse& other);

    /**
     * @brief Assignment operator
     * @param other Horse to assign
     * @return Reference to this horse
     */
    Horse& operator=(const Horse& other);

    // Getters
    const Solution& getSolution() const { return solution_; }
    const Solution& getBestSolution() const { return best_solution_; }
    double getFitness() const { return fitness_; }
    double getBestFitness() const { return best_fitness_; }
    double getAge() const { return age_; }
    double getGrazingAbility() const { return grazing_ability_; }
    double getStamina() const { return stamina_; }
    bool isLeader() const { return is_leader_; }
    int getStagnationCounter() const { return stagnation_counter_; }
    int getMakespan() const { return solution_.getMakespan(); }
    int getBestMakespan() const { return best_solution_.getMakespan(); }

    // Setters
    void setSolution(const Solution& solution);
    void setLeader(bool is_leader) { is_leader_ = is_leader; }
    void setGrazingAbility(double ability) { grazing_ability_ = ability; }
    void setStamina(double stamina) { stamina_ = stamina; }

    /**
     * @brief Initialize horse with random solution
     */
    void initializeRandom();

    /**
     * @brief Initialize horse with greedy solution
     */
    void initializeGreedy();

    /**
     * @brief Update fitness based on current solution
     */
    void updateFitness();

    /**
     * @brief Update best solution if current is better
     * @return True if best solution was updated
     */
    bool updateBest();

    /**
     * @brief Perform grazing behavior (local search)
     * @param intensity Intensity of grazing (0.0 to 1.0)
     * @return True if improvement was found
     */
    bool graze(double intensity = 0.5);

    /**
     * @brief Perform roaming behavior (exploration)
     * @param exploration_rate Rate of exploration (0.0 to 1.0)
     * @return New solution from roaming
     */
    Solution roam(double exploration_rate = 0.3);

    /**
     * @brief Follow the leader horse
     * @param leader Reference to the leader horse
     * @param following_rate Rate of following (0.0 to 1.0)
     * @return New solution following the leader
     */
    Solution followLeader(const Horse& leader, double following_rate = 0.7);

    /**
     * @brief Mate with another horse to produce offspring
     * @param mate Partner horse for mating
     * @param crossover_rate Crossover rate (0.0 to 1.0)
     * @return Offspring solution
     */
    Solution mateWith(const Horse& mate, double crossover_rate = 0.8);

    /**
     * @brief Apply mutation to the current solution
     * @param mutation_rate Mutation rate (0.0 to 1.0)
     */
    void mutate(double mutation_rate = 0.1);

    /**
     * @brief Age the horse (increase age and possibly decrease abilities)
     */
    void increaseAge();

    /**
     * @brief Reset age and abilities (rejuvenation)
     */
    void rejuvenate();

    /**
     * @brief Check if horse is stagnant
     * @param max_stagnation Maximum stagnation iterations
     * @return True if stagnant
     */
    bool isStagnant(int max_stagnation = 10) const;

    /**
     * @brief Reset stagnation counter
     */
    void resetStagnation() { stagnation_counter_ = 0; }

    /**
     * @brief Comparison operators (based on fitness)
     */
    bool operator<(const Horse& other) const;
    bool operator>(const Horse& other) const;
    bool operator==(const Horse& other) const;

    /**
     * @brief Print horse information
     */
    void print() const;

private:
    /**
     * @brief Calculate fitness from makespan
     * @param makespan Makespan value
     * @return Fitness value
     */
    double calculateFitness(int makespan) const;

    /**
     * @brief Order crossover (OX) operator
     * @param parent1 First parent solution
     * @param parent2 Second parent solution
     * @return Offspring solution
     */
    Solution orderCrossover(const Solution& parent1, const Solution& parent2) const;

    /**
     * @brief Partially mapped crossover (PMX) operator
     * @param parent1 First parent solution
     * @param parent2 Second parent solution
     * @return Offspring solution
     */
    Solution partiallyMappedCrossover(const Solution& parent1, const Solution& parent2) const;

    /**
     * @brief Apply 2-opt local search
     * @return True if improvement was found
     */
    bool apply2OptSearch();

    /**
     * @brief Apply insertion-based local search
     * @return True if improvement was found
     */
    bool applyInsertionSearch();
};

#endif // HORSE_H
