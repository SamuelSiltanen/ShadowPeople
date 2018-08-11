#pragma once

#include <stdint.h>

class Timer
{
public:
    Timer();

    void start();
    float stop();
    float stopAndRestart();
private:
    int64_t	m_ticksPerSecond;
	int64_t	m_time;
};
