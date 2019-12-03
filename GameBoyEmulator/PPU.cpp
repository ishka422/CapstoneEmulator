#pragma once
#include "PPU.h"
#include <bitset>



PPU::PPU(CPU* cpu, MMU* memory)
{
	this->cpu = cpu;
	this->memory = memory;
	lcdControl = 0;
	Mat temp(SCREEN_WIDTH, SCREEN_HEIGHT, CV_8UC1);
	temp = Scalar::all(255);
	//screen.create(SCREEN_HEIGHT, SCREEN_WIDTH, CV_8UC1);
	temp.copyTo(screen);
	
}


PPU::~PPU()
{
}

void PPU::updateGraphics()
{
	lcdControl = memory->readByte(0xFF40);
	setLCDStatus();
	
	if (LCDEnabled()) {
		lineCounter -= cpu->cycles;
	}
	else {
		return;
	}
	if (lineCounter <= 0) {
		memory->incLY();
		uint8_t currentLine = memory->readByte(0xFF44);
		lineCounter = 456;

		if (currentLine == 144) {
			memory->requestInterupt(0);
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
	lcdControl = memory->readByte(0xFF40);
	if ((lcdControl & 1) == 1) {

		unsigned short tileData = 0;
		unsigned short backgroundMemory = 0;
		bool sign = false;

		uint8_t scrollY = memory->readByte(0xFF42);
		uint8_t scrollX = memory->readByte(0xFF43);
		uint8_t windowX = memory->readByte(0xFF4A);
		uint8_t windowY = memory->readByte(0xFF4B) - 7;

		bool windowEnable = false;
		//bit 5 dictates whether the game is using a window
		if (((lcdControl >> 5) & 1) == 1) {
			windowEnable = (windowY <= memory->readByte(0xFF44));
		}
		//which memory location is the tile located in?
		//cout << std::bitset<8>(lcdControl) << endl;
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
		else {
			if (((lcdControl >> 6) & 1) == 1)
				backgroundMemory = 0x9C00;
			else
				backgroundMemory = 0x9800;
		}
		uint8_t ypos = 0;

		if (!windowEnable) {
			ypos = scrollY + memory->readByte(0xFF44);
		}
		else {
			ypos = memory->readByte(0xFF44) - windowY;
		}
		unsigned short tileRow = (((uint8_t)(ypos / 8)) * 32);

		for (int pixel = 0; pixel < 160; pixel++) {
			uint8_t xpos = pixel + scrollX;
			if (windowEnable) {
				if (pixel >= windowX) {
					xpos = pixel - windowX;
				}
			}
			else {
				//cout << "no" << endl;
			}
			unsigned short tileCol = (xpos / 8);
			signed short tileNum;
			unsigned short tileAddress = backgroundMemory + tileRow + tileCol;
			//cout << std::hex << tileAddress << endl;
			if (!sign) {
				tileNum = memory->readByte(tileAddress);
			}
			else {
				tileNum = (char)memory->readByte(tileAddress);
			}

			unsigned short tileLocation = tileData;
			if (sign) {
				tileLocation += ((tileNum + 128) * 16);
			}
			else {
				tileLocation += tileNum * 16;
			}
			uint8_t line = ypos % 8;
			line *= 2;
			
			uint8_t data1 = memory->readByte(tileLocation + line);
			uint8_t data2 = memory->readByte(tileLocation + line + 1);

			//cout << std::hex << (int)(tileLocation + line) <<"\t"<< std::bitset<8> (data1)<< endl;

			int intensityBit = xpos % 8;
			intensityBit -= 7;
			intensityBit *= -1;

			int intensity = (data2 >> intensityBit) & 1;
			intensity <<= 1;
			intensity |= (data1 >> intensityBit) & 1;

			uchar col = getPallet(0xFF47, intensity);

			int finally = memory->readByte(0xFF44);
			if ((finally < 0) || (finally > 143) || (pixel < 0) || (pixel > 159)) {
				continue;
			}
			if (col != 255) {
				cout << "x: " << finally << "\ty: " << pixel << "\t" << (int)col << endl;
			}
			screen.at<uchar>(finally, pixel) = saturate_cast<uchar>(col);

		}
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
	lcdControl = memory->readByte(0xFF40);
	bool use8x16 = ((lcdControl >> 2) & 1) == 1;
	

	for (int sprite = 0; sprite < 40; sprite++)
	{
		
		uint8_t index = sprite * 4;
		uint8_t yPos = memory->readByte(0xFE00 + index) - 16;
		uint8_t xPos = memory->readByte(0xFE00 + index + 1) - 8;
		uint8_t tileLocation = memory->readByte(0xFE00 + index + 2);
		uint8_t attributes = memory->readByte(0xFE00 + index + 3);

		bool yFlip = ((attributes >> 6)&1 ) == 1;
		bool xFlip = ((attributes >> 5) & 1) == 1;

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
				screen.at<uchar>(pixel, scanline) = value;
				
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
			memory->requestInterupt(1);
		}
		if (memory->readByte(0xFF44) == memory->readByte(0xFF45)) {
			status |= (1 << 2);
			if (((status >> 6) & 1 )== 1) {
				memory->requestInterupt(1);
			}
			
		}
		else {
			status &= ~(1 << 2);
		}
		memory->writeByte(0xFF41, status);
		

	}

}

bool PPU::LCDEnabled()
{
	return ((memory->readByte(0xFF40) >> 7) & 1) == 1;
}

void PPU::drawLine()
{
	uint8_t control = memory->readByte(0xFF40);
	if ((control & 1 )== 1) {
		
		doTiles();
	}
	if (((control >> 1) & 1) == 1) {
		cout << "yes" << endl;
		doSprites();
	}
}

Mat PPU::showScreen()
{
	return screen;
}

void PPU::drawBackground()
{
	Mat bg(128, 192, CV_8UC1);
	int tileIndex = 0;
	for (int i = 0; i < 16; i++)
	{
		for (int j = 0; j < 24; j++) {
			for (int a = 0; a < 8; a++) {
				for (int b = 0; b < 8; b++) {
					/*if ((0x8000 + tileIndex) <0x8210 && (0x8000 + tileIndex) >= 0x8200) {
						bg.at<uchar>(a + (8 * i), (8 * j) + (7 - b)) = 0;
					}*/
					//else {
						uint8_t data1 = memory->readByte(0x8000 + tileIndex);
						uint8_t data2 = memory->readByte(0x8000 + tileIndex + 1);

						uint8_t value = (data2 >> b) & 1;
						value <<= 1;
						value |= ((data1 >> b) & 1);

						uint8_t pixel = (255 - (value*(255 / 3)));

						uchar col = getPallet(0xFF47, value);

						bg.at<uchar>(a + (8 * i), (8 * j) + (7 - b)) = col;
					//}
					

				}
				tileIndex += 2;
			}
			
		}
	}

	imshow("Tiles", bg);
}
