#include <iostream>
#include <stdio.h>
#include <fstream>
#include "MMU.h"
#include "CPU.h"
#include "PPU.h"
#include <opencv2/core/core.hpp>
#include <SDL2/SDL.h>
#include "main.h"

using namespace std;

typedef unsigned char BYTE;

const int MAXCYCLES = 69905;
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
		case SDLK_LEFT: key = 1; cout << "yes" << endl; break;
		case SDLK_UP: key = 2; break;
		case SDLK_DOWN: key = 3; break;
		}
		if (key != -1)
		{
			memory -> setKeyPressed(key);
		}
	}
	//If a key was released
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
		SDL_Event event;
		SDL_Window *window{ SDL_CreateWindow("window", 0,0,0,0,0) };
		Mat screen;
		/*screen.create(160, 144, CV_8UC1);*/
		bool logOPs = false;

		ifstream gameROM;
		gameROM.open(argv[1], ios::in | ios::binary | ios::ate);
		int size = gameROM.tellg();

		uint8_t *cartridgeBlock = new uint8_t[0x20000];
		gameROM.seekg(0, ios::beg);
		gameROM.read((char*)cartridgeBlock, size);

		int cyclesThisUpdate = 0;
		
		memory = new MMU(cartridgeBlock);
		memory->setMBCRule(cartridgeBlock[0x147]);
		processor = new CPU(memory);
		ppu = new PPU(processor, memory);

		bool calledOpcodes[0x100];
		memset(calledOpcodes, false, sizeof(calledOpcodes));
		int key = 0;
		while (true) {
			while (SDL_PollEvent(&event)) {
				handleInput(event);
			}
			cyclesThisUpdate = 0;
			processor->resetCycles();
			if (processor->needScreenRefresh()) {
				while (cyclesThisUpdate < MAXCYCLES) {
					/*if (!calledOpcodes[memory->readByte(processor->PC->getValue())]) {
						calledOpcodes[memory->readByte(processor->PC->getValue())] = true;
						cout << std::hex << (int)memory->readByte(processor->PC->getValue()) << "\t"<< std::hex << (int)processor->PC->getValue()<< endl;
					}*/
					processor->execute();
					/*if (processor->masterInterrupt) {
						cout << std::hex << (int)processor->getOpcode() << "\t" << std::hex << (int)processor->PC->getValue()<< endl; 
					}*/
					cyclesThisUpdate += processor->cycles;
					processor->updateTimers();
					ppu->updateGraphics();
					processor->handleInterrupts();
					
					//cout << std::hex<< (int)processor->PC->getValue() << endl;
				}

				ppu->showScreen().copyTo(screen);
				ppu->drawBackground();
				imshow("GBE", screen);
				key = waitKey(1);
			}
				
		}
		cout << "called standard opcodes" << endl;
		for (size_t i = 0; i < 0x100; i++)
		{
			if (calledOpcodes[i])
				cout << std::hex << i << endl;
		}
		

		
		//char contents[static_cast<size_t>(end - start)];

		//contents.resize(static_cast<size_t>(end - start));
		
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