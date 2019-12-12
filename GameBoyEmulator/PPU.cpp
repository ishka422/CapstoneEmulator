#pragma once
#include "PPU.h"
#include <bitset>

PPU::PPU(CPU* cpu, MMU* memory, std::string gameName)
{
	this->cpu = cpu;
	this->memory = memory;
	this->gameName = gameName;
	lcdControl = 0;
	Mat temp(SCREEN_HEIGHT * 2, SCREEN_WIDTH * 2, CV_8UC1);
	temp = Scalar::all(255);
	screen.create(SCREEN_HEIGHT * 2, SCREEN_WIDTH * 2, CV_8UC1);
	temp.copyTo(screen);
	imNum = 0;
	lineCounter = 0;
}

PPU::~PPU()
{
}

void PPU::updateGraphics()
{
	lcdControl = memory->readByte(LCD_CONTROL);
	setLCDStatus();

	if (LCDEnabled()) {
		lineCounter -= cpu->cycles;
	}
	else {

		return;
	}
	if (lineCounter <= 0) {
		memory->incLY();
		uint8_t currentLine = memory->readByte(SCANLINE);
		lineCounter = 456;

		if (currentLine == 144) {
			memory->requestInterupt(0);
		}
		else if (currentLine > 153) {
			memory->writeByte(SCANLINE, 0);
		}
		else if (currentLine < 144) {
			

			drawLine();
		}
	}
}

void PPU::drawTilesOnLine()
{
	lcdControl = memory->readByte(LCD_CONTROL);

	if (lcdControl & 1) {
		int scanline = memory->readByte(SCANLINE);
		uint16_t tileData = 0;
		uint16_t backgroundMemory = 0;
		bool sign = false;

		bool windowEnable = false;
		//bit 5 dictates whether the game is using a window
		if (((lcdControl >> 5) & 1)) {
			windowEnable = ((memory->readByte(WINDOW_Y) - 7) <= scanline);
		}
		//which memory location is the tile located in?
		if (((lcdControl >> 4) & 1)) {
			tileData = VRAM_START;
		}
		else {
			tileData = VRAM_START + 0x800;
			sign = true;
		}

		if (!windowEnable) {
			if (((lcdControl >> 3) & 1)) {
				backgroundMemory = 0x9C00;
			}
			else {
				backgroundMemory = 0x9800;
			}
		}
		else {
			if (((lcdControl >> 6) & 1))
				backgroundMemory = 0x9C00;
			else
				backgroundMemory = 0x9800;
		}
		uint8_t ypos = 0;

		if (!windowEnable) {
			ypos = memory->readByte(SCROLL_Y) + scanline;
		}
		else {
			ypos = scanline - (memory->readByte(WINDOW_Y) - 7);
		}
		uint16_t tileRow = (((uint8_t)(ypos / 8)) * 32);

		for (int pixel = 0; pixel < SCREEN_WIDTH; pixel++) {
			uint8_t xpos = pixel + memory->readByte(SCROLL_X);
			if (windowEnable) {
				if (pixel >= memory->readByte(WINDOW_X)) {
					xpos = pixel - memory->readByte(WINDOW_X);
				}
			}

			uint16_t tileCol = (xpos / 8);
			int16_t tileNum;
			uint16_t tileAddress = backgroundMemory + tileRow + tileCol;
			if (!sign) {
				tileNum = memory->readByte(tileAddress);
			}
			else {
				tileNum = (char)memory->readByte(tileAddress);
			}

			uint16_t tileOffset = 0;
			if (sign) {
				tileOffset = ((tileNum + 128) * 16);
			}
			else {
				tileOffset = tileNum * 16;
			}

			uint8_t line = ypos % 8;
			line *= 2;

			if (memory->replacedTile[tileOffset >> 4]) {
				if (imageReplace[tileOffset >> 4].cols == 16) {
					int intensityLoc = xpos % 8;
					intensityLoc *= 2;
					uchar intenArr[4];
					intenArr[0] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line, intensityLoc);
					intenArr[1] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line, intensityLoc + 1);
					intenArr[2] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line + 1, intensityLoc);
					intenArr[3] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line + 1, intensityLoc + 1);

					uchar colArr[4];

					colArr[0] = getPallet(0xFF47, intenArr[0] / 85);
					colArr[1] = getPallet(0xFF47, intenArr[1] / 85);
					colArr[2] = getPallet(0xFF47, intenArr[2] / 85);
					colArr[3] = getPallet(0xFF47, intenArr[3] / 85);

					if ((scanline >= 0) && (scanline <= 143) && (pixel >= 0) && (pixel <= 159)) {
						screen.at<uchar>(scanline * 2, pixel * 2) = colArr[0];
						screen.at<uchar>(scanline * 2, pixel * 2 + 1) = colArr[1];
						screen.at<uchar>(scanline * 2 + 1, pixel * 2) = colArr[2];
						screen.at<uchar>(scanline * 2 + 1, pixel * 2 + 1) = colArr[3];
					}
				}
				else if(imageReplace[tileOffset >> 4].cols == 8){
					line /= 2;

					int intensityLoc = xpos % 8;
					uchar intensity = 255 - imageReplace[tileOffset >> 4].at<uchar>(line, intensityLoc);
					uchar color = getPallet(0xFF47, intensity / 85);

					if ((scanline >= 0) && (scanline <= 143) && (pixel >= 0) && (pixel <= 159)) {
						screen.at<uchar>(scanline * 2, pixel * 2) = color;
						screen.at<uchar>(scanline * 2, pixel * 2 + 1) = color;
						screen.at<uchar>(scanline * 2 + 1, pixel * 2) = color;
						screen.at<uchar>(scanline * 2 + 1, pixel * 2 + 1) = color;
					}

				}
			}
			else {
				uint16_t tileLocation = tileData + tileOffset;

				uint8_t data1 = memory->readByte(tileLocation + line);
				uint8_t data2 = memory->readByte(tileLocation + line + 1);

				int intensityBit = 7 - (xpos % 8);

				int intensity = (data2 >> intensityBit) & 1;
				intensity <<= 1;
				intensity |= (data1 >> intensityBit) & 1;

				uchar col = getPallet(0xFF47, intensity);
				if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159)) {
					continue;
				}
				screen.at<uchar>(scanline * 2, pixel * 2) = saturate_cast<uchar>(col);
				screen.at<uchar>(scanline * 2, pixel * 2 + 1) = saturate_cast<uchar>(col);
				screen.at<uchar>(scanline * 2 + 1, pixel * 2) = saturate_cast<uchar>(col);
				screen.at<uchar>(scanline * 2 + 1, pixel * 2 + 1) = saturate_cast<uchar>(col);
			}
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

void PPU::drawSpritesOnLine()
{
	lcdControl = memory->readByte(LCD_CONTROL);
	bool use2Tiles = ((lcdControl >> 2) & 1);

	for (int spriteIndex = 0; spriteIndex < 160; spriteIndex += 4)
	{
		uint8_t yPos = memory->readByte(0xFE00 + spriteIndex) - 16;
		uint8_t xPos = memory->readByte(0xFE00 + spriteIndex + 1) - 8;
		uint8_t tileLocation = memory->readByte(0xFE00 + spriteIndex + 2);
		uint8_t attributes = memory->readByte(0xFE00 + spriteIndex + 3);

		bool flipOnY = ((attributes >> 6) & 1);
		bool flipOnX = ((attributes >> 5) & 1);

		int scanline = memory->readByte(SCANLINE);

		int ysize = 8;
		if (use2Tiles)
			ysize = 16;

		if ((scanline >= yPos) && (scanline < (yPos + ysize)))
		{
			int line = scanline - yPos;

			if (flipOnY)
				line = ysize - line;
			
			int tileOffset = tileLocation * 16;
			if (memory->replacedTile[tileOffset >> 4]) {
				if (imageReplace[tileOffset >> 4].cols == 16) {
					line *= 2;
					for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
					{
						int colorBit = 7 - tilePixel;

						if (flipOnX)
						{
							colorBit = tilePixel;
						}

						int intensityLoc = colorBit;
						intensityLoc *= 2;
						uint16_t pallet = 0;

						if ((attributes >> 4) & 1)
							pallet = 0xFF49;
						else
							pallet = 0xFF48;


						uchar intenArr[4];
						intenArr[0] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line, intensityLoc);
						intenArr[1] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line, intensityLoc + 1);
						intenArr[2] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line + 1, intensityLoc);
						intenArr[3] = 255 - imageReplace[tileOffset >> 4].at<uchar>(line + 1, intensityLoc + 1);

						uchar colArr[4];

						colArr[0] = getPallet(pallet, intenArr[0] / 85);
						colArr[1] = getPallet(pallet, intenArr[1] / 85);
						colArr[2] = getPallet(pallet, intenArr[2] / 85);
						colArr[3] = getPallet(pallet, intenArr[3] / 85);

						int xPix = 7 - tilePixel;
						int pixel = xPos + xPix;

						if ((scanline >= 0) && (scanline <= 143) && (pixel >= 0) && (pixel <= 159)) {
							if(colArr[0] !=255)
							screen.at<uchar>(scanline * 2, pixel * 2) = colArr[0];
							if (colArr[1] != 255)
							screen.at<uchar>(scanline * 2, pixel * 2 + 1) = colArr[1];
							if (colArr[2] != 255)
							screen.at<uchar>(scanline * 2 + 1, pixel * 2) = colArr[2];
							if (colArr[3] != 255)
							screen.at<uchar>(scanline * 2 + 1, pixel * 2 + 1) = colArr[3];
						}



					}


				}
				else if(imageReplace[tileOffset >> 4].cols == 8){
					for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
					{
						int colorBit = 7 - tilePixel;

						if (flipOnX)
						{
							colorBit = tilePixel;
						}
						int intensityLoc = colorBit;
						uint16_t pallet = 0;

						if ((attributes >> 4) & 1)
							pallet = 0xFF49;
						else
							pallet = 0xFF48;

						uchar intensity = 255 - imageReplace[tileOffset >> 4].at<uchar>(line, intensityLoc);
						uchar color = getPallet(pallet, intensity / 85);
						if (color == 255)
							continue;
						int xPix = 7 - tilePixel;
						int pixel = xPos + xPix;
						if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159)) {
							continue;
						}

						screen.at<uchar>(scanline * 2, pixel * 2) = color;
						screen.at<uchar>(scanline * 2, pixel * 2 + 1) = color;
						screen.at<uchar>(scanline * 2 + 1, pixel * 2) = color;
						screen.at<uchar>(scanline * 2 + 1, pixel * 2 + 1) = color;

					}


				}
			}
			else {
				line *= 2;
				uint16_t dataAddress = (VRAM_START + (tileLocation * 16)) + line;
				uint8_t data1 = memory->readByte(dataAddress);
				uint8_t data2 = memory->readByte(dataAddress + 1);
				
				for (int tilePixel = 7; tilePixel >= 0; tilePixel--)
				{
					int colorBit = tilePixel;

					if (flipOnX)
					{
						colorBit = 7 - colorBit;
					}

					int intensity = (data2 >> colorBit) & 1;
					intensity <<= 1;
					intensity |= (data1 >> colorBit) & 1;

					uint16_t pallet = 0;

					if ((attributes >> 4) & 1)
						pallet = 0xFF49;
					else
						pallet = 0xFF48;

					uchar value = getPallet(pallet, intensity);

					// white is transparent for sprites.
					if (value == 255)
						continue;

					int xPix = 7 - tilePixel;
					int pixel = xPos + xPix;

					if ((scanline < 0) || (scanline > 143) || (pixel < 0) || (pixel > 159)) {
						continue;
					}

					screen.at<uchar>(scanline * 2, pixel * 2) = value;
					screen.at<uchar>(scanline * 2, pixel * 2 + 1) = value;
					screen.at<uchar>(scanline * 2 + 1, pixel * 2) = value;
					screen.at<uchar>(scanline * 2 + 1, pixel * 2 + 1) = value;
				}
			}
		}
	}
}

void PPU::setLCDStatus()
{
	uint8_t status = memory->readByte(LCD_STATUS);
	if (!LCDEnabled()) {
		lineCounter = 456;
		memory->writeByte(SCANLINE, 0);
		status &= (~(~0U << 6) << 2);
		status |= 1;
		memory->writeByte(LCD_STATUS, status);
		return;
	}
	uint8_t currentLine = memory->readByte(SCANLINE);
	uint8_t currMode = status & (~(~0U << 2));

	uint8_t mode = 0;
	bool requestInterupt = false;

	if (currentLine >= 144) {
		mode = 1;
		status |= 1;
		status &= ~(1 << 1);
		requestInterupt = ((status >> 4) & 1);
	}
	else {
		if (lineCounter > 376) { // 456 - 377
			mode = 2;
			status |= (1 << 1);
			status &= ~(1 << 0);
			requestInterupt = ((status >> 5) & 1);
		}
		else if (lineCounter >= 204) { // 376 - 204
			mode = 3;
			status |= (1 << 1);
			status |= 1;
		}
		else {
			mode = 0;
			status &= ~(1 << 1);
			status &= ~1;
			requestInterupt = ((status >> 3) & 1);
		}

		if (requestInterupt && (mode != currMode)) {
			memory->requestInterupt(1);
		}
		if (memory->readByte(SCANLINE) == memory->readByte(0xFF45)) {
			status |= (1 << 2);
			if (((status >> 6) & 1) == 1) {
				memory->requestInterupt(1);
			}
		}
		else {
			status &= ~(1 << 2);
		}
		memory->writeByte(LCD_STATUS, status);
	}
}

bool PPU::LCDEnabled()
{
	return ((memory->readByte(LCD_CONTROL) >> 7) & 1) == 1;
}

void PPU::drawLine(){
	uint8_t control = memory->readByte(LCD_CONTROL);
	if ((control & 1) == 1) {
		drawTilesOnLine();
	}
	if (((control >> 1) & 1) == 1) {
		drawSpritesOnLine();
	}
}

Mat PPU::showScreen()
{
	return screen;
}

void PPU::loadReplacements()
{
	vector<string> fileList;
	string path = gameName + "-replace";
	//string path = gameName + "-replace";

	for (const auto & entry : std::filesystem::directory_iterator(path)) {
		fileList.push_back(entry.path().string());
		replacedLocation.push_back(-1);
	}

	for (int i = 0; i < fileList.size(); i += 2)
	{
		imageList.push_back(imread(fileList[i], IMREAD_GRAYSCALE));
		replaceList.push_back(imread(fileList[i + 1], IMREAD_GRAYSCALE));
		tileLoaded.push_back(false);
	}
	for (int i = 0; i < replaceList.size(); i++) {
		for (int h = 0; h < replaceList[i].rows; h++) {
			for (int j = 0; j < replaceList[i].cols; j++)
			{
				uchar pixel = replaceList[i].at<uchar>(h, j);
				if (pixel < 255 && pixel > 170) {
					if ((255 - pixel) > (pixel - 170)) {
						replaceList[i].at<uchar>(h, j) = 170;
					}
					else {
						replaceList[i].at<uchar>(h, j) = 255;
					}
				}
				if (pixel < 170 && pixel > 85) {
					if ((170 - pixel) > (pixel - 85)) {
						replaceList[i].at<uchar>(h, j) = 85;
					}
					else {
						replaceList[i].at<uchar>(h, j) = 170;
					}
				}
				if (pixel < 85 && pixel > 0) {
					if ((85 - pixel) > (pixel)) {
						replaceList[i].at<uchar>(h, j) = 0;
					}
					else {
						replaceList[i].at<uchar>(h, j) = 85;
					}
				}
			}
		}
	}
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
					uint8_t data1 = memory->readByte(VRAM_START + tileIndex);
					uint8_t data2 = memory->readByte(VRAM_START + tileIndex + 1);

					uint8_t value = (data2 >> b) & 1;
					value <<= 1;
					value |= ((data1 >> b) & 1);

					uint8_t pixel = (255 - (value*(255 / 3)));

					uchar col = getPallet(0xFF47, value);

					bg.at<uchar>(a + (8 * i), (8 * j) + (7 - b)) = pixel;
				}
				tileIndex += 2;
			}
		}
	}

	imshow("Tiles", bg);
}

void PPU::saveSprites()
{
	if (!memory->writingToVRAM && memory->pendingChanges) {
		Mat tile(8, 8, CV_8UC1);
		for (int i = 0; i < 0x180; i++) {
			if (memory->changedTile[i]) {
				int tileIndex = 0;
				for (int x = 0; x < 8; x++) {
					uint8_t data1 = memory->readByte(VRAM_START + (i << 4) + tileIndex);
					uint8_t data2 = memory->readByte(VRAM_START + (i << 4) + tileIndex + 1);
					for (int y = 0; y < 8; y++) {
						uint8_t value = (data2 >> y) & 1;
						value <<= 1;
						value |= ((data1 >> y) & 1);

						uint8_t pixel = (255 - (value*(255 / 3)));

						uchar col = getPallet(0xFF47, value);
						tile.at<uchar>(x, (7 - y)) = col;
					}
					tileIndex += 2;
				}
				bool exists = false;
				Mat dst;
				for (int l = 0; l < seenTiles.size(); l++) {
					cv::bitwise_xor(seenTiles[l], tile, dst);
					if (cv::sum(dst) == Scalar(0)) {
						exists = true;
						break;
					}
				}

				if (!exists && sum(tile) != Scalar(255 * 8 * 8) && sum(tile) != Scalar(0)) {
					Mat output(64, 64, CV_8UC1);
					for (int x = 0; x < 64; x += 8) {
						for (int y = 0; y < 64; y += 8) {
							for (int i = 0; i < 8; i++) {
								for (int j = 0; j < 8; j++) {
									output.at<uchar>(x + i, y + j) = tile.at<uchar>(x / 8, y / 8);
								}
							}
						}
					}
					seenTiles.push_back(tile.clone());
					imwrite("./" + gameName + "/" + to_string(imNum) + ".bmp", output);
					imwrite("./" + gameName + "/" + to_string(imNum) + "a.bmp", tile);
					imNum++;
				}

				memory->changedTile[i] = false;
			}
		}
		memory->pendingChanges = false;
	}
}

void PPU::handleReplacement()
{
	if (!memory->writingToVRAM && memory->pendingChanges) {
		for (int i = 0; i < tileLoaded.size(); i++) {
			if (tileLoaded[i] && replacedLocation[i] != -1) {
				Mat tile(8, 8, CV_8UC1);
				int tileIndex = 0;
				for (int x = 0; x < 8; x++) {
					uint8_t data1 = memory->readByte(VRAM_START + (replacedLocation[i] << 4) + tileIndex);
					uint8_t data2 = memory->readByte(VRAM_START + (replacedLocation[i] << 4) + tileIndex + 1);
					for (int y = 0; y < 8; y++) {
						uint8_t value = (data2 >> y) & 1;
						value <<= 1;
						value |= ((data1 >> y) & 1);

						uint8_t pixel = (255 - (value*(255 / 3)));

						uchar col = getPallet(0xFF47, value);
						tile.at<uchar>(x, (7 - y)) = col;
					}
					tileIndex += 2;
				}
				Mat dst;
				cv::bitwise_xor(imageList[i], tile, dst);
				if (cv::sum(dst) != Scalar(0)) {
					tileLoaded[i] = false;
				}
			}
			if (!tileLoaded[i]) {
				Mat tile(8, 8, CV_8UC1);
				for (int j = 0; j < 0x180; j++) {
					if (memory->changedTile[j] && !memory->replacedTile[j]) {
						int tileIndex = 0;
						for (int x = 0; x < 8; x++) {
							uint8_t data1 = memory->readByte(VRAM_START + (j << 4) + tileIndex);
							uint8_t data2 = memory->readByte(VRAM_START + (j << 4) + tileIndex + 1);
							for (int y = 0; y < 8; y++) {
								uint8_t value = (data2 >> y) & 1;
								value <<= 1;
								value |= ((data1 >> y) & 1);

								uint8_t pixel = (255 - (value*(255 / 3)));

								uchar col = getPallet(0xFF47, value);
								tile.at<uchar>(x, (7 - y)) = col;
							}
							tileIndex += 2;
						}
						Mat dst;
						cv::bitwise_xor(imageList[i], tile, dst);
						if (cv::sum(dst) == Scalar(0)) {
							tileLoaded[i] = true;
							replacedLocation[i] = j;
							imageReplace[j] = replaceList[i].clone();
							memory->replacedTile[j] = true;
						}
					}
				}
			}
		}
		memset(memory->changedTile, false, sizeof(memory->changedTile));
		memory->pendingChanges = false;
	}
}