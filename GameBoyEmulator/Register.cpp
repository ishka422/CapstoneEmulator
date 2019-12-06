#include "Register.h"
#include <iostream>

void Register::increment()
{
	value++;
}

void Register::decrement()
{
	value--;
}

Register::Register()
{
	value = 0;
}

Register::~Register()
{
	//value = NULL;
}

void Register::setValue(uint8_t val)
{
	value = val;
}

uint8_t Register::getValue()
{
	return value;
}

