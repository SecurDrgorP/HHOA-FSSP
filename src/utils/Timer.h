#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

/**
 * @brief Simple timer class for measuring execution time
 */
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    bool is_running_;
    std::string name_;

public:
    /**
     * @brief Constructor
     * @param name Optional name for the timer
     */
    explicit Timer(const std::string& name = "Timer");

    /**
     * @brief Start the timer
     */
    void start();

    /**
     * @brief Stop the timer
     */
    void stop();

    /**
     * @brief Reset the timer
     */
    void reset();

    /**
     * @brief Get elapsed time in milliseconds
     * @return Elapsed time in milliseconds
     */
    double getElapsedMs() const;

    /**
     * @brief Get elapsed time in seconds
     * @return Elapsed time in seconds
     */
    double getElapsedSeconds() const;

    /**
     * @brief Get elapsed time in microseconds
     * @return Elapsed time in microseconds
     */
    long long getElapsedMicroseconds() const;

    /**
     * @brief Check if timer is running
     * @return True if running, false otherwise
     */
    bool isRunning() const { return is_running_; }

    /**
     * @brief Get timer name
     * @return Timer name
     */
    const std::string& getName() const { return name_; }

    /**
     * @brief Set timer name
     * @param name New timer name
     */
    void setName(const std::string& name) { name_ = name; }

    /**
     * @brief Print elapsed time
     */
    void printElapsed() const;

    /**
     * @brief Get formatted elapsed time string
     * @return Formatted time string
     */
    std::string getFormattedTime() const;
};

/**
 * @brief RAII timer class that automatically starts on construction and stops on destruction
 */
class ScopedTimer {
private:
    Timer timer_;

public:
    /**
     * @brief Constructor that starts timing
     * @param name Optional name for the timer
     */
    explicit ScopedTimer(const std::string& name = "ScopedTimer");

    /**
     * @brief Destructor that stops timing and prints result
     */
    ~ScopedTimer();

    /**
     * @brief Get the underlying timer
     * @return Reference to the timer
     */
    Timer& getTimer() { return timer_; }
};

#endif // TIMER_H
