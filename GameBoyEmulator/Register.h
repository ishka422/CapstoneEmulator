#pragma once
#include "cstdint"
class Register
{
public:

	uint8_t value;
	void setValue(uint8_t val);
	uint8_t getValue();
	void increment();
	void decrement();

	Register();
	~Register();

private:
	
};

