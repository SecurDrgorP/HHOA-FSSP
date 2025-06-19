#include "Random.h"
#include <chrono>
#include <stdexcept>

Random Random::instance_;

Random::Random() {
    // Initialize with current time as seed
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto seed = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    generator_.seed(static_cast<unsigned int>(seed));
}

Random& Random::getInstance() {
    return instance_;
}

void Random::setSeed(unsigned int seed) {
    generator_.seed(seed);
}

int Random::randInt(int min, int max) {
    if (min > max) {
        throw std::invalid_argument("min cannot be greater than max");
    }
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(generator_);
}

double Random::randDouble() {
    std::uniform_real_distribution<double> distribution(0.0, 1.0);
    return distribution(generator_);
}

double Random::randDouble(double min, double max) {
    if (min >= max) {
        throw std::invalid_argument("min must be less than max");
    }
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(generator_);
}

bool Random::randBool(double probability) {
    if (probability < 0.0 || probability > 1.0) {
        throw std::invalid_argument("probability must be between 0.0 and 1.0");
    }
    return randDouble() < probability;
}

std::vector<int> Random::randPermutation(int n) {
    if (n < 0) {
        throw std::invalid_argument("n must be non-negative");
    }
    
    std::vector<int> permutation(n);
    for (int i = 0; i < n; ++i) {
        permutation[i] = i;
    }
    shuffle(permutation);
    return permutation;
}

std::vector<int> Random::sample(int population_size, int sample_size) {
    if (population_size < 0 || sample_size < 0) {
        throw std::invalid_argument("sizes must be non-negative");
    }
    if (sample_size > population_size) {
        throw std::invalid_argument("sample size cannot be larger than population size");
    }
    
    std::vector<int> population(population_size);
    for (int i = 0; i < population_size; ++i) {
        population[i] = i;
    }
    
    shuffle(population);
    return std::vector<int>(population.begin(), population.begin() + sample_size);
}

double Random::randNormal(double mean, double stddev) {
    if (stddev <= 0.0) {
        throw std::invalid_argument("standard deviation must be positive");
    }
    std::normal_distribution<double> distribution(mean, stddev);
    return distribution(generator_);
}

double Random::randExponential(double lambda) {
    if (lambda <= 0.0) {
        throw std::invalid_argument("lambda must be positive");
    }
    std::exponential_distribution<double> distribution(lambda);
    return distribution(generator_);
}
