#ifndef MMU_H
#define MMU_H


#pragma once
#include "cstdint"
#include <array>
#include <iostream> 
#include "timings.h"
#include <bitset>
#include <vector>
#include <algorithm>






class MMU
{
	bool MBC1rules;
	bool MBC2rules;
	bool enableRAM;
	bool ROMBanking;
	int currentBank;
	int currRomBank;
	int* tCycles;
	uint8_t* opcode;
	uint8_t joyPadState;
	uint8_t* romBlock;

	
	
public:
	bool inBios;
	static uint8_t bios[0x100];
	bool changedTile[0x180];
	bool replacedTile[0x180];
	
	bool writingToVRAM;
	bool pendingChanges;
	

	//$0000 - $00FF:	Restart and Interupt vectors
	//$0100 - $014F:	Cartridge Header/bios/ whatever
	//$0150 - $3FFF:	Cartridge ROM- Bank 0
	uint8_t rom[0x4000];

	//$4000 - $7FFF:	Cartridge ROM - Switchable bank
	uint8_t romBank[0x20000];

	//$8000 - $97FF:	Character RAM
	uint8_t CRAM[0x1800];

	//$9800 - $9BFFF:	BG Map Data 1
	uint8_t BGMap1[0x400];

	//$9C00 - $9FFF:	BG Map data 2
	uint8_t BGMap2[0x400];

	//$A000 - $BFFF:	Cartridge RAM  
	//0x8000 to support up to 4 RAM banks
	uint8_t cartridgeRAM[0x8000];

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
	uint8_t interuptEnable;


	MMU(uint8_t* block);
	~MMU();
	void requestInterupt(uint8_t id);
	void setOpcode(uint8_t* ptr);
	void setTCycles(int* tCycles);
	void setMBCRule(uint8_t setting);
	void setKeyPressed(int key);
	void setKeyReleased(int key);
	uint8_t getJoypadState();

	uint8_t readByte(uint16_t addr);
	uint16_t readWord(uint16_t addr);

	//for reading the next 2 bytes from memory, will mostly(entrirly?) use the PC for the pointer
	//uint16_t read2Byte(Register16* PC);

	void writeByte(uint16_t addr, uint8_t value);
	void writeWord(uint16_t addr, uint16_t value);

	/*void connectTimer(Timer* timer);*/
	/*void enableInterupt();
	void disableInterupt();*/
	bool interuptable();
	void incDivider();
	void incLY();

};

#endif