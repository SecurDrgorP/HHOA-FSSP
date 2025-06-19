#ifndef HORSE_HERD_H
#define HORSE_HERD_H

#include "Horse.h"
#include <vector>
#include <memory>

/**
 * @brief Manages a herd of horses in the Horse Herd Optimization Algorithm
 */
class HorseHerd {
private:
    std::vector<Horse> horses_;                    // Population of horses
    std::shared_ptr<ProblemInstance> instance_;    // Problem instance
    Horse leader_;                                 // Current leader horse
    int herd_size_;                               // Size of the herd
    double diversity_;                            // Current diversity measure
    int generation_;                              // Current generation number

public:
    /**
     * @brief Constructor
     * @param instance Problem instance
     * @param herd_size Size of the herd
     */
    HorseHerd(std::shared_ptr<ProblemInstance> instance, int herd_size);

    /**
     * @brief Destructor
     */
    ~HorseHerd() = default;

    // Getters
    const std::vector<Horse>& getHorses() const { return horses_; }
    const Horse& getLeader() const { return leader_; }
    const Horse& getBestHorse() const;
    int getHerdSize() const { return herd_size_; }
    double getDiversity() const { return diversity_; }
    int getGeneration() const { return generation_; }
    std::shared_ptr<ProblemInstance> getInstance() const { return instance_; }

    /**
     * @brief Initialize the herd
     * @param random_ratio Ratio of horses initialized randomly (vs greedy)
     */
    void initialize(double random_ratio = 0.8);

    /**
     * @brief Update the leader based on fitness
     * @return True if leader changed
     */
    bool updateLeader();

    /**
     * @brief Calculate diversity of the herd
     * @return Diversity measure
     */
    double calculateDiversity();

    /**
     * @brief Perform grazing behavior for all horses
     * @param intensity Grazing intensity
     * @return Number of horses that improved
     */
    int performGrazing(double intensity = 0.5);

    /**
     * @brief Perform roaming behavior for horses
     * @param roaming_rate Percentage of horses to roam
     * @param exploration_rate Exploration intensity
     * @return Number of horses that roamed
     */
    int performRoaming(double roaming_rate = 0.3, double exploration_rate = 0.3);

    /**
     * @brief Perform following behavior
     * @param following_rate Rate of following the leader
     * @return Number of horses that followed
     */
    int performFollowing(double following_rate = 0.7);

    /**
     * @brief Perform mating behavior
     * @param mating_rate Percentage of horses to participate in mating
     * @param crossover_rate Crossover rate for mating
     * @return Number of offspring created
     */
    int performMating(double mating_rate = 0.4, double crossover_rate = 0.8);

    /**
     * @brief Apply mutation to horses
     * @param mutation_rate Mutation rate
     * @return Number of horses mutated
     */
    int performMutation(double mutation_rate = 0.1);

    /**
     * @brief Age all horses and apply aging effects
     */
    void ageHorses();

    /**
     * @brief Replace weak horses with new ones
     * @param replacement_rate Percentage of horses to replace
     * @return Number of horses replaced
     */
    int replaceWeakHorses(double replacement_rate = 0.1);

    /**
     * @brief Rejuvenate stagnant horses
     * @param max_stagnation Maximum stagnation iterations
     * @return Number of horses rejuvenated
     */
    int rejuvenateStagnantHorses(int max_stagnation = 20);

    /**
     * @brief Apply local search to best horses
     * @param num_horses Number of best horses to improve
     * @return Number of horses improved
     */
    int improveElite(int num_horses = 3);

    /**
     * @brief Get statistics of the herd
     * @return Statistics string
     */
    std::string getStatistics() const;

    /**
     * @brief Print herd information
     */
    void print() const;

    /**
     * @brief Print detailed herd information
     */
    void printDetailed() const;

    /**
     * @brief Sort horses by fitness (best first)
     */
    void sortByFitness();

    /**
     * @brief Get the best solution found so far
     * @return Best solution
     */
    Solution getBestSolution() const;

    /**
     * @brief Get the average fitness of the herd
     * @return Average fitness
     */
    double getAverageFitness() const;

    /**
     * @brief Get the worst fitness in the herd
     * @return Worst fitness
     */
    double getWorstFitness() const;

    /**
     * @brief Increment generation counter
     */
    void nextGeneration() { generation_++; }

private:
    /**
     * @brief Select horses for mating using tournament selection
     * @param tournament_size Size of the tournament
     * @return Selected horse index
     */
    int tournamentSelection(int tournament_size = 3);

    /**
     * @brief Select horses for replacement (worst first)
     * @param num_horses Number of horses to select
     * @return Vector of horse indices
     */
    std::vector<int> selectForReplacement(int num_horses);

    /**
     * @brief Create a new random horse
     * @return New horse
     */
    Horse createRandomHorse();

    /**
     * @brief Calculate distance between two solutions
     * @param sol1 First solution
     * @param sol2 Second solution
     * @return Distance measure
     */
    double calculateDistance(const Solution& sol1, const Solution& sol2) const;
};

#endif // HORSE_HERD_H
