#include <iostream>
#include <stdio.h>
#include <fstream>
#include "MMU.h"
#include "CPU.h"
#include "PPU.h"
#include <opencv2/core/core.hpp>

using namespace std;

typedef unsigned char BYTE;

const int MAXCYCLES = 69905;

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


int main(int argc, char *argv[]) {
	if (argc > 1) {
		Mat screen;
		/*screen.create(160, 144, CV_8UC1);*/

		ifstream gameROM;
		gameROM.open(argv[1], ios::in | ios::binary | ios::ate);
		int size = gameROM.tellg();

		uint8_t *cartridgeBlock = new uint8_t[0x20000];
		gameROM.seekg(0, ios::beg);
		gameROM.read((char*)cartridgeBlock, size);

		int cyclesThisUpdate = 0;
		
		MMU* memory = new MMU(cartridgeBlock);
		memory->setMBCRule(cartridgeBlock[0x147]);
		CPU* processor = new CPU(memory);
		PPU* ppu = new PPU(processor, memory);

		int key = -1;
		while (key == -1) {
			cyclesThisUpdate = 0;
			processor->resetCycles();
			if (processor->needScreenRefresh()) {
				while (cyclesThisUpdate < MAXCYCLES) {
					processor->execute();
					cyclesThisUpdate += processor->cycles;
					ppu->updateGraphics();
					processor->handleInterrupts();
					
					//cout << std::hex<< (int)processor->PC->getValue() << endl;
				}

				ppu->showScreen().copyTo(screen);
				imshow("GBE", screen);
				key = waitKey(1);
			}
				
		}
		cout << "called standard opcodes" << endl;
		for (size_t i = 0; i < 0xFF; i++)
		{
			if (processor->calledOpcodes[i])
				cout << std::hex << i << endl;
		}
		cout << "called CB opcodes" << endl;
		for (size_t i = 0; i < 0xFF; i++)
		{
			if (processor->CBCalledOpcodes[i])
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