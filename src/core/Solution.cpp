#include "Solution.h"
#include "../utils/Random.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <climits>

Solution::Solution(std::shared_ptr<ProblemInstance> instance)
    : instance_(instance), makespan_(-1), makespan_calculated_(false) {
    if (!instance_ || !instance_->isValid()) {
        throw std::invalid_argument("Invalid problem instance");
    }
    
    int num_jobs = instance_->getNumJobs();
    job_sequence_.resize(num_jobs);
    std::iota(job_sequence_.begin(), job_sequence_.end(), 0);  // Initialize with 0, 1, 2, ..., n-1
}

Solution::Solution(const std::vector<int>& job_sequence, std::shared_ptr<ProblemInstance> instance)
    : job_sequence_(job_sequence), instance_(instance), makespan_(-1), makespan_calculated_(false) {
    if (!instance_ || !instance_->isValid()) {
        throw std::invalid_argument("Invalid problem instance");
    }
    
    if (job_sequence_.size() != static_cast<size_t>(instance_->getNumJobs())) {
        throw std::invalid_argument("Job sequence size does not match problem instance");
    }
}

Solution::Solution(const Solution& other)
    : job_sequence_(other.job_sequence_), instance_(other.instance_),
      makespan_(other.makespan_), makespan_calculated_(other.makespan_calculated_),
      completion_times_(other.completion_times_) {}

Solution& Solution::operator=(const Solution& other) {
    if (this != &other) {
        job_sequence_ = other.job_sequence_;
        instance_ = other.instance_;
        makespan_ = other.makespan_;
        makespan_calculated_ = other.makespan_calculated_;
        completion_times_ = other.completion_times_;
    }
    return *this;
}

int Solution::getJobAt(int position) const {
    if (position < 0 || position >= static_cast<int>(job_sequence_.size())) {
        throw std::out_of_range("Invalid position");
    }
    return job_sequence_[position];
}

void Solution::setJobSequence(const std::vector<int>& sequence) {
    if (sequence.size() != static_cast<size_t>(instance_->getNumJobs())) {
        throw std::invalid_argument("Job sequence size does not match problem instance");
    }
    job_sequence_ = sequence;
    invalidateCache();
}

void Solution::setJobAt(int position, int job) {
    if (position < 0 || position >= static_cast<int>(job_sequence_.size())) {
        throw std::out_of_range("Invalid position");
    }
    job_sequence_[position] = job;
    invalidateCache();
}

void Solution::swapJobs(int pos1, int pos2) {
    if (pos1 < 0 || pos1 >= static_cast<int>(job_sequence_.size()) ||
        pos2 < 0 || pos2 >= static_cast<int>(job_sequence_.size())) {
        throw std::out_of_range("Invalid positions");
    }
    std::swap(job_sequence_[pos1], job_sequence_[pos2]);
    invalidateCache();
}

int Solution::getMakespan() const {
    if (!makespan_calculated_) {
        calculateMakespan();
    }
    return makespan_;
}

const std::vector<std::vector<int>>& Solution::getCompletionTimes() const {
    if (!makespan_calculated_) {
        calculateMakespan();
    }
    return completion_times_;
}

int Solution::getCompletionTime(int job_position, int machine) const {
    if (!makespan_calculated_) {
        calculateMakespan();
    }
    
    if (job_position < 0 || job_position >= static_cast<int>(completion_times_.size()) ||
        machine < 0 || machine >= static_cast<int>(completion_times_[0].size())) {
        throw std::out_of_range("Invalid job position or machine index");
    }
    
    return completion_times_[job_position][machine];
}

void Solution::initializeRandom() {
    Random& rng = Random::getInstance();
    rng.shuffle(job_sequence_);
    invalidateCache();
}

void Solution::initializeGreedy() {
    // Simple greedy heuristic: sort jobs by total processing time
    std::vector<std::pair<int, int>> job_total_times;
    
    for (int job = 0; job < instance_->getNumJobs(); ++job) {
        int total_time = 0;
        for (int machine = 0; machine < instance_->getNumMachines(); ++machine) {
            total_time += instance_->getProcessingTime(job, machine);
        }
        job_total_times.emplace_back(total_time, job);
    }
    
    // Sort by total processing time (shortest first)
    std::sort(job_total_times.begin(), job_total_times.end());
    
    for (size_t i = 0; i < job_total_times.size(); ++i) {
        job_sequence_[i] = job_total_times[i].second;
    }
    
    invalidateCache();
}

bool Solution::isValid() const {
    if (job_sequence_.size() != static_cast<size_t>(instance_->getNumJobs())) {
        return false;
    }
    
    std::vector<bool> used(instance_->getNumJobs(), false);
    for (int job : job_sequence_) {
        if (job < 0 || job >= instance_->getNumJobs() || used[job]) {
            return false;
        }
        used[job] = true;
    }
    
    return true;
}

bool Solution::apply2Opt() {
    bool improved = false;
    int current_makespan = getMakespan();
    
    for (int i = 0; i < static_cast<int>(job_sequence_.size()) - 1; ++i) {
        for (int j = i + 1; j < static_cast<int>(job_sequence_.size()); ++j) {
            // Try swapping jobs at positions i and j
            swapJobs(i, j);
            
            int new_makespan = getMakespan();
            if (new_makespan < current_makespan) {
                current_makespan = new_makespan;
                improved = true;
            } else {
                // Revert the swap
                swapJobs(i, j);
            }
        }
    }
    
    return improved;
}

bool Solution::applyInsertionSearch() {
    bool improved = false;
    int current_makespan = getMakespan();
    
    for (int i = 0; i < static_cast<int>(job_sequence_.size()); ++i) {
        for (int j = 0; j < static_cast<int>(job_sequence_.size()); ++j) {
            if (i == j) continue;
            
            // Remove job from position i and insert at position j
            int job = job_sequence_[i];
            job_sequence_.erase(job_sequence_.begin() + i);
            job_sequence_.insert(job_sequence_.begin() + (j > i ? j - 1 : j), job);
            
            int new_makespan = getMakespan();
            if (new_makespan < current_makespan) {
                current_makespan = new_makespan;
                improved = true;
            } else {
                // Revert the move
                job_sequence_.erase(job_sequence_.begin() + (j > i ? j - 1 : j));
                job_sequence_.insert(job_sequence_.begin() + i, job);
                invalidateCache();
            }
        }
    }
    
    return improved;
}

Solution Solution::createSwapNeighbor() const {
    Solution neighbor(*this);
    Random& rng = Random::getInstance();
    
    int pos1 = rng.randInt(0, job_sequence_.size() - 1);
    int pos2 = rng.randInt(0, job_sequence_.size() - 1);
    
    neighbor.swapJobs(pos1, pos2);
    return neighbor;
}

Solution Solution::createInsertNeighbor() const {
    Solution neighbor(*this);
    Random& rng = Random::getInstance();
    
    int from_pos = rng.randInt(0, job_sequence_.size() - 1);
    int to_pos = rng.randInt(0, job_sequence_.size() - 1);
    
    if (from_pos != to_pos) {
        int job = neighbor.job_sequence_[from_pos];
        neighbor.job_sequence_.erase(neighbor.job_sequence_.begin() + from_pos);
        neighbor.job_sequence_.insert(neighbor.job_sequence_.begin() + (to_pos > from_pos ? to_pos - 1 : to_pos), job);
        neighbor.invalidateCache();
    }
    
    return neighbor;
}

int Solution::distanceTo(const Solution& other) const {
    if (job_sequence_.size() != other.job_sequence_.size()) {
        return INT_MAX;
    }
    
    int distance = 0;
    for (size_t i = 0; i < job_sequence_.size(); ++i) {
        if (job_sequence_[i] != other.job_sequence_[i]) {
            distance++;
        }
    }
    
    return distance;
}

void Solution::print() const {
    std::cout << "Job Sequence: ";
    for (size_t i = 0; i < job_sequence_.size(); ++i) {
        std::cout << "J" << (job_sequence_[i] + 1);
        if (i < job_sequence_.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    std::cout << "Makespan: " << getMakespan() << std::endl;
}

void Solution::printDetailed() const {
    print();
    
    const auto& completion_times = getCompletionTimes();
    
    std::cout << "\nCompletion Times:" << std::endl;
    std::cout << std::setw(8) << "Position";
    for (int machine = 0; machine < instance_->getNumMachines(); ++machine) {
        std::cout << std::setw(8) << ("M" + std::to_string(machine + 1));
    }
    std::cout << std::endl;
    
    for (int pos = 0; pos < static_cast<int>(job_sequence_.size()); ++pos) {
        std::cout << std::setw(8) << ("J" + std::to_string(job_sequence_[pos] + 1));
        for (int machine = 0; machine < instance_->getNumMachines(); ++machine) {
            std::cout << std::setw(8) << completion_times[pos][machine];
        }
        std::cout << std::endl;
    }
}

bool Solution::operator<(const Solution& other) const {
    return getMakespan() < other.getMakespan();
}

bool Solution::operator>(const Solution& other) const {
    return getMakespan() > other.getMakespan();
}

bool Solution::operator==(const Solution& other) const {
    return job_sequence_ == other.job_sequence_;
}

bool Solution::operator!=(const Solution& other) const {
    return !(*this == other);
}

void Solution::calculateMakespan() const {
    int num_jobs = job_sequence_.size();
    int num_machines = instance_->getNumMachines();
    
    completion_times_.assign(num_jobs, std::vector<int>(num_machines, 0));
    
    for (int pos = 0; pos < num_jobs; ++pos) {
        int job = job_sequence_[pos];
        
        for (int machine = 0; machine < num_machines; ++machine) {
            int processing_time = instance_->getProcessingTime(job, machine);
            
            if (pos == 0 && machine == 0) {
                // First job on first machine
                completion_times_[pos][machine] = processing_time;
            } else if (pos == 0) {
                // First job on other machines
                completion_times_[pos][machine] = completion_times_[pos][machine - 1] + processing_time;
            } else if (machine == 0) {
                // Other jobs on first machine
                completion_times_[pos][machine] = completion_times_[pos - 1][machine] + processing_time;
            } else {
                // Other jobs on other machines
                completion_times_[pos][machine] = std::max(completion_times_[pos - 1][machine],
                                                         completion_times_[pos][machine - 1]) + processing_time;
            }
        }
    }
    
    makespan_ = num_jobs > 0 ? completion_times_[num_jobs - 1][num_machines - 1] : 0;
    makespan_calculated_ = true;
}

void Solution::invalidateCache() {
    makespan_calculated_ = false;
    makespan_ = -1;
    completion_times_.clear();
}
