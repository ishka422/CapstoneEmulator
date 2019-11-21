#include "Timer.h"



Timer::Timer(mmu* memory)
{
	this->memory = memory;
	memory->connectTimer(this);
	now = clock();
	
	
}

void Timer::connectCPU(CPU * cpu)
{
	this->cpu = cpu;
}


void Timer::resetCycles()
{
	mCycles = 0;
}

void Timer::setCycles(uint8_t opcode)
{
	mCycles += machineCycles[opcode];
}

void Timer::addCBCycles(uint8_t opcode)
{
	mCycles += CBMachineCycles[opcode];
}

void Timer::update()
{
	now = clock();

}

bool Timer::needScreenRefresh()
{
	clock_t diffTime = clock();
	double elapsed = (double)(diffTime - now) * 1000000.0 / CLOCKS_PER_SEC;
	if (elapsed > 1667.0) {
		now = clock();
		return true;
	}
	else {
		return false;
	}

}

void Timer::updateTimers()
{
	doDivideRegister();

	if (isClockEnabled()) {
		tCycles -= mCycles;
		
		if (tCycles <= 0) {
			setClockFreq();

			if (memory->readByte(TIMA) == 255) {
				memory->writeByte(TIMA, memory->readByte(TMA));
				cpu->requestInterupt(2);
			}
			else {
				memory->writeByte(TIMA, memory->readByte(TIMA) + 1);
			}
		}
	}
}

void Timer::doDivideRegister()
{
	divideRegister += mCycles;
	if (divideRegister >= 255) {
		divideRegister = 0;
		memory->incDivider();
	}
}

bool Timer::isClockEnabled()
{
	return (memory->readByte(TAC) >> 2) == 1;
}


Timer::~Timer()
{
}
