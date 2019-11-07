#pragma once
#include "cstdint";
class Register
{
public:

	void increment();
	void setValue(uint8_t val);
	uint8_t getValue();
	void increment();
	void decrement();

	Register() : value(0) {};
	~Register();

private:
	uint8_t value;
};

