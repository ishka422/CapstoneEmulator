#pragma once
#include "Register.h"

class Register16
{
public:
	Register16();
	void setLow(uint8_t val);
	void setHigh(uint8_t val);
	uint8_t getLow();
	uint8_t getHigh();
	void setHighRegister(Register* reg);
	void setLowRegister(Register* reg);
	Register* getHighReg();
	Register* getLowReg();
	void setValue(uint16_t val);
	uint16_t getValue();
	void inc();
	void dec();
	~Register16();
	Register* high;
	Register* low;

private:
	
};

