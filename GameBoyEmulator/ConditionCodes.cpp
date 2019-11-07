#include "ConditionCodes.h"



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

	F->setValue(F->getValue() | 1 << 4);
	
}

bool ConditionCodes::getCarry()
{
	return (F->getValue()>>7) ==1;
}

bool ConditionCodes::getHalfCarry()
{
	return ((F->getValue() & (1<<6)) >> 6) == 1;
}

bool ConditionCodes::getSub()
{
	return ((F->getValue() & (1 << 5)) >> 5) == 1;
}

bool ConditionCodes::getZero()
{
	return ((F->getValue() & (1 << 4)) >> 4) == 1;
}

void ConditionCodes::clearCarry()
{
	F->setValue(F->getValue() & (0 << 7));
}

void ConditionCodes::clearHalfCarry()
{
	F->setValue(F->getValue() & (0 << 6));
}

void ConditionCodes::clearSub()
{
	F->setValue(F->getValue() & (0 << 5));
}

void ConditionCodes::clearZero()
{
	F->setValue(F->getValue() & (0 << 4));
}

void ConditionCodes::clearFlags() {
	F->setValue(0);
}


ConditionCodes::~ConditionCodes()
{
}
