#include "TimeManager.h"

TimeManager* TimeManager::instance = nullptr;

TimeManager::TimeManager(void)
    : startFrame(Clock::now())
    , fDeltaTime(0.f)
    , ulDeltaTime(0)
{
}

TimeManager& TimeManager::getInstance()
{
    if (!TimeManager::instance)
        TimeManager::instance = new TimeManager;
    return *TimeManager::instance;
}

void TimeManager::update(void)
{
    auto now = Clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - startFrame);
    ulDeltaTime = static_cast<unsigned long>(elapsed.count() / 1000); // em milissegundos
    fDeltaTime  = elapsed.count() * 1e-6f;                            // em segundos
    startFrame  = now;
}

void TimeManager::print(void)
{
    std::cout << "deltaTime: " << fDeltaTime << " s\n";
}

float TimeManager::getfDeltaTime(void)    { return fDeltaTime; }
unsigned long TimeManager::getulDeltaTime(void) { return ulDeltaTime; }
