#include "PPU.h"



PPU::PPU(Timer * timer)
{
}


PPU::~PPU()
{
}

void PPU::updateGraphics()
{
	setLCDStatus();
	
	if (LCDEnabled()) {
		lineCounter -= timer->mCycles;
	}
	else {
		return;
	}
	if (lineCounter <= 0) {
		memory->incLY();
		uint8_t currentLine = memory->readByte(0xFF44);
		lineCounter = 456;

		if (currentLine = 144) {
			cpu->requestInterupt(0);
		}
		else if (currentLine > 153){
			memory->writeByte(0xFF44, 0);
		}
		else if (currentLine < 144) {
			drawLine();
		}
	}
}

void PPU::setLCDStatus()
{
	uint8_t status = memory->readByte(0xFF41);
	if (!LCDEnabled()) {
		lineCounter = 456;
		memory->writeByte(0xFF44, 0);
		status &= (~(~0U << 6) << 2);
		status |= 1;
		memory->writeByte(0xFF41, status);
		return;
	}
	uint8_t lineNum = memory->readByte(0xFF44);
	uint8_t currMode = status & (~(~0U << 2));

	uint8_t mode = 0;
	bool requestInterupt = false;

	if (lineNum >= 144) {
		mode = 1;
		status |= 1;
		status &= (~(~0U << 1) << 1);
		requestInterupt = ((status >> 4)&1) == 1;
	}

}
