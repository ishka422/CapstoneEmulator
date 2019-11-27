#pragma once
#include "MMU.h"

uint8_t MMU::bios[0x100] = {
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



MMU::MMU()
{
	MBC1rules = false;
	MBC2rules = false;
	enableRAM = false;
	ROMBanking = false;
	currentBank = 0;
	currRomBank = 0;
	int temp = 0;
	tCycles = &temp;
	interuptEnable = true;
	inBios = true;
}


MMU::~MMU()
{
}

void MMU::setTCycles(int * tCycles)
{
	this->tCycles = tCycles;
}

void MMU::setMBCRule(uint8_t setting)
{
	switch (setting) {
	case 1:
	case 2:
	case 3:
		MBC1rules = true;
		break;
	case 4:
	case 5:
	case 6:
		MBC2rules = true;
		break;
	default: 
		break;
	}
}

uint8_t MMU::readByte(uint16_t addr)
{
	switch (addr & 0xF000)
	{
	case 0x0000:
		if (inBios) {
			if (addr < 0x0100) {
				return bios[addr];
			}
			else if (addr == 0x0100) {
				inBios = false;
			}
		}
		return rom[addr];
	case 0x1000:
	case 0x2000:
	case 0x3000:
		return rom[addr];

	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
		return romBank[(addr - 0x4000) + (currRomBank * 0x4000)];

	case 0x8000:
	case 0x9000:
		switch (addr & 0xFF00) {
		case 0x9800:
		case 0x9900:
		case 0x9A00:
		case 0x9B00:
			return BGMap1[addr - 0x9800];
		case 0x9C00:
		case 0x9D00:
		case 0x9E00:
		case 0x9F00:
			return BGMap2[addr - 0x9C00];
		default:
			return CRAM[addr - 0x8000];
		}
	case 0xA000:
	case 0xB000:
		return cartridgeRAM[currentBank * (addr - 0xA000)];
	case 0xC000:
		return ROMBank[addr - 0xC000];
	case 0xD000:
		return GBCBank[addr - 0xD000];
	case 0xF000:
		if (addr >= 0xFF80) {
			return zram[addr - 0xFF80];
		}
		if (addr >= 0xFF00) {
			//implement IO later
			return 0;
		}
		if (addr >= 0xFEA0) {
			std::cout << "you shouldn't be here" << std::endl;
			return 0;
		}
		if (addr >= 0xFE00) {
			return oam[addr - 0xFE00];
		}


	default:
		break;
	}


}

uint16_t MMU::readWord(uint16_t addr)
{
	std::cout << (unsigned int)addr << "addr" << std::endl;
	return ((readByte(addr + 1)) << 8) + readByte(addr);
}

void MMU::writeByte(uint16_t addr, uint8_t value)
{
	switch (addr & 0xF000)
	{
	case 0x0000:
		if (inBios) {
			if (addr < 0x0100) {
				bios[addr] = value;
				break;
			}
			else if (addr == 0x0100) {
				inBios = false;
				break;
			}
		}
	case 0x1000:
		if (MBC1rules || MBC2rules) {
			//enable RAM banking
			if (MBC2rules) {
				//if MBC2, bit 4 must be 0
				if (((addr >> 3) & 1) == 1) {
					return;
				}
			}
			//0xA enables RAM
			if ((value & 0xF) == 0xA) {
				enableRAM = true;
			}
			//0x0 disables RAM
			else if ((value & 0xF) == 0) {
				enableRAM = false;
			}
		}
		break;
	case 0x2000:
	case 0x3000:
		if (MBC1rules || MBC2rules) {
			if (MBC2rules) {
				currRomBank = value & 0xF;
				if (currRomBank == 0) {
					currRomBank++;
				}
				return;
			}
			uint8_t loFive = value & ~(~0U << 5);
			value &= (~(~0U << 3) << 5);
			value += loFive;
			if (currRomBank == 0) {
				currRomBank++;
			}
		}
		break;

	case 0x4000:
	case 0x5000:
		if (MBC1rules) {
			if (ROMBanking) {
				currRomBank &= ~(~0U << 5);
				value &= (~(~0U << 3) << 5);
				currRomBank += value;
				if (currRomBank == 0) {
					currRomBank++;
				}
			}
			else {
				currentBank = value & (~(~0U << 2));
			}
		}
		break;
	case 0x6000:
	case 0x7000:
		if (MBC1rules) {
			uint8_t bitZero = value & 1;
			ROMBanking = (bitZero == 0);
			if (ROMBanking) {
				currRomBank == 0;
			}
		}
		break;
	case 0x8000:
	case 0x9000:
		switch (addr & 0xFF00) {
		case 0x9800:
		case 0x9900:
		case 0x9A00:
		case 0x9B00:
			BGMap1[addr - 0x9800] = value;
			break;
		case 0x9C00:
		case 0x9D00:
		case 0x9E00:
		case 0x9F00:
			BGMap2[addr - 0x9C00] = value;
			break;
		default:
			CRAM[addr - 0x8000] = value;
			break;
		}
		break;
	case 0xA000:
	case 0xB000:
		cartridgeRAM[addr - 0xA000] = value;
		break;
	case 0xC000:
		ROMBank[addr - 0xC000] = value;
		break;
	case 0xD000:
		GBCBank[addr - 0xD000] = value;
		break;
	case 0xF000:
		if (addr >= 0xFF80) {
			zram[addr - 0xFF80] = value;
			break;
		}
		if (addr >= 0xFF00) {
			if (addr == 0xFF07) { //TAC
				uint8_t currFreq = readByte(0xFF07) & 0x3;
				io[7] = value;
				uint8_t newFreq = value & 0x3;
				if (newFreq != currFreq) {
					switch (newFreq) {
					case 0: *tCycles = 1024; break;
					case 1: *tCycles = 16; break;
					case 2: *tCycles = 64; break;
					case 3: *tCycles = 256; break;
					}
					break;
				}

			}
			else if (addr == 0xFF04) {
				io[4] = 0;
				break;
			}
			else if (addr < 0xFF40) {
				io[addr - 0xFF00] = value;
				break;
			}
			else {
				//whenever the gameboy tries to write to this location directly, the 
				//value gets set to zero
				if (addr == 0xFF46) {
					unsigned short newAddr = value << 8;
					for (int i = 0; i < 0xA0; i++) {
						writeByte(0xFE00 + i, readByte(newAddr + i));
					}
				}
				if(addr = 0xFF44){
					ppu[4] = 0;
					break;
				}
				else {
					ppu[addr - 0xFF40] = value;
					break;
				}
			}

			//implement IO later
			break;
		}
		if (addr >= 0xFEA0) {
			std::cout << "you shouldn't be here" << std::endl;
			break;
		}
		if (addr >= 0xFE00) {
			oam[addr - 0xFE00] = value;
			break;
		}
	default:
		break;
	}
}

void MMU::writeWord(uint16_t addr, uint16_t value)
{
	writeByte(addr, value & 0xFF);
	writeByte(addr + 1, (value >> 8) & 0xFF);
}

bool MMU::interuptable()
{
	return interuptEnable;
}

void MMU::incDivider()
{
	io[4]++;
}

void MMU::incLY()
{
	ppu[4]++;
}
