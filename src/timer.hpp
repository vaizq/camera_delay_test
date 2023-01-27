#pragma once

#include <chrono>


using std::chrono::steady_clock;

class Timer
{
public:
    Timer()
    {
        start();
    }

    void start()
    {
        timeStart = steady_clock::now();
    }

    template <typename T>
    std::chrono::duration<T> time()
    {
        auto now = steady_clock::now();
        return std::chrono::duration_cast<T>(now - timeStart);
    }

private:
    steady_clock::time_point timeStart;
};
