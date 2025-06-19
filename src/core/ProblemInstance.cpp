#include "ProblemInstance.h"
#include "../utils/Random.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

ProblemInstance::ProblemInstance(int num_jobs, int num_machines, const std::string& instance_name)
    : num_jobs_(num_jobs), num_machines_(num_machines), instance_name_(instance_name) {
    processing_times_.resize(num_jobs_, std::vector<int>(num_machines_, 0));
}

ProblemInstance::ProblemInstance(const std::vector<std::vector<int>>& processing_times, 
                               const std::string& instance_name)
    : processing_times_(processing_times), instance_name_(instance_name) {
    num_jobs_ = processing_times.size();
    num_machines_ = processing_times.empty() ? 0 : processing_times[0].size();
}

int ProblemInstance::getProcessingTime(int job, int machine) const {
    if (job < 0 || job >= num_jobs_ || machine < 0 || machine >= num_machines_) {
        throw std::out_of_range("Invalid job or machine index");
    }
    return processing_times_[job][machine];
}

void ProblemInstance::setProcessingTime(int job, int machine, int time) {
    if (job < 0 || job >= num_jobs_ || machine < 0 || machine >= num_machines_) {
        throw std::out_of_range("Invalid job or machine index");
    }
    if (time < 0) {
        throw std::invalid_argument("Processing time cannot be negative");
    }
    processing_times_[job][machine] = time;
}

std::shared_ptr<ProblemInstance> ProblemInstance::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return nullptr;
    }

    int num_jobs, num_machines;
    file >> num_jobs >> num_machines;

    if (num_jobs <= 0 || num_machines <= 0) {
        std::cerr << "Error: Invalid problem dimensions" << std::endl;
        return nullptr;
    }

    auto instance = std::make_shared<ProblemInstance>(num_jobs, num_machines, filename);

    for (int job = 0; job < num_jobs; ++job) {
        for (int machine = 0; machine < num_machines; ++machine) {
            int time;
            file >> time;
            instance->setProcessingTime(job, machine, time);
        }
    }

    file.close();
    return instance;
}

bool ProblemInstance::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot create file " << filename << std::endl;
        return false;
    }

    file << num_jobs_ << " " << num_machines_ << std::endl;
    
    for (int job = 0; job < num_jobs_; ++job) {
        for (int machine = 0; machine < num_machines_; ++machine) {
            file << processing_times_[job][machine];
            if (machine < num_machines_ - 1) file << " ";
        }
        file << std::endl;
    }

    file.close();
    return true;
}

std::shared_ptr<ProblemInstance> ProblemInstance::generateRandom(int num_jobs, int num_machines, 
                                                               int min_time, int max_time) {
    if (num_jobs <= 0 || num_machines <= 0 || min_time < 0 || max_time < min_time) {
        throw std::invalid_argument("Invalid parameters for random instance generation");
    }

    auto instance = std::make_shared<ProblemInstance>(num_jobs, num_machines, 
                                                     "Random_" + std::to_string(num_jobs) + "x" + std::to_string(num_machines));

    for (int job = 0; job < num_jobs; ++job) {
        for (int machine = 0; machine < num_machines; ++machine) {
            int time = Random::getInstance().randInt(min_time, max_time);
            instance->setProcessingTime(job, machine, time);
        }
    }

    return instance;
}

void ProblemInstance::print() const {
    std::cout << "Problem Instance: " << instance_name_ << std::endl;
    std::cout << "Jobs: " << num_jobs_ << ", Machines: " << num_machines_ << std::endl;
    std::cout << "Processing Times:" << std::endl;
    
    // Print header
    std::cout << std::setw(6) << "Job";
    for (int machine = 0; machine < num_machines_; ++machine) {
        std::cout << std::setw(6) << "M" + std::to_string(machine + 1);
    }
    std::cout << std::endl;
    
    // Print processing times
    for (int job = 0; job < num_jobs_; ++job) {
        std::cout << std::setw(6) << "J" + std::to_string(job + 1);
        for (int machine = 0; machine < num_machines_; ++machine) {
            std::cout << std::setw(6) << processing_times_[job][machine];
        }
        std::cout << std::endl;
    }
}

bool ProblemInstance::isValid() const {
    if (num_jobs_ <= 0 || num_machines_ <= 0) {
        return false;
    }
    
    if (processing_times_.size() != static_cast<size_t>(num_jobs_)) {
        return false;
    }
    
    for (const auto& job_times : processing_times_) {
        if (job_times.size() != static_cast<size_t>(num_machines_)) {
            return false;
        }
        for (int time : job_times) {
            if (time < 0) {
                return false;
            }
        }
    }
    
    return true;
}
