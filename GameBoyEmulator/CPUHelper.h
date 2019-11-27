#pragma once
#include "MMU.h"
inline void CPU::load(Register * tar, uint8_t value)
{
	tar->setValue(value);
}

inline void CPU::ADD(uint8_t val)
{
	CC->clearFlags();
	int result = a->getValue() + val;
	a->setValue((uint8_t)(result & 0xFF));

	if ((result & 0xFF) == 0)
	{
		CC->setZero();
	}
	if ((result >> 8) != 0) {
		CC->setCarry();
	}
	if ((((a->getValue() & 0x0F) + (val & 0x0F)) & 0x10) != 0)
	{
		CC->setHalfCarry();
	}

}

inline void CPU::sub(uint8_t val)
{
	CC->clearFlags();
	int result = a->getValue() - val;
	a->setValue((uint8_t)(result & 0xFF));
	CC->setSub();
	if ((result & 0x100) != 0) {
		CC->setCarry();
	}
	if (((a->getValue() & 0x0F) - (val & 0x0F)) & 0x10 != 0)
	{
		CC->setHalfCarry();
	}
	if (result == 0) {
		CC->setZero();
	}

}

inline void CPU::SBC(uint8_t val)
{
	CC->clearFlags();
	int result = (a->getValue() - val) - CC->getCarry();
	CC->setSub();
	if ((result & 0x100) != 0)
	{
		CC->setCarry();
	}
	if (result == 0) {
		CC->setZero();
	}
	if (result < 0)
		CC->setCarry();
	if ((a->getValue() & 0x0F) - (val & 0x0F) - CC->getCarry())
		CC->setHalfCarry();
	a->setValue((uint8_t)(result & 0xFF));

}

inline void CPU::ADD_HL(uint16_t val)
{
	bool zero = CC->getZero();
	CC->clearFlags();
	int result = HL.getValue() + val;

	if (zero) {
		CC->setZero();
	}
	if ((result >> 16) != 0) {
		CC->setCarry();
	}
	if (((HL.getValue() >> 8) & 0x0F) + ((val >> 8) & 0x0F) & 0x10 != 0) {
		CC->setHalfCarry();
	}

	HL.setValue((uint16_t)(val & 0xFFFF));


}

inline void CPU::ADD_SP(int8_t val)
{
	CC->clearFlags();
	int result = SP->getValue() + val;


	if ((result >> 16) != 0) {
		CC->setCarry();
	}
	if (((HL.getValue() >> 8) & 0x0F) + ((val >> 8) & 0x0F) & 0x10 != 0) {
		CC->setHalfCarry();
	}

	SP->setValue((uint16_t)(val & 0xFFFF));
}

inline void CPU::CP(uint8_t val)
{
	CC->clearFlags();
	if (a->getValue() == val) {
		CC->setZero();
	}
	CC->setSub();
	if (((a->getValue() & 0x0F) - (val & 0x0F)) & 0x10 != 0)
	{
		CC->setHalfCarry();
	}
	if (a->getValue() < val) {
		CC->setCarry();
	}

}

inline void CPU::DEC8b(Register * reg)
{

	reg->decrement();
	if (reg->getValue() == 0) {
		CC->setZero();
	}
	else {
		CC->clearZero();
	}
	CC->setSub();
	if ((reg->getValue() & 0x0F) == 0x0F) {
		CC->setHalfCarry();
	}
	else {
		CC->clearHalfCarry();
	}


}

inline void CPU::INC8b(Register * reg)
{
	reg->increment();
	if (reg->getValue() == 0) {
		CC->setZero();
	}
	else {
		CC->clearZero();
	}
	if ((reg->getValue() & 0x0F) == 0x00) {
		CC->setHalfCarry();
	}
	else {
		CC->clearHalfCarry();
	}
}

inline void CPU::ADC(uint8_t val)
{
	int result = a->getValue() + val + CC->getCarry();
	CC->clearFlags();
	a->setValue((uint8_t)(result & 0xFF));
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	if (result > 0xFF) {
		CC->setCarry();
	}
	if (((a->getValue() & 0x0F) + (val & 0x0F) + CC->getCarry()) & 0x10 != 0) {
		CC->setHalfCarry();
	}

}

inline void CPU::AND(uint8_t val)
{
	CC->clearFlags();
	uint8_t result = a->getValue() & val;
	if (result == 0) {
		CC->setZero();
	}
	CC->setHalfCarry();
	a->setValue(result);
}

inline void CPU::OR(uint8_t val)
{
	CC->clearFlags();
	uint8_t result = a->getValue() & val;
	if (result == 0) {
		CC->setZero();
	}
	a->setValue(result);
}

inline void CPU::XOR(Register * reg)
{
	uint8_t result = a->getValue() ^ reg->getValue();
	CC->clearFlags();
	if (result == 0) {
		CC->setZero();
	}
	a->setValue(result);
}



inline void CPU::BIT(uint8_t val, int bit)
{
	if (((val >> bit) & 0x01) ==0) {
		CC->setZero();
	}
	else {
		CC->clearZero();
	}
	CC->setHalfCarry();
	CC->clearSub();
}

inline void CPU::RES(Register* reg, int bit)
{
	reg->setValue(reg->getValue() & (~(1 << bit)));
}

inline void CPU::RL(Register * reg)
{
	int carrySet = (CC->getCarry()) ? 1 : 0;
	int result = reg->getValue();
	CC->clearFlags();
	if ((result >> 7) == 1) {
		CC->setCarry();
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	result <<= 1;
	result |= carrySet;
	reg->setValue((uint8_t)(result & 0xFF));
}

inline void CPU::RLC(Register * reg)
{

	int result = reg->getValue();
	CC->clearFlags();
	result <<= 1;

	if ((reg->getValue() >> 7) == 1) {
		CC->setCarry();
		result |= 1;
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}

	reg->setValue((uint8_t)(result & 0xFF));
}

inline void CPU::RR(Register * reg)
{
	int carrySet = (CC->getCarry()) ? (1 << 7) : 0;
	int result = reg->getValue();
	CC->clearFlags();
	if ((result & 1) == 1) {
		CC->setCarry();
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	result >>= 1;
	result |= carrySet;
	reg->setValue((uint8_t)(result & 0xFF));
}

inline void CPU::RRC(Register * reg)
{
	int result = reg->getValue();
	CC->clearFlags();
	result >>= 1;

	if ((reg->getValue() & 1) == 1) {
		CC->setCarry();
		result |= 1 << 7;
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}

	reg->setValue((uint8_t)(result & 0xFF));
}

inline void CPU::SET(Register * reg, int bit)
{
	reg->setValue(reg->getValue() | (1 << bit));
}

inline void CPU::SLA(Register * reg)
{
	CC->clearFlags();
	if ((reg->getValue() >> 7) != 0) {
		CC->setCarry();
	}
	reg->setValue(reg->getValue() << 1);
	if (reg->getValue() == 0) {
		CC->setZero();
	}
}

inline void CPU::SRA(Register * reg)
{
	CC->clearFlags();
	uint8_t msb = reg->getValue() & (1 << 7);
	if ((reg->getValue() & 1) == 1) {
		CC->setCarry();
	}

	reg->setValue((reg->getValue() >> 1) | msb);
	if (reg->getValue() == 0) {
		CC->setZero();
	}
}

inline void CPU::SRL(Register * reg)
{
	CC->clearFlags();
	if ((reg->getValue() & 1) == 1) {
		CC->setCarry();
	}

	reg->setValue(reg->getValue() >> 1);
	if (reg->getValue() == 0) {
		CC->setZero();
	}
}

inline uint8_t CPU::SWAP(uint8_t val)
{
	uint8_t low = val & 0x0F;
	val >>= 4;
	low <<= 4;
	return (low & val);
}



inline void CPU::PUSH(Register16* reg)
{
	SP->dec();
	memory->writeByte(SP->getValue(), reg->getHigh());
	SP->dec();
	memory->writeByte(SP->getValue(), reg->getLow());
}

inline void CPU::POP(Register16 * reg)
{
	reg->setLow(memory->readByte(SP->getValue()));
	SP->inc();
	reg->setHigh(memory->readByte(SP->getValue()));
	SP->inc();

}

inline void CPU::CALL()
{
	uint8_t lo = memory->readByte(PC->getValue());
	PC->inc();
	uint8_t hi = memory->readByte(PC->getValue());
	PC->inc();
	PUSH(PC);
	PC->setHigh(hi);
	PC->setLow(lo);
}