#pragma once
#include"Register.h"
class ConditionCodes
{
public:
	ConditionCodes();
	Register* getF();
	void setCarry();
	void setHalfCarry();
	void setSub();
	void setZero();
	bool getCarry();
	bool getHalfCarry();
	bool getSub();
	bool getZero();
	void clearCarry();
	void clearHalfCarry();
	void clearSub();
	void clearZero();
	void clearFlags();
	~ConditionCodes();
private:
	Register* F;
	

};

