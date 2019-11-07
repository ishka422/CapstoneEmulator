#include "Timer.h"



Timer::Timer()
{
	lastTimer = now;
	long sinceLast = now - lastTimer;
	double cyclesToCatchUp = 4.19 * sinceLast;
	int cycles = 0;
}


Timer::~Timer()
{
}
