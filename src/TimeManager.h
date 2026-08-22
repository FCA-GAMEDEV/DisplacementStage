#pragma once

#include <chrono>
#include <iostream>

class TimeManager
{
public:
    static TimeManager& getInstance(void);
    void  update(void);
    void  print(void);
    float getfDeltaTime(void);
    unsigned long getulDeltaTime(void);

private:
    TimeManager(void);
    TimeManager(const TimeManager&) {}
    TimeManager& operator=(const TimeManager&) { return *this; }
    ~TimeManager(void) {}

    static TimeManager* instance;

    using Clock     = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    TimePoint startFrame;

    float         fDeltaTime  = 0.f;
    unsigned long ulDeltaTime = 0;
};
