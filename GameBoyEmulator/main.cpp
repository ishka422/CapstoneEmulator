#include <iostream>
#include <stdio.h>
#include <fstream>
#include "MMU.h"
#include "CPU.h"
#include "PPU.h"
#include <opencv2/core/core.hpp>
#include <SDL2/SDL.h>
#include "main.h"

#define MAXCYCLES 69905

using namespace std;

typedef unsigned char BYTE;


MMU* memory;
CPU* processor;
PPU* ppu;

struct HexCharStruct
{
	unsigned char c;
	HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
	return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
	return HexCharStruct(_c);
}


long getFileSize(FILE *file)

{
	
		long lCurPos, lEndPos;
	
		lCurPos = ftell(file);
	
		fseek(file, 0, 2);
	
		lEndPos = ftell(file);
	
		fseek(file, lCurPos, 0);
	
		return lEndPos;
	
}



void handleInput(SDL_Event& event) {
	if (event.type == SDL_KEYDOWN)
	{
		int key = -1;
		switch (event.key.keysym.sym)
		{
		case SDLK_a: key = 4; break;
		case SDLK_s: key = 5; break;
		case SDLK_RETURN: key = 7; break;
		case SDLK_SPACE: key = 6; break;
		case SDLK_RIGHT: key = 0; break;
		case SDLK_LEFT: key = 1; break;
		case SDLK_UP: key = 2; break;
		case SDLK_DOWN: key = 3; break;
		}
		if (key != -1)
		{
			memory -> setKeyPressed(key);
		}
	}
	else if (event.type == SDL_KEYUP)
	{
		int key = -1;
		switch (event.key.keysym.sym)
		{
		case SDLK_a: key = 4; break;
		case SDLK_s: key = 5; break;
		case SDLK_RETURN: key = 7; break;
		case SDLK_SPACE: key = 6; break;
		case SDLK_RIGHT: key = 0; break;
		case SDLK_LEFT: key = 1; break;
		case SDLK_UP: key = 2; break;
		case SDLK_DOWN: key = 3; break;
		}
		if (key != -1)
		{
			memory ->setKeyReleased(key);
		}
	}
}


int main(int argc, char *argv[]) {
	if (argc > 1) {
		
		Mat screen(Size(160 *2, 144*2), CV_8UC1);
		SDL_Event event;
		bool logOPs = false;

		ifstream gameROM;
		gameROM.open(argv[1], ios::in | ios::binary | ios::ate);
		int size = gameROM.tellg();

		uint8_t *cartridgeBlock = new uint8_t[0x20000];
		gameROM.seekg(0, ios::beg);
		gameROM.read((char*)cartridgeBlock, size);

		int cyclesThisUpdate = 0;
		
		std::string gameName("");

		for (int i = 0x0134; i < 0x144; i++) {
			if (cartridgeBlock[i] != 0x00) {
				gameName += (char)cartridgeBlock[i];
			}
		}

		memory = new MMU(cartridgeBlock);
		memory->setMBCRule(cartridgeBlock[0x147]);
		processor = new CPU(memory);
		ppu = new PPU(processor, memory, gameName);
		if (argc > 3) {
			if (*argv[3] == 'r')
				ppu->loadReplacements();
		}
		int key = 0;
		while (key != -1) {
			while (SDL_PollEvent(&event)) {
				handleInput(event);
			}
			cyclesThisUpdate = 0;
			processor->resetCycles();
			if (processor->needScreenRefresh()) {
				while (cyclesThisUpdate < MAXCYCLES) {
					processor->execute();
					cyclesThisUpdate += processor->cycles;
					processor->updateTimers();
					ppu->updateGraphics();
					processor->handleInterrupts();
				}
				ppu->showScreen().copyTo(screen);

				if (argc > 2) {
					if(*argv[2] == 'd')
						ppu->drawBackground();
					if (argc > 3) {
						if (*argv[3] == 's') {
							ppu->saveSprites();
						}else if(*argv[3] == 'r') {
							ppu->handleReplacement();
						}
					}
				}
				imshow(gameName, screen);
				key = waitKey(1);
			}
		}
		
		getchar();
		return 0;
	}
	else {
		cout << "Must enter file";
		cout << argv[0] << "\n";
		getchar();
		return 0;
	}


}