#ifndef PROBLEM_INSTANCE_H
#define PROBLEM_INSTANCE_H

#include <vector>
#include <string>
#include <memory>

/**
 * @brief Represents a Flow Shop Scheduling Problem instance
 * 
 * This class encapsulates a FSSP instance with n jobs and m machines.
 * Each job has processing times for each machine.
 */
class ProblemInstance {
private:
    int num_jobs_;           // Number of jobs (n)
    int num_machines_;       // Number of machines (m)
    std::vector<std::vector<int>> processing_times_;  // processing_times_[job][machine]
    std::string instance_name_;

public:
    /**
     * @brief Constructor for ProblemInstance
     * @param num_jobs Number of jobs
     * @param num_machines Number of machines
     * @param instance_name Name of the instance
     */
    ProblemInstance(int num_jobs, int num_machines, const std::string& instance_name = "");

    /**
     * @brief Constructor from processing times matrix
     * @param processing_times Matrix of processing times [job][machine]
     * @param instance_name Name of the instance
     */
    ProblemInstance(const std::vector<std::vector<int>>& processing_times, 
                   const std::string& instance_name = "");

    // Getters
    int getNumJobs() const { return num_jobs_; }
    int getNumMachines() const { return num_machines_; }
    int getProcessingTime(int job, int machine) const;
    const std::vector<std::vector<int>>& getProcessingTimes() const { return processing_times_; }
    const std::string& getInstanceName() const { return instance_name_; }

    // Setters
    void setProcessingTime(int job, int machine, int time);
    void setInstanceName(const std::string& name) { instance_name_ = name; }

    /**
     * @brief Load instance from file
     * @param filename Path to the instance file
     * @return Shared pointer to the loaded instance
     */
    static std::shared_ptr<ProblemInstance> loadFromFile(const std::string& filename);

    /**
     * @brief Save instance to file
     * @param filename Path to save the instance
     * @return True if successful, false otherwise
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * @brief Generate random instance
     * @param num_jobs Number of jobs
     * @param num_machines Number of machines
     * @param min_time Minimum processing time
     * @param max_time Maximum processing time
     * @return Shared pointer to the generated instance
     */
    static std::shared_ptr<ProblemInstance> generateRandom(int num_jobs, int num_machines, 
                                                          int min_time = 1, int max_time = 100);

    /**
     * @brief Print instance information
     */
    void print() const;

    /**
     * @brief Validate the instance
     * @return True if valid, false otherwise
     */
    bool isValid() const;
};

#endif // PROBLEM_INSTANCE_H
