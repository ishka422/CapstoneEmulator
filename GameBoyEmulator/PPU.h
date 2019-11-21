#pragma once

#include "Timer.h"
class PPU
{
public:
	int SCREEN_SIZE = 160 * 144 * 4;

	mmu* memory;
	CPU* cpu;


	PPU(Timer * timer, CPU* cpu, mmu* memory);
	~PPU();

	void reset();
	void step();
	void updateGraphics();

	void setLCDStatus();
	bool LCDEnabled();
	void drawLine();


private:
	Timer * timer;
	int mode = 0;
	long modeClock = 0;
	int line = 0;
	int lineCounter = 456;


};

