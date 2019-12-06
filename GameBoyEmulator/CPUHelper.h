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
	a->setValue((uint8_t)(result & 0xFF));
}

inline void CPU::sub(uint8_t val)
{
	CC->clearFlags();
	int result = a->getValue() - val;
	
	int carryBits = a->getValue() ^ val ^ result;
	CC->setSub();
	if ((carryBits & 0x100) != 0) {
		CC->setCarry();
	}
	{
	if ((carryBits & 0x10) != 0)
		CC->setHalfCarry();
	}
	if (result == 0) {
		CC->setZero();
	}
	a->setValue((uint8_t)(result & 0xFF));
}

inline void CPU::SBC(uint8_t val)
{
	int carry = CC->getCarry() ? 1:0;
	int result = a->getValue() - val - carry;
	CC->clearFlags();
	CC->setSub();
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	if (result < 0) {
		CC->setCarry();
	}
	
	if (((a->getValue() & 0xF) - (val & 0xF) - carry) < 0) {
		CC->setHalfCarry();
	}
	a->setValue((uint8_t)(result&0xFF));

}

inline void CPU::ADD_HL(uint16_t val)
{
	bool zero = CC->getZero();
	CC->clearFlags();
	int result = HL->getValue() + val;

	if (zero) {
		CC->setZero();
	}
	if ((result &0x10000)!= 0) {
		CC->setCarry();
	}
	if ((HL->getValue()&0xFFF) + (val &0xFFF) > 0xFFF) {
		CC->setHalfCarry();
	}

	HL->setValue((uint16_t)(result));
}

inline void CPU::ADD_SP(int8_t val)
{
	CC->clearFlags();
	unsigned int result = SP->getValue() + val;

	if ((result >> 16) != 0) {
		CC->setCarry();
	}
	if ((((SP->getValue() >> 8) & 0x0F) + ((val >> 8) & 0x0F) & 0x10) != 0) {
		CC->setHalfCarry();
	}

	SP->setValue((uint16_t)(result & 0xFFFF));
}

inline void CPU::CP(uint8_t val)
{
	CC->clearFlags();
	if ((a->getValue() == val)) {
		CC->setZero();
	}
	CC->setSub();
	if (((a->getValue() - val)&0xF) > (a->getValue() &0xF))
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
	CC->clearSub();
	if (reg->getValue() == 0) {
		CC->setZero();
	}
	else {
		CC->clearZero();
	}
	if ((reg->getValue() & 0x0F) ==0x00) {
		CC->setHalfCarry();
	}
	else {
		CC->clearHalfCarry();
	}
}

inline void CPU::ADC(uint8_t val)
{
	int carry = CC->getCarry() ? 1 : 0;
	int result = a->getValue() + val + carry;
	CC->clearFlags();
	
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	if (result > 0xFF) {
		CC->setCarry();
	}
	if ((((a->getValue() & 0x0F) + (val & 0x0F) + carry)) > 0x0F) {
		CC->setHalfCarry();
	}
	a->setValue((uint8_t)(result & 0xFF));
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
	uint8_t result = a->getValue() | val;
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
	//std::cout << std::hex << val << std::endl;
	if (((val >> bit) & 1) == 0) {
		//cout << "true" << endl;
		CC->setZero();
	}
	else {
		//cout << "false" << endl;
		CC->clearZero();
	}
	CC->setHalfCarry();
	CC->clearSub();
}

inline void CPU::RES(Register* reg, int bit)
{
	reg->setValue(reg->getValue() & (~(0x1 << bit)));
}

inline void CPU::RL(Register * reg)
{
	bool carrySet = (CC->getCarry());
	bool MSB = (reg->getValue() >> 7) == 1;
	uint8_t result = reg->getValue();
	CC->clearFlags();
	result <<= 1;
	if (MSB)
		CC->setCarry();
	if (carrySet)
		result |= 1;
	if (result == 0)
		CC->setZero();
	reg->setValue(result);
}

inline void CPU::RLC(Register * reg)
{
	bool msb = (reg->getValue() >> 7) == 1;
	CC->clearFlags();
	uint8_t result = reg->getValue();
	result <<= 1;

	if (msb) {
		CC->setCarry();
		result |= 1;
	}
	if (result == 0) {
		CC->setZero();
	}
	reg->setValue(result);
}

inline void CPU::RR(Register * reg)
{
	bool carrySet = CC->getCarry();
	bool lsb = (reg->getValue() & 1) == 1;

	CC->clearFlags();
	uint8_t result = reg->getValue() >> 1;

	if (lsb) {
		CC->setCarry();
	}
	if (carrySet) {
		result |= (1 << 7);
	}
	if (result == 0) {
		CC->setZero();
	}

	reg->setValue(result);
	
}

inline void CPU::RRC(Register * reg)
{
	uint8_t result = reg->getValue();
	bool lsb = (result & 1) == 1;
	CC->clearFlags();

	result >>= 1;

	if (lsb){
		CC->setCarry();
		result |= (1 << 7);
	}
	if (result == 0) {
		CC->setZero();
	}
	reg->setValue(result);
}

inline void CPU::SET(Register * reg, int bit)
{
	reg->setValue(reg->getValue() | (1 << bit));
}

inline void CPU::SLA(Register * reg)
{
	CC->clearFlags();
	uint8_t result = reg->getValue();
	bool msb = (result >> 7) == 1;

	result <<= 1;

	if (msb) {
		CC->setCarry();
	}
	if (result == 0) {
		CC->setZero();
	}
	reg->setValue(result);
	
}

inline void CPU::SRA(Register * reg)
{
	uint8_t result = reg->getValue();
	bool lsb = (result & 1) == 1;
	bool msb = (result >> 7) == 1;

	CC->clearFlags();

	result >>= 1;

	if (msb) {
		result |= (1 << 7);
	}
	if (lsb) {
		CC->setCarry();
	}
	if (result == 0) {
		CC->setZero();
	}
	reg->setValue(result);
}

inline void CPU::SRL(Register * reg)
{
	uint8_t result = reg->getValue();
	bool lsb = (result & 1) == 1;

	CC->clearFlags();

	result >>= 1;

	if (lsb) {
		CC->setCarry();
	}
	if (result == 0) {
		CC->setZero();
	}

	reg->setValue(result);
}

inline uint8_t CPU::SWAP(uint8_t val)
{
	CC->clearFlags();
	uint8_t low = val & 0x0F;
	val >>= 4;
	low <<= 4;
	if ((low | val) == 0) {
		CC->setZero();
	}
	return (low | val);
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