#include "mmu.h"



mmu::mmu()
{
	interuptEnable = true;
}


mmu::~mmu()
{
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
		return romBank[addr - 0x4000];

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
		return cartridgeRAM[addr - 0xA000];
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
		rom[addr] = value;
		break;
	case 0x1000:
	case 0x2000:
	case 0x3000:
		rom[addr] = value;
		break;
	case 0x4000:
	case 0x5000:
	case 0x6000:
	case 0x7000:
		romBank[addr - 0x4000] = value;
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

void mmu::enableInterupt()
{
	interuptEnable = true;
}

void mmu::disableInterupt()
{
	interuptEnable = false;
}

bool mmu::interuptable()
{
	return interuptEnable;
}
