#pragma once

#include "CPU.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#define MEMORY_REGION 0x8000
#define TILE_SIZE 16
#define OFFSET 128
#define SCREEN_HEIGHT 160
#define SCREEN_WIDTH 144
using namespace cv;
class PPU
{
public:
	
	Mat screen;
	//int SCREEN_SIZE = 160 * 144 * 4;

	MMU* memory;
	CPU* cpu;
	uint8_t lcdControl;


	PPU(CPU* cpu, MMU* memory);
	~PPU();

	void reset();
	void step();
	void updateGraphics();

	uchar getPallet(uint16_t address, int intensity);
	void doTiles();
	void doSprites();
	void setLCDStatus();
	bool LCDEnabled();
	void drawLine();
	Mat showScreen();


private:
	int mode = 0;
	long modeClock = 0;
	int line = 0;
	int lineCounter = 456;


};

