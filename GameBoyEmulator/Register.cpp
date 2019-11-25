#include "Register.h"


void Register::increment()
{
	value++;
}

void Register::decrement()
{
	value--;
}

Register::~Register()
{
	delete &value;
}

void Register::setValue(uint8_t val)
{
	value = val;
}

uint8_t Register::getValue()
{
	return value;
}

