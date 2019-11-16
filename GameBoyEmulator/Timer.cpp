#include "Timer.h"



Timer::Timer(CPU* cpu)
{
	this->cpu = cpu;
	lastTimer = now;
	long sinceLast = now - lastTimer;
	
}

void Timer::update()
{
	now = std::chrono::steady_clock::now();

}

bool Timer::needScreenRefresh()
{
	auto elapsed = std::chrono::steady_clock::now() - now;
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	return microseconds > 1667;
}


Timer::~Timer()
{
}
