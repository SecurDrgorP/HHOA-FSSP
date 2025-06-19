#include "../src/core/ProblemInstance.h"
#include "../src/core/Solution.h"
#include "../src/algorithm/HHOA.h"
#include "../src/utils/Random.h"
#include <iostream>
#include <cassert>

void testProblemInstance() {
    std::cout << "Testing ProblemInstance..." << std::endl;
    
    // Test creation
    auto instance = ProblemInstance::generateRandom(5, 3, 1, 10);
    assert(instance != nullptr);
    assert(instance->getNumJobs() == 5);
    assert(instance->getNumMachines() == 3);
    assert(instance->isValid());
    
    // Test processing times
    for (int j = 0; j < 5; ++j) {
        for (int m = 0; m < 3; ++m) {
            int time = instance->getProcessingTime(j, m);
            assert(time >= 1 && time <= 10);
        }
    }
    
    std::cout << "ProblemInstance tests passed!" << std::endl;
}

void testSolution() {
    std::cout << "Testing Solution..." << std::endl;
    
    auto instance = ProblemInstance::generateRandom(4, 3, 5, 15);
    Solution solution(instance);
    
    // Test basic properties
    assert(solution.getNumJobs() == 4);
    assert(solution.isValid());
    
    // Test makespan calculation
    int makespan = solution.getMakespan();
    assert(makespan > 0);
    
    // Test job sequence operations
    solution.swapJobs(0, 1);
    assert(solution.isValid());
    
    std::cout << "Solution tests passed!" << std::endl;
}

void testHHOA() {
    std::cout << "Testing HHOA..." << std::endl;
    
    auto instance = ProblemInstance::generateRandom(6, 4, 1, 20);
    
    HHOAParameters params;
    params.population_size = 10;
    params.max_iterations = 50;
    
    HHOA algorithm(instance, params);
    Solution best = algorithm.optimize();
    
    assert(best.isValid());
    assert(best.getMakespan() > 0);
    
    std::cout << "Best makespan: " << best.getMakespan() << std::endl;
    std::cout << "HHOA tests passed!" << std::endl;
}

int main() {
    try {
        std::cout << "Running HHOA-FSSP Tests..." << std::endl;
        std::cout << "=========================" << std::endl;
        
        // Set random seed for reproducible tests
        Random::getInstance().setSeed(42);
        
        testProblemInstance();
        testSolution();
        testHHOA();
        
        std::cout << std::endl;
        std::cout << "All tests passed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
