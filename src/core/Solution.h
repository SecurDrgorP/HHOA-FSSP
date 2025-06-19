#ifndef SOLUTION_H
#define SOLUTION_H

#include <vector>
#include <memory>
#include "ProblemInstance.h"

/**
 * @brief Represents a solution for the Flow Shop Scheduling Problem
 * 
 * A solution is represented as a permutation of jobs, indicating the order
 * in which jobs should be processed on all machines.
 */
class Solution {
private:
    std::vector<int> job_sequence_;          // Job permutation (0-indexed)
    std::shared_ptr<ProblemInstance> instance_;  // Problem instance
    mutable int makespan_;                   // Cached makespan value
    mutable bool makespan_calculated_;       // Flag to check if makespan is calculated
    mutable std::vector<std::vector<int>> completion_times_;  // Completion times [job][machine]

public:
    /**
     * @brief Constructor with problem instance
     * @param instance Shared pointer to the problem instance
     */
    explicit Solution(std::shared_ptr<ProblemInstance> instance);

    /**
     * @brief Constructor with job sequence and problem instance
     * @param job_sequence Initial job sequence
     * @param instance Shared pointer to the problem instance
     */
    Solution(const std::vector<int>& job_sequence, std::shared_ptr<ProblemInstance> instance);

    /**
     * @brief Copy constructor
     * @param other Solution to copy
     */
    Solution(const Solution& other);

    /**
     * @brief Assignment operator
     * @param other Solution to assign
     * @return Reference to this solution
     */
    Solution& operator=(const Solution& other);

    // Getters
    const std::vector<int>& getJobSequence() const { return job_sequence_; }
    std::shared_ptr<ProblemInstance> getInstance() const { return instance_; }
    int getNumJobs() const { return job_sequence_.size(); }
    int getJobAt(int position) const;

    // Setters
    void setJobSequence(const std::vector<int>& sequence);
    void setJobAt(int position, int job);
    void swapJobs(int pos1, int pos2);

    /**
     * @brief Calculate and return the makespan (total completion time)
     * @return Makespan value
     */
    int getMakespan() const;

    /**
     * @brief Get completion times matrix
     * @return Completion times [job_in_sequence][machine]
     */
    const std::vector<std::vector<int>>& getCompletionTimes() const;

    /**
     * @brief Get completion time for a specific job and machine
     * @param job_position Position of job in the sequence
     * @param machine Machine index
     * @return Completion time
     */
    int getCompletionTime(int job_position, int machine) const;

    /**
     * @brief Initialize with random job sequence
     */
    void initializeRandom();

    /**
     * @brief Initialize with greedy heuristic (shortest processing time first)
     */
    void initializeGreedy();

    /**
     * @brief Check if the solution is valid
     * @return True if valid, false otherwise
     */
    bool isValid() const;

    /**
     * @brief Apply 2-opt local search improvement
     * @return True if improvement was found, false otherwise
     */
    bool apply2Opt();

    /**
     * @brief Apply insertion-based local search
     * @return True if improvement was found, false otherwise
     */
    bool applyInsertionSearch();

    /**
     * @brief Create a neighbor solution by swapping two random jobs
     * @return New solution with two jobs swapped
     */
    Solution createSwapNeighbor() const;

    /**
     * @brief Create a neighbor solution by inserting a job at a different position
     * @return New solution with one job moved
     */
    Solution createInsertNeighbor() const;

    /**
     * @brief Calculate distance between two solutions (number of different positions)
     * @param other Other solution
     * @return Distance value
     */
    int distanceTo(const Solution& other) const;

    /**
     * @brief Print the solution
     */
    void print() const;

    /**
     * @brief Print detailed solution with Gantt chart
     */
    void printDetailed() const;

    /**
     * @brief Comparison operators
     */
    bool operator<(const Solution& other) const;
    bool operator>(const Solution& other) const;
    bool operator==(const Solution& other) const;
    bool operator!=(const Solution& other) const;

private:
    /**
     * @brief Calculate makespan and completion times
     */
    void calculateMakespan() const;

    /**
     * @brief Invalidate cached values
     */
    void invalidateCache();
};

#endif // SOLUTION_H
