#include "Timer.hpp"

// Unfortunately, this is windows-specific
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Errors.hpp"

Timer::Timer()
{
    SP_ASSERT(QueryPerformanceFrequency((LARGE_INTEGER *)&m_ticksPerSecond),
              "Failed to query performance counter");
}

void Timer::start()
{
    SP_ASSERT(QueryPerformanceCounter((LARGE_INTEGER *)&m_time),
              "Failed to query performace counter");
}

float Timer::stop()
{
    int64_t currentTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	return static_cast<float>(currentTime - m_time) / m_ticksPerSecond;	
}

float Timer::stopAndRestart()
{
    int64_t currentTime;
	QueryPerformanceCounter((LARGE_INTEGER *)&currentTime);
	float seconds = static_cast<float>(currentTime - m_time) / m_ticksPerSecond;
	m_time = currentTime;
    return seconds;
}