#pragma once

#include "Timer.h"
class PPU
{
public:
	int SCREEN_SIZE = 160 * 144 * 4;



	PPU(Timer * timer);
	~PPU();

	void reset();
	void step();

private:
	Timer * timer;
	int mode = 0;
	long modeClock = 0;
	int line = 0;


};

