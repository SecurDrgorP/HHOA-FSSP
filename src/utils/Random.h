#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <vector>
#include <algorithm>
#include <stdexcept>

/**
 * @brief Singleton class for random number generation
 * 
 * This class provides various random number generation utilities
 * using the Mersenne Twister algorithm.
 */
class Random {
private:
    std::mt19937 generator_;
    static Random instance_;

    /**
     * @brief Private constructor for singleton pattern
     */
    Random();

public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the Random instance
     */
    static Random& getInstance();

    /**
     * @brief Set the random seed
     * @param seed Seed value for the random number generator
     */
    void setSeed(unsigned int seed);

    /**
     * @brief Generate random integer in range [min, max]
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return Random integer in the specified range
     */
    int randInt(int min, int max);

    /**
     * @brief Generate random double in range [0.0, 1.0)
     * @return Random double value
     */
    double randDouble();

    /**
     * @brief Generate random double in range [min, max)
     * @param min Minimum value (inclusive)
     * @param max Maximum value (exclusive)
     * @return Random double in the specified range
     */
    double randDouble(double min, double max);

    /**
     * @brief Generate random boolean value
     * @param probability Probability of returning true (default: 0.5)
     * @return Random boolean value
     */
    bool randBool(double probability = 0.5);

    /**
     * @brief Shuffle a vector randomly
     * @param vec Vector to shuffle
     */
    template<typename T>
    void shuffle(std::vector<T>& vec);

    /**
     * @brief Select a random element from a vector
     * @param vec Vector to select from
     * @return Reference to a random element
     */
    template<typename T>
    const T& choice(const std::vector<T>& vec);

    /**
     * @brief Generate random permutation of integers [0, n-1]
     * @param n Size of the permutation
     * @return Random permutation vector
     */
    std::vector<int> randPermutation(int n);

    /**
     * @brief Generate random sample without replacement
     * @param population_size Size of the population to sample from
     * @param sample_size Size of the sample
     * @return Vector of sampled indices
     */
    std::vector<int> sample(int population_size, int sample_size);

    /**
     * @brief Generate normally distributed random number
     * @param mean Mean of the distribution
     * @param stddev Standard deviation of the distribution
     * @return Normally distributed random number
     */
    double randNormal(double mean = 0.0, double stddev = 1.0);

    /**
     * @brief Generate exponentially distributed random number
     * @param lambda Rate parameter
     * @return Exponentially distributed random number
     */
    double randExponential(double lambda = 1.0);
};

// Template implementations
template<typename T>
void Random::shuffle(std::vector<T>& vec) {
    std::shuffle(vec.begin(), vec.end(), generator_);
}

template<typename T>
const T& Random::choice(const std::vector<T>& vec) {
    if (vec.empty()) {
        throw std::invalid_argument("Cannot choose from empty vector");
    }
    int index = randInt(0, vec.size() - 1);
    return vec[index];
}

#endif // RANDOM_H
