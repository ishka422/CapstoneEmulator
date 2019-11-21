#include "mmu.h"



mmu::mmu()
{
	interuptEnable = true;
}


mmu::~mmu()
{
}

void mmu::setMBCRule(uint8_t setting)
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

uint8_t mmu::readByte(uint8_t addr)
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

uint16_t mmu::readWord(uint16_t addr)
{
	return readByte(addr) + (readByte(addr + 1) << 8);
}

void mmu::writeByte(uint8_t addr, uint8_t value)
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
					case 0: timer->tCycles = 1024; break;
					case 1: timer->tCycles = 16; break;
					case 2: timer->tCycles = 64; break;
					case 3: timer->tCycles = 256; break;
					}
				}

			}
			else if (addr == 0xFF04) {
				io[4] = 0;
			}
			else if (addr < 0xFF40) {
				io[addr - 0xFF00] = value;
			}
			else {
				if(addr = 0xFF44){
					ppu[4] = 0;
				}else
					ppu[addr - 0xFF40] = value;
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

void mmu::writeWord(uint16_t addr, uint16_t value)
{
	writeByte(addr, value & 0xFF);
	writeByte(addr + 1, (value >> 8) & 0xFF);
}

void mmu::connectTimer(Timer * timer)
{
	this->timer = timer;
}

bool mmu::interuptable()
{
	return interuptEnable;
}

void mmu::incDivider()
{
	io[4]++;
}

void mmu::incLY()
{
	ppu[4]++;
}
