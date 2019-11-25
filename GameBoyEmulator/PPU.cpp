#pragma once
#include "PPU.h"



PPU::PPU(CPU* cpu, MMU* memory)
{
	this->cpu = cpu;
	this->memory = memory;
	screen.create(SCREEN_HEIGHT, SCREEN_WIDTH, CV_8UC1);
}


PPU::~PPU()
{
}

void PPU::updateGraphics()
{
	
	
	lcdControl = memory->readByte(0xFF40);
	setLCDStatus();
	
	if (LCDEnabled()) {
		lineCounter -= cpu->mCycles;
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

void PPU::doTiles()
{
	unsigned short tileData = 0;
	unsigned short backgroundMemory = 0;
	bool sign = false;

	uint8_t scrollY = memory->readByte(0xFF42);
	uint8_t scrollX = memory->readByte(0xFF43);
	uint8_t windowX = memory->readByte(0xFF4A);
	uint8_t windowY = memory->readByte(0xFF4B) - 7;

	bool windowEnable = false;
	//bit 5 dictates weather the game is using a window
	if (((lcdControl >> 5) & 1) == 1) {
		windowEnable = (windowY <= memory->readByte(0xFF44));
	}
	//which memory location is the tile located in?
	if (((lcdControl >> 4) & 1) == 1) {
		tileData = 0x8000;
	}
	else {
		tileData = 0x8800;
		sign = true;
	}

	if (!windowEnable) {
		
		if (((lcdControl >> 3) & 1) == 1) {
			backgroundMemory = 0x9C00;
		}
		else {
			backgroundMemory = 0x9800;
		}
	}
	uint8_t ypos = 0;
	
	if (!windowEnable) {
		ypos = scrollY + memory->readByte(0xFF44);
	}
	else {
		ypos = memory->readByte(0xFF44) - windowY;
	}
	unsigned short tileRow = (((uint8_t)(ypos / 8)) * 32);

	for (int i = 0; i < 160; i++){
		uint8_t xpos = i + scrollX;
		if (windowEnable) {
			if (i >= windowX) {
				xpos = i - windowX;
			}
		}
		unsigned short tileCol = (xpos / 8);
		signed short tileNum;

		unsigned short tileAddress = backgroundMemory + tileRow + tileCol;

		if (!sign) {
			tileNum = memory->readByte(tileAddress);
		}
		else {
			tileNum = (char)memory->readByte(tileAddress);
		}

		short tileLocation = tileData;

		if (sign) {
			tileLocation += ((tileNum + 128) * 16);
		}
		else {
			tileLocation += tileNum * 16;
		}
		uint8_t line = ypos % 8;
		line *= 2;
		uint8_t data1 = memory->readByte(tileLocation + line);
		uint8_t data2 = memory->readByte(tileLocation + line+1);
		int intensityBit = xpos % 8;
		intensityBit -= 7;
		intensityBit *= -1;

		int intensity = (data2 >> intensityBit) & 1;
		intensity <<= 1;
		intensity |= (data1 >> intensityBit) & 1;

		
		uchar pixel = getPallet(0xFF47, intensity);
		
		int x = memory->readByte(0xFF44);
		if ((x < 0) || (x > 143) || (i < 0) || (i > 159)) {
			continue;
		}
		screen.at<uchar>(i, x) = pixel;

	}

}

uchar PPU::getPallet(uint16_t address, int intensity) {
	uint8_t pallet = memory->readByte(address);
	int hi = 0;
	int lo = 0;
	switch (intensity) {
		case 0: hi = 1; lo = 0; break;
		case 1: hi = 3; lo = 2; break;
		case 2: hi = 5; lo = 4; break;
		case 3: hi = 7; lo = 6; break;
	}
	int value = ((pallet >> hi) & 1) << 1;
	value += ((pallet >> lo) & 1);
	uchar pixel = (255 - (value*(255 / 3)));
	return pixel;
}

void PPU::doSprites()
{
	
	bool use8x16 = (lcdControl >> 2) & 1 == 1;
	

	for (int sprite = 0; sprite < 40; sprite++)
	{
		
		uint8_t index = sprite * 4;
		uint8_t yPos = memory->readByte(0xFE00 + index) - 16;
		uint8_t xPos = memory->readByte(0xFE00 + index + 1) - 8;
		uint8_t tileLocation = memory->readByte(0xFE00 + index + 2);
		uint8_t attributes = memory->readByte(0xFE00 + index + 3);

		bool yFlip = (attributes >> 6)&1 == 1;
		bool xFlip = (attributes >> 5) & 1 == 1;

		int scanline = memory->readByte(0xFF44);

		int ysize = 8;
		if (use8x16)
			ysize = 16;

		
		if ((scanline >= yPos) && (scanline < (yPos + ysize)))
		{
			int line = scanline - yPos;

			
			if (yFlip)
			{
				line -= ysize;
				line *= -1;
			}

			line *= 2; 
			unsigned short dataAddress = (0x8000 + (tileLocation * 16)) + line;
			uint8_t data1 = memory->readByte(dataAddress);
			uint8_t data2 = memory->readByte(dataAddress + 1);

			
			for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
			{
				int colorBit = tilePixel;
				
				if (xFlip)
				{
					colorBit -= 7;
					colorBit *= -1;
				}

				
				int colorNum = (data2 >> colorBit)&1;
				colorNum <<= 1;
				colorNum |= (data1 >> colorBit) & 1;

				unsigned short colorAddress =((attributes >> 4)&1)==1 ? 0xFF49 : 0xFF48;
				uchar value = getPallet(colorAddress, colorNum);

				// white is transparent for sprites.
				if (value == 255)
					continue;

				int xPix = 0 - tilePixel;
				xPix += 7;

				int pixel = xPos + xPix;

				// sanity check
				if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159))
				{
					continue;
				}
				screen.at<uchar>(scanline, pixel) = value;
				
			}
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
	uint8_t currentLine = memory->readByte(0xFF44);
	uint8_t currMode = status & (~(~0U << 2));

	uint8_t mode = 0;
	bool requestInterupt = false;

	if (currentLine >= 144) {
		mode = 1;
		status |= 1;
		status &= ~(1 << 1);
		requestInterupt = ((status >> 4)&1) == 1;
	}
	else {
		int mode2Bounds = 456 - 80;
		int mode3Bounds = mode2Bounds - 172;

		if(lineCounter > mode2Bounds){
			mode = 2;
			status |= (1 << 1);
			status &= ~(1<< 0);
			requestInterupt = ((status >> 5) & 1) == 1;
		}
		else if (lineCounter >= mode3Bounds){
			mode = 3;
			status |= (1 << 1);
			status |= 1;
		}
		else {
			mode = 0;
			status &= ~(1 << 1);
			status &= ~1;
			requestInterupt = ((status >> 3) & 1) == 1;
		}

		if (requestInterupt && (mode != currMode)) {
			cpu->requestInterupt(1);
		}
		if (memory->readByte(0xFF44) == memory->readByte(0xFF45)) {
			status |= (1 << 2);
			if ((status >> 6) & 1 == 1) {
				cpu->requestInterupt(1);
			}
			else {
				status &= ~(1 << 2);
			}
			memory->writeByte(0xFF41, status);
		}

	}

}

bool PPU::LCDEnabled()
{
	return ((memory->readByte(0xFF40) >> 7) & 1) == 1;
}

void PPU::drawLine()
{
	uint8_t control = memory->readByte(0xFF40);
	if (control & 1 == 1) {
		doTiles();
	}
	if ((control >> 1) & 1 == 1) {
		doSprites();
	}
}

void PPU::showScreen()
{
	imshow("GBE", screen);
}
