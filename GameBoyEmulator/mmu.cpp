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



MMU::MMU(uint8_t* block)
{
	uint8_t temp2 = 0;
	opcode = &temp2;
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
	memset(rom, 0, sizeof(rom));
	memset(romBank, 0, sizeof(romBank));
	memset(CRAM, 0, sizeof(CRAM));
	memset(BGMap1, 0, sizeof(BGMap1));
	memset(BGMap2, 0, sizeof(BGMap2));
	memset(cartridgeRAM, 0, sizeof(cartridgeRAM));
	memset(ROMBank, 0, sizeof(ROMBank));
	memset(GBCBank, 0, sizeof(GBCBank));
	memset(echo, 0, sizeof(echo));
	memset(oam, 0, sizeof(oam));
	memset(io, 0, sizeof(io));
	memset(zram, 0, sizeof(zram));
	memset(ppu, 0, sizeof(ppu));
	for (int i = 0; i < 0x4000; i++)
	{
		rom[i] = block[i];
	}
	for (int i = 0x4000; i < 0x8000; i++)
	{
		romBank[i - 0x4000] = block[i];
	}

	joyPadState = 0xff;
	/*writeByte(0xFF05, 0x00);
	writeByte(0xFF06, 0x00);
	writeByte(0xFF07, 0x00);
	writeByte(0xFF10, 0x80);
	writeByte(0xFF11, 0xBF);
	writeByte(0xFF12, 0xF3);
	writeByte(0xFF14, 0xBF);
	writeByte(0xFF16, 0x3F);
	writeByte(0xFF17, 0x00);
	writeByte(0xFF1A, 0x7F);
	writeByte(0xFF1B, 0xFF);
	writeByte(0xFF1C, 0x9F);
	writeByte(0xFF1E, 0xBF);
	writeByte(0xFF20, 0xFF);
	writeByte(0xFF21, 0x00);
	writeByte(0xFF22, 0x00);
	writeByte(0xFF23, 0xBF);
	writeByte(0xFF25, 0xF3);
	writeByte(0xFF26, 0xF1);
	writeByte(0xFF40, 0xF1);
	writeByte(0xFF42, 0x00);
	writeByte(0xFF43, 0x00);
	writeByte(0xFF45, 0x00);
	writeByte(0xFF47, 0xFC);
	writeByte(0xFF48, 0xFF);
	writeByte(0xFF49, 0xFF);
	writeByte(0xFF4A, 0x00);
	writeByte(0xFF4B, 0x00);
	writeByte(0xFFFF, 0x00);*/

}


MMU::~MMU()
{
}

void MMU::requestInterupt(uint8_t id)
{
	uint8_t requestRegister = readByte(0xFF0F);
	requestRegister |= (1 << id);
	writeByte(0xFF0F, requestRegister);
}

void MMU::setOpcode(uint8_t* ptr)
{
	this->opcode = ptr;
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

void MMU::setKeyPressed(int key)
{
	bool notSet = false;
	if (((joyPadState >> key) & 1) == 0) {
		notSet = true;
	}
	bool button = true;
	if (key < 3) {
		button = false;
	}
	bool requestInterrupt = false;
	uint8_t keyRequested = io[0];

	joyPadState &= (~(1 << key) & 0xFF);

	if (button && ((keyRequested >> 5) & 1) == 0) {
		requestInterrupt = true;
	}
	else if (!button && ((keyRequested >> 4) & 1) == 0)
	{
		requestInterrupt = true;
	}
	if (requestInterrupt && !notSet) {
		requestInterupt(4);
	}
}

void MMU::setKeyReleased(int key)
{
	joyPadState |= (1 << key);
}

uint8_t MMU::getJoypadState()
{
	uint8_t state = io[0];
	state = ~state;
	

	if (((state >> 4) & 1) == 0) {
		uint8_t top = joyPadState >> 4;
		top |= 0xF0;
		state &= top;
	}
	else if (((state >> 5) & 1) == 0) {
		uint8_t bottom = joyPadState & 0x0F;
		bottom |= 0xF0;
		state &= bottom;
	}
	//std::cout << std::bitset<8>(state) << std::endl;
	return state;
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
			else if (addr == 0x100) {

				inBios = false;
			}
		}
		else {
			return rom[addr];
		}
	case 0x1000:
	case 0x2000:
	case 0x3000:
		return rom[addr];

	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
		return romBank[(addr - 0x4000)];

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
		return cartridgeRAM[(addr - 0xA000)];
	case 0xC000:
		return ROMBank[addr - 0xC000];
	case 0xD000:
		return GBCBank[addr - 0xD000];
	case 0xF000:
		if (addr == 0xFFFF) {
			return interuptEnable;
		}
		if (addr >= 0xFF80) {

			return zram[addr - 0xFF80];
		}
		if (addr >= 0xFF40) {
			return ppu[addr - 0xFF40];
		}
		if (addr >= 0xFF00) {
			if (addr == 0xFF00) {
				return getJoypadState();
			}
			else {
				return io[addr - 0xFF00];
			}
		}
		if (addr >= 0xFEA0) {
			//std::cout << "you shouldn't read here      " <<std::hex << *opcode << std::endl;
			return unused[addr - 0xFEA0];
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
	//std::cout << std::hex<< (((readByte(addr + 1)) << 8) + readByte(addr)) << "addr" << std::endl;
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
		if (MBC1rules) {
			if ((value & 0xF) == 0xA) {
				enableRAM = true;
			}
			else if ((value & 0xF) == 0) {
				enableRAM = false;
			}
		}
		else if (MBC2rules) {
			if (((addr >> 8) & 1) == 0) {
				if ((value & 0xF) == 0xA) {
					enableRAM = true;
				}
				else if ((value & 0xF) == 0) {
					enableRAM = false;
				}
			}
		}
		break;
	case 0x2000:
	case 0x3000:
		if (MBC1rules) {
			if (value == 0) {
				value++;
			}
			value &= (~(~0U << 5));
			currRomBank &= 224;
			currRomBank |= value;
		}
		else if (MBC2rules) {
			value &= (~(~0U << 4));
			currRomBank = value;
		}
		break;

	case 0x4000:
	case 0x5000:
		if (MBC1rules) {
			if (ROMBanking) {
				currentBank = 0;
				value &= 3;
				value <<= 5;
				if ((currRomBank & (~(~0U << 5))) == 0) {
					value++;
				}
				currRomBank &= (~(~0U << 5));
				currRomBank |= value;
			}
			else {
				currentBank = value & 3;
			}
		}
		break;
	case 0x6000:
	case 0x7000:
		if (MBC1rules) {
			uint8_t bitZero = value & 1;
			ROMBanking = (bitZero == 0);
			if (ROMBanking) {
				currRomBank = 0;
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
			//std::cout << "addr: " << std::hex << (int)(addr - 0x8000) << "\tValue: " << std::hex << (int)value << std::endl;
			CRAM[addr - 0x8000] = value;
			break;
		}
		break;
	case 0xA000:
	case 0xB000:
		if (enableRAM) {
			if (MBC1rules) {
				uint16_t newaddress = addr - 0xA000;
				cartridgeRAM[(0x2000 * currentBank) + newaddress] = value;
			}
			else if (MBC2rules && (addr < 0xA200)) {
				uint16_t newaddress = addr - 0xA000;
				cartridgeRAM[(0x2000 * currentBank) + newaddress] = value;
			}
		}
		break;
	case 0xC000:
		ROMBank[addr - 0xC000] = value;
		break;
	case 0xD000:
		GBCBank[addr - 0xD000] = value;
		break;
	case 0xE000:

	case 0xF000:
		
		if (addr == 0xFFFF) {
			interuptEnable = value;
			break;
		}
		if (addr >= 0xFF80) {
			zram[addr - 0xFF80] = value;
			break;
		}
		if (addr >= 0xFF40) {
			if (addr == 0xFF46) {
				unsigned short newAddr = value << 8;
				for (int i = 0; i < 0xA0; i++) {
					writeByte(0xFE00 + i, readByte(newAddr + i));
				}
				break;
			}
			else if (addr == 0xFF44) {
				//whenever the gameboy tries to write to this location directly, the 
				//value gets set to zero
				ppu[4] = 0;
				break;
			}
			else {
				ppu[addr - 0xFF40] = value;
				break;
			}
		}
		if(addr >= 0xFF00) {
			if (addr == 0xFF07) { //TAC
				uint8_t currFreq = readByte(0xFF07) & 0x3;
				io[7] = value;
				uint8_t newFreq = readByte(0xFF07) & 0x3;
				if (newFreq != currFreq) {
					switch (newFreq) {
					case 0: *tCycles = 1024; break;
					case 1: *tCycles = 16; break;
					case 2: *tCycles = 64; break;
					case 3: *tCycles = 256; break;
					}
					
				}
				break;
			}
			else if (addr == 0xFF04) {
				io[4] = 0;
				break;
			}
			else {
				/*if (addr == 0xFF01) {
					std::cout << (unsigned char)value;
				}*/
				io[addr - 0xFF00] = value;
				break;
			}
		}
		if (addr >= 0xFEA0) {
			unused[addr - 0xFEA0] = value;
			//std::cout<<"nope: " << std::hex << (int)addr <<"\t"<< std::hex << (int)*opcode << std::endl;
			break;
		}
		if (addr >= 0xFE00) {
			oam[addr - 0xFE00] = value;
			break;
		}
		if (addr >= 0xE000) {
			writeByte(addr - 0x2000, value);
			break;
		}
	default:
		std::cout << "confused: " << std::hex << (int)addr << std::endl;
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
