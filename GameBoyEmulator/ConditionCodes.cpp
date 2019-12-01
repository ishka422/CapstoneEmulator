#include "ConditionCodes.h"
#include <iostream>
#include <bitset>


ConditionCodes::ConditionCodes()
{
	F = new Register();
}

Register * ConditionCodes::getF()
{
	return F;
}

void ConditionCodes::setCarry()
{
	

	F->setValue(F->getValue() | (1 << 7));
	
}

void ConditionCodes::setHalfCarry()
{
	//XXX
	F->setValue(F->getValue() | (1 << 6));
}

void ConditionCodes::setSub()
{
	F->setValue(F->getValue() | (1 << 5));
}

void ConditionCodes::setZero()
{
	uint8_t val = F->getValue();
	val |= (1 << 4);
	F->setValue(val);
	
}

bool ConditionCodes::getCarry()
{
	return (F->getValue()>>7) ==1;
}

bool ConditionCodes::getHalfCarry()
{
	return ((F->getValue() >> 6) & 1) == 1;
}

bool ConditionCodes::getSub()
{
	return ((F->getValue() >> 5) & 1) == 1;
}

bool ConditionCodes::getZero()
{
	//std::cout << std::bitset<8>(F->getValue()) << std::endl;
	return ((F->getValue() >>4) &1) == 1;
}

void ConditionCodes::clearCarry()
{
	F->setValue(F->getValue() & ~(1 << 7));
}

void ConditionCodes::clearHalfCarry()
{
	F->setValue(F->getValue() & ~(1 << 6));
}

void ConditionCodes::clearSub()
{
	F->setValue(F->getValue() & ~(1 << 5));
}

void ConditionCodes::clearZero()
{
	F->setValue(F->getValue() & ~(1 << 4));
}

void ConditionCodes::clearFlags() {
	F->setValue(0);
}


ConditionCodes::~ConditionCodes()
{
}
