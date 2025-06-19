#include "Timer.h"
#include <iostream>
#include <iomanip>
#include <sstream>

Timer::Timer(const std::string& name) : name_(name), is_running_(false) {}

void Timer::start() {
    start_time_ = std::chrono::high_resolution_clock::now();
    is_running_ = true;
}

void Timer::stop() {
    if (is_running_) {
        end_time_ = std::chrono::high_resolution_clock::now();
        is_running_ = false;
    }
}

void Timer::reset() {
    is_running_ = false;
    start_time_ = std::chrono::high_resolution_clock::time_point();
    end_time_ = std::chrono::high_resolution_clock::time_point();
}

double Timer::getElapsedMs() const {
    auto end = is_running_ ? std::chrono::high_resolution_clock::now() : end_time_;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_);
    return duration.count() / 1000.0;
}

double Timer::getElapsedSeconds() const {
    return getElapsedMs() / 1000.0;
}

long long Timer::getElapsedMicroseconds() const {
    auto end = is_running_ ? std::chrono::high_resolution_clock::now() : end_time_;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_);
    return duration.count();
}

void Timer::printElapsed() const {
    std::cout << name_ << ": " << getFormattedTime() << std::endl;
}

std::string Timer::getFormattedTime() const {
    double seconds = getElapsedSeconds();
    std::ostringstream oss;
    
    if (seconds < 1.0) {
        oss << std::fixed << std::setprecision(3) << getElapsedMs() << " ms";
    } else if (seconds < 60.0) {
        oss << std::fixed << std::setprecision(3) << seconds << " s";
    } else {
        int minutes = static_cast<int>(seconds / 60);
        double remaining_seconds = seconds - (minutes * 60);
        oss << minutes << "m " << std::fixed << std::setprecision(1) << remaining_seconds << "s";
    }
    
    return oss.str();
}

ScopedTimer::ScopedTimer(const std::string& name) : timer_(name) {
    timer_.start();
}

ScopedTimer::~ScopedTimer() {
    timer_.stop();
    timer_.printElapsed();
}
