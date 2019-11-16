#include <iostream>
#include <stdio.h>
#include <fstream>
#include "mmu.h"
#include "CPU.h"

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
		ifstream gameROM;
		gameROM.open(argv[1], ios::in | ios::binary | ios::ate);
		int size = gameROM.tellg();
		char *cartridgeBlock = new char[size];
		gameROM.seekg(0, ios::beg);
		gameROM.read(cartridgeBlock, size);

		int cyclesThisUpdate = 0;

		mmu* memory();
		memory.setMBCRules(cartridgeBlock[0x147])
		CPU* processor(memory);
		Timer timer(processor);

		 
		while (true) {
			cyclesThisUpdate = 0;
			if (timer.needScreenRefresh()) {
				while (cyclesThisUpdate < MAXCYCLES) {
					//process opcodes
					//cTU += timer.update
				}					
				//update time
			//update screen
			}
				
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