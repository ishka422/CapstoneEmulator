#include "Register16.h"



Register16::Register16()
{
	high->setValue(0);
	low->setValue(0);
}

void Register16::setLow(uint8_t val)
{
	low->setValue(val);
}

void Register16::setHigh(uint8_t val)
{
	high->setValue(val);
}

uint8_t Register16::getLow()
{
	return low->getValue();
}

uint8_t Register16::getHigh()
{
	return high->getValue();
}

Register* Register16::getHighReg()
{
	return high;
}

Register* Register16::getLowReg()
{
	return low;
}

void Register16::setValue(uint16_t val)
{
	high->setValue((val >> 8) & 0xFF);
	low->setValue(val & 0xFF);
}

uint16_t Register16::getValue()
{
	return ((high->getValue() << 8) + low->getValue()) & 0xFFFF;
}

void Register16::inc()
{
	uint16_t value = getValue();
	value++;
	setValue(value);
}

void Register16::dec()
{
	uint16_t value = getValue();
	value--;
	setValue(value);
}


Register16::~Register16()
{
}
