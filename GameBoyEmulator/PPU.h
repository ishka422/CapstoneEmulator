#pragma once

#include "CPU.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <filesystem>

#define VRAM_START 0x8000
#define TILE_SIZE 16
#define OFFSET 128
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define LCD_CONTROL 0xFF40
#define LCD_STATUS 0xFF41
#define SCROLL_Y 0xFF42
#define SCROLL_X 0xFF43
#define SCANLINE 0xFF44
#define WINDOW_Y 0xFF4A
#define WINDOW_X 0xFF4B



using namespace cv;


class PPU
{
public:
	

	Mat screen;

	MMU* memory;
	CPU* cpu;
	uint8_t lcdControl;
	std::string gameName;
	int imNum;
	std::vector<Mat> seenTiles;
	vector<Mat> imageList;
	vector<Mat> replaceList;
	vector<bool> tileLoaded;
	vector<int> replacedLocation;
	Mat imageReplace[0x180];

	PPU(CPU* cpu, MMU* memory, std::string gameName);
	~PPU();

	void reset();
	void step();
	void updateGraphics();

	uchar getPallet(uint16_t address, int intensity);
	void drawTilesOnLine();
	void drawSpritesOnLine();
	void setLCDStatus();
	bool LCDEnabled();
	void drawLine();
	Mat showScreen();
	void loadReplacements();
	void drawBackground();
	void saveSprites();
	void handleReplacement(); 


private:
	int mode = 0;
	long modeClock = 0;
	int line = 0;
	int lineCounter = 456;


};

