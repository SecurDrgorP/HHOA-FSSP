#include "core/ProblemInstance.h"
#include "core/Solution.h"
#include "algorithm/HHOA.h"
#include "utils/Logger.h"
#include "utils/Timer.h"
#include "utils/Random.h"
#include <iostream>
#include <string>
#include <memory>

void printBanner() {
    std::cout << "=========================================" << std::endl;
    std::cout << "  Horse Herd Optimization Algorithm" << std::endl;
    std::cout << "  for Flow Shop Scheduling Problem" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -f <file>        Load problem instance from file" << std::endl;
    std::cout << "  -j <jobs>        Number of jobs (for random instance)" << std::endl;
    std::cout << "  -m <machines>    Number of machines (for random instance)" << std::endl;
    std::cout << "  -p <population>  Population size (default: 30)" << std::endl;
    std::cout << "  -i <iterations>  Maximum iterations (default: 1000)" << std::endl;
    std::cout << "  -s <seed>        Random seed (default: time-based)" << std::endl;
    std::cout << "  -o <output>      Output file for results" << std::endl;
    std::cout << "  -v              Verbose output" << std::endl;
    std::cout << "  -h              Show this help" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << program_name << " -j 10 -m 5 -p 30 -i 1000" << std::endl;
    std::cout << "  " << program_name << " -f data/instances/ta001.txt -v" << std::endl;
}

std::shared_ptr<ProblemInstance> createTestInstance() {
    // Create a small test instance
    std::vector<std::vector<int>> processing_times = {
        {54, 83, 15, 71, 77, 36, 53, 38, 27, 87},  // Job 1
        {79, 3, 11, 99, 56, 70, 99, 60, 5, 56},    // Job 2
        {16, 89, 49, 15, 89, 45, 60, 23, 57, 64},  // Job 3
        {58, 90, 78, 99, 96, 33, 75, 98, 25, 43},  // Job 4
        {35, 9, 28, 25, 84, 52, 98, 80, 89, 69},   // Job 5
        {76, 63, 84, 52, 44, 6, 69, 48, 45, 11},   // Job 6
        {28, 35, 95, 92, 35, 94, 91, 21, 66, 88},  // Job 7
        {9, 92, 35, 91, 52, 42, 99, 78, 59, 83},   // Job 8
        {74, 60, 88, 32, 17, 18, 85, 36, 43, 96},  // Job 9
        {77, 32, 38, 99, 25, 69, 79, 60, 77, 94}   // Job 10
    };
    
    return std::make_shared<ProblemInstance>(processing_times, "TestInstance_10x10");
}

void iterationCallback(int iteration, const Solution& best_solution, const HHOAStatistics& stats) {
    if (iteration % 100 == 0) {
        std::cout << "Iteration " << iteration 
                  << " - Best Makespan: " << best_solution.getMakespan() 
                  << " - Improvements: " << stats.total_improvements << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        printBanner();
        
        // Parse command line arguments
        std::string instance_file;
        std::string output_file;
        int num_jobs = 10;
        int num_machines = 5;
        int population_size = 30;
        int max_iterations = 1000;
        unsigned int seed = 0;
        bool verbose = false;
        bool use_file = false;
        
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "-f" && i + 1 < argc) {
                instance_file = argv[++i];
                use_file = true;
            } else if (arg == "-j" && i + 1 < argc) {
                num_jobs = std::stoi(argv[++i]);
            } else if (arg == "-m" && i + 1 < argc) {
                num_machines = std::stoi(argv[++i]);
            } else if (arg == "-p" && i + 1 < argc) {
                population_size = std::stoi(argv[++i]);
            } else if (arg == "-i" && i + 1 < argc) {
                max_iterations = std::stoi(argv[++i]);
            } else if (arg == "-s" && i + 1 < argc) {
                seed = std::stoul(argv[++i]);
            } else if (arg == "-o" && i + 1 < argc) {
                output_file = argv[++i];
            } else if (arg == "-v") {
                verbose = true;
            }
        }
        
        // Initialize logger
        Logger& logger = Logger::getInstance();
        logger.initialize("../data/results/hhoa_log.txt", verbose ? LogLevel::DEBUG : LogLevel::INFO, true);
        
        // Set random seed
        Random& rng = Random::getInstance();
        if (seed != 0) {
            rng.setSeed(seed);
            LOG_INFO("Using random seed: " + std::to_string(seed));
        }
        
        // Load or create problem instance
        std::shared_ptr<ProblemInstance> instance;
        
        if (use_file) {
            LOG_INFO("Loading instance from file: " + instance_file);
            instance = ProblemInstance::loadFromFile(instance_file);
            if (!instance) {
                std::cerr << "Error: Failed to load instance from " << instance_file << std::endl;
                return 1;
            }
        } else {
            if (num_jobs == 10 && num_machines == 5) {
                // Use predefined test instance
                LOG_INFO("Using predefined test instance (10x10)");
                instance = createTestInstance();
            } else {
                // Generate random instance
                LOG_INFO("Generating random instance: " + std::to_string(num_jobs) + "x" + std::to_string(num_machines));
                instance = ProblemInstance::generateRandom(num_jobs, num_machines, 1, 100);
            }
        }
        
        if (!instance || !instance->isValid()) {
            std::cerr << "Error: Invalid problem instance" << std::endl;
            return 1;
        }
        
        // Print instance information
        std::cout << "Problem Instance: " << instance->getInstanceName() << std::endl;
        std::cout << "Jobs: " << instance->getNumJobs() << ", Machines: " << instance->getNumMachines() << std::endl;
        std::cout << std::endl;
        
        if (verbose) {
            instance->print();
            std::cout << std::endl;
        }
        
        // Configure HHOA parameters
        HHOAParameters params;
        params.population_size = population_size;
        params.max_iterations = max_iterations;
        params.adaptive_parameters = true;
        
        if (verbose) {
            params.print();
            std::cout << std::endl;
        }
        
        // Create and run HHOA
        std::cout << "Starting HHOA optimization..." << std::endl;
        std::cout << "Population: " << population_size << ", Iterations: " << max_iterations << std::endl;
        std::cout << std::endl;
        
        HHOA algorithm(instance, params);
        
        // Set callback for progress reporting
        if (verbose) {
            algorithm.setIterationCallback(iterationCallback);
        }
        
        // Run optimization
        ScopedTimer optimization_timer("Optimization");
        Solution best_solution = algorithm.optimize();
        
        // Print results
        std::cout << std::endl;
        std::cout << "=== OPTIMIZATION RESULTS ===" << std::endl;
        std::cout << "Best Makespan: " << best_solution.getMakespan() << std::endl;
        std::cout << "Execution Time: " << algorithm.getStatistics().execution_time_ms << " ms" << std::endl;
        std::cout << "Iterations: " << algorithm.getStatistics().iterations_executed << std::endl;
        std::cout << "Total Improvements: " << algorithm.getStatistics().total_improvements << std::endl;
        std::cout << "Leader Changes: " << algorithm.getStatistics().leader_changes << std::endl;
        std::cout << std::endl;
        
        // Print best solution
        std::cout << "Best Solution:" << std::endl;
        best_solution.print();
        std::cout << std::endl;
        
        if (verbose) {
            best_solution.printDetailed();
            std::cout << std::endl;
            algorithm.printDetailed();
        }
        
        // Save results if output file specified
        if (!output_file.empty()) {
            if (algorithm.saveResults(output_file)) {
                std::cout << "Results saved to: " << output_file << std::endl;
            } else {
                std::cerr << "Warning: Failed to save results to " << output_file << std::endl;
            }
            
            // Save statistics
            std::string stats_file = output_file + "_stats.csv";
            if (algorithm.getStatistics().saveToFile(stats_file)) {
                std::cout << "Statistics saved to: " << stats_file << std::endl;
            }
        }
        
        // Save test instance if it was generated
        if (!use_file && num_jobs != 10) {
            std::string instance_filename = "data/instances/generated_" + 
                                          std::to_string(num_jobs) + "x" + 
                                          std::to_string(num_machines) + ".txt";
            if (instance->saveToFile(instance_filename)) {
                std::cout << "Instance saved to: " << instance_filename << std::endl;
            }
        }
        
        LOG_INFO("HHOA execution completed successfully");
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        LOG_ERROR("Exception: " + std::string(e.what()));
        return 1;
    }
    
    return 0;
}
