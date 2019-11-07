#pragma once
#include "cstdint";
#include <array>
#include<iostream> 

class mmu
{

	bool inBios = true;

	uint8_t bios[0x100] = {
			0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
			0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
			0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
			0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
			0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
			0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
			0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
			0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
			0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
			0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
			0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
			0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
			0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
			0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
			0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
			0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50
	};

	//$0000 - $00FF:	Restart and Interupt vectors
	//$0100 - $014F:	Cartridge Header/bios/ whatever
	//$0150 - $3FFF:	Cartridge ROM- Bank 0
	uint8_t rom[0x4000];

	//$4000 - $7FFF:	Cartridge ROM - Switchable bank
	uint8_t romBank[0x4000];

	//$8000 - $97FFF:	Character RAM
	uint8_t CRAM[0x1800];

	//$9800 - $9BFFF:	BG Map Data 1
	uint8_t BGMap1[0x400];

	//$9C00 - $9FFF:	BG Map data 2
	uint8_t BGMap2[0x400];

	//$A000 - $BFFF:	Cartridge RAM
	uint8_t cartridgeRAM[0x2000];

	//$C000 - $CFFF:	RAM Bank 0
	uint8_t ROMBank[0x1000];

	//$D000 - $DFFF:	GBC Bank 1 - 7
	uint8_t GBCBank[0x1000];

	//$E000 - $FDFF:	Echo Area, do not use
	uint8_t echo[0x1e00];

	//$FE00 - $FE9F:	Object Attribute Memory (OAM)
	uint8_t oam[0xA0];

	//$FEA0 - $FEFF:	Unused
	uint8_t unused[0x60];

	//$FF00 - $FF7F:	I/O Registers
	uint8_t io[0x40];
	uint8_t ppu[0x40];

	//$FF80 - $FFFE:	Zero Page 
	uint8_t zram[0x7f];

	//$FFFF:			Interupt Enable Flag
	bool interuptEnable {true};

public:
	mmu();
	~mmu();

	uint8_t readByte(uint8_t addr);
	uint16_t readWord(uint16_t addr);

	//for reading the next 2 bytes from memory, will mostly(entrirly?) use the PC for the pointer
	//uint16_t read2Byte(Register16* PC);

	void writeByte(uint8_t addr, uint8_t value);
	void writeWord(uint16_t addr, uint16_t value);

	void enableInterupt();
	void disableInterupt();
	bool interuptable();

};
