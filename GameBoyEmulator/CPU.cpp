#include "CPU.h"

using Action = std::function<void()>;

void CPU::load(Register * tar, uint8_t value)
{
	tar->setValue(value);
}

void CPU::ADD(uint8_t val)
{
	CC->clearFlags();
	int result = a->getValue() + val;
	a->setValue((uint8_t)(result & 0xFF));
	
	if ((result & 0xFF)== 0)
	{
		CC->setZero();
	}
	if ((result >> 8) != 0) {
		CC->setCarry();
	}
	if (((a->getValue() & 0x0F) + (val & 0x0F)) & 0x10 != 0)
	{
		CC->setHalfCarry();
	}

}

void CPU::sub(uint8_t val)
{
	CC->clearFlags();
	int result = a->getValue - val;
	a->setValue((uint8_t)(result & 0xFF));
	CC->setSub();
	if ((result & 0x100) != 0) {
		CC->setCarry();
	}
	if (((a->getValue() & 0x0F) - (val & 0x0F)) & 0x10 != 0)
	{
		CC->setHalfCarry();
	}
	if (result == 0) {
		CC->setZero();
	}
	
}

void CPU::SBC(uint8_t val)
{
	CC->clearFlags();
	int result = (a->getValue() - val) - CC->getCarry();
	CC->setSub();
	if ((result & 0x100) != 0)
	{
		CC->setCarry();
	}
	if (result == 0) {
		CC->setZero();
	}
	if (result < 0)
		CC->setCarry();
	if ((a->getValue() & 0x0F) - (val & 0x0F) - CC->getCarry())
		CC->setHalfCarry();
	a->setValue((uint8_t)(result & 0xFF));

}

void CPU::ADD_HL(uint16_t val)
{
	bool zero = CC->getZero();
	CC->clearFlags();
	int result = HL.getValue() + val;

	if (zero) {
		CC->setZero();
	}
	if ((result >> 16) != 0) {
		CC->setCarry();
	}
	if (((HL.getValue() >> 8) & 0x0F) + ((val >> 8) & 0x0F) & 0x10 != 0) {
		CC->setHalfCarry();
	}

	HL.setValue((uint16_t)(val & 0xFFFF));


}

void CPU::ADD_SP(int8_t val)
{
	CC->clearFlags();
	int result = SP->getValue() + val;


	if ((result >> 16) != 0) {
		CC->setCarry();
	}
	if (((HL.getValue() >> 8) & 0x0F) + ((val >> 8) & 0x0F) & 0x10 != 0) {
		CC->setHalfCarry();
	}

	SP->setValue((uint16_t)(val & 0xFFFF));
}

void CPU::CP(uint8_t val)
{
	CC->clearFlags();
	if (a->getValue() == val) {
		CC->setZero;
	}
	CC->setSub();
	if (((a->getValue() & 0x0F) - (val & 0x0F)) & 0x10 != 0)
	{
		CC->setHalfCarry();
	}
	if (a->getValue() < val) {
		CC->setCarry();
	}

}

void CPU::DEC8b(Register * reg)
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

void CPU::INC8b(Register * reg)
{
	reg->increment();
	if (reg->getValue() == 0) {
		CC->setZero();
	}
	else {
		CC->clearZero();
	}
	if ((reg->getValue() & 0x0F) == 0x00) {
		CC->setHalfCarry();
	}
	else {
		CC->clearHalfCarry();
	}
}

void CPU::ADC(uint8_t val)
{
	int result = a->getValue() + val + CC->getCarry();
	CC->clearFlags();
	a->setValue((uint8_t)(result & 0xFF));
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	if (result > 0xFF) {
		CC->setCarry();
	}
	if (((a->getValue() & 0x0F) + (val & 0x0F) + CC->getCarry()) & 0x10 != 0) {
		CC->setHalfCarry();
	}
	
}

void CPU::AND(uint8_t val)
{
	CC->clearFlags();
	uint8_t result = a->getValue() & val;
	if (result == 0) {
		CC->setZero();
	}
	CC->setHalfCarry();
	a->setValue(result);
}

void CPU::OR(uint8_t val)
{
	CC->clearFlags();
	uint8_t result = a->getValue() & val;
	if (result == 0) {
		CC->setZero();
	}
	a->setValue(result);
}

void CPU::XOR(Register * reg)
{
	uint8_t result = a->getValue() ^ reg->getValue();
	CC->clearFlags();
	if (result == 0) {
		CC->setZero();
	}
	a->setValue(result);
}



void CPU::BIT(uint8_t val, int bit)
{
	if ((val >> bit) & 0x01) {
		CC->setZero();
	}
	else {
		CC->clearZero();
	}
	CC->setHalfCarry();
	CC->clearSub();
}

void CPU::RES(Register* reg, int bit)
{
	reg->setValue(reg->getValue() & (~(1 << bit)));
}

void CPU::RL(Register * reg)
{
	int carrySet = (CC->getCarry()) ? 1 : 0;
	int result = reg->getValue();
	CC->clearFlags();
	if((result >> 7) == 1){
		CC->setCarry();
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	result <<= 1;
	result |= carrySet;
	reg->setValue((uint8_t)(result & 0xFF));
}

void CPU::RLC(Register * reg)
{

	int result = reg->getValue();
	CC->clearFlags();
	result <<= 1;
	
	if ((reg->getValue() >> 7) == 1) {
		CC->setCarry();
		result |= 1;
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}

	reg->setValue((uint8_t)(result & 0xFF));
}

void CPU::RR(Register * reg)
{
	int carrySet = (CC->getCarry()) ? (1<<7) : 0;
	int result = reg->getValue();
	CC->clearFlags();
	if ((result & 1) == 1) {
		CC->setCarry();
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}
	result >>= 1;
	result |= carrySet;
	reg->setValue((uint8_t)(result & 0xFF));
}

void CPU::RRC(Register * reg)
{
	int result = reg->getValue();
	CC->clearFlags();
	result >>= 1;

	if ((reg->getValue() & 1) == 1) {
		CC->setCarry();
		result |= 1<<7;
	}
	if ((result & 0xFF) == 0) {
		CC->setZero();
	}

	reg->setValue((uint8_t)(result & 0xFF));
}

void CPU::SET(Register * reg, int bit)
{
	reg->setValue(reg->getValue() | (1 << bit));
}

void CPU::SLA(Register * reg)
{
	CC->clearFlags();
	if ((reg->getValue() >> 7) != 0) {
		CC->setCarry();
	}
	reg->setValue(reg->getValue() << 1);
	if (reg->getValue() == 0) {
		CC->setZero;
	}
}

void CPU::SRA(Register * reg)
{
	CC->clearFlags();
	uint8_t msb = reg->getValue() & (1 << 7);
	if ((reg->getValue() & 1) == 1) {
		CC->setCarry();
	}

	reg->setValue((reg->getValue() >> 1) | msb);
	if (reg->getValue() == 0) {
		CC->setZero();
	}
}

void CPU::SRL(Register * reg)
{
	CC->clearFlags();
	if ((reg->getValue() & 1) == 1) {
		CC->setCarry();
	}

	reg->setValue(reg->getValue() >> 1);
	if (reg->getValue() == 0) {
		CC->setZero();
	}
}

uint8_t CPU::SWAP(uint8_t val)
{
	uint8_t low = val & 0x0F;
	val >>= 4;
	low <<= 4;
	return (low & val);
}



void CPU::PUSH(Register16* reg)
{
	SP->dec();
	memory->writeByte(SP->getValue, reg->getHigh());
	SP->dec();
	memory->writeByte(SP->getValue, reg->getLow());
}

void CPU::POP(Register16 * reg)
{
	reg->setLow(memory->readByte(SP->getValue));
	SP->inc();
	reg->setHigh(memory->readByte(SP->getValue()));
	SP->inc();

}

void CPU::CALL()
{
	uint8_t lo = memory->readByte(PC->getValue());
	PC->inc();
	uint8_t hi = memory->readByte(PC->getValue());
	PC->inc();
	PUSH(PC);
	PC->setHigh(hi);
	PC->setLow(lo);
}

CPU::CPU(mmu* memory)
{

	map <uint8_t, Action> opcodes;
	this->memory = memory;


}

CPU::~CPU()
{
}

uint16_t CPU::get2Bytes()
{
	uint8_t hi = memory->readByte(PC->getValue() + 1);
	uint8_t lo = memory->readByte(PC->getValue());
	PC->inc();
	PC->inc();

	uint16_t val = (hi << 8) | (lo);
	
	return val;
}



// Load Instructions
void CPU::LA_A_C(){

	a->setValue(c->getValue());
}
void CPU::LD_A_A(){
	a->setValue(a->getValue());
}
void CPU::LD_A_a16(){
	a->setValue(
		memory->readByte(memory->read2Byte(PC))
	);
	PC->inc();
	PC->inc();
}
void CPU::LD_A_aBC(){
	a->setValue(memory->readByte(BC.getValue));
}
void CPU::LD_A_aDE(){
	a->setValue(memory->readByte(DE.getValue));
}
void CPU::LD_A_aHL(){
	a->setValue(memory->readByte(HL.getValue));
}
void CPU::LD_A_B(){
	a->setValue(b->getValue);
}
void CPU::LD_A_C(){
	a->setValue(c->getValue);
}
void CPU::LD_A_D(){
	a->setValue(d->getValue);
}
void CPU::LD_A_d8(){
	a->setValue(
		memory->readByte(PC->getValue)
	);
	PC->inc();
}
void CPU::LD_A_E(){
	a->setValue(e->getValue());
}
void CPU::LD_A_H(){
	a->setValue(h->getValue());
}
void CPU::LD_A_L(){
	a->setValue(l->getValue());
}
void CPU::LD_a16_A(){
	memory->writeWord(get2Bytes(), a->getValue());
	PC->inc();
	PC->inc();
}
void CPU::LD_a16_SP(){
	memory->writeWord(get2Bytes(), SP->getValue());
	PC->inc();
	PC->inc();
}
void CPU::LD_aBC_A(){
	memory->writeByte(BC.getValue(), a->getValue());
}
void CPU::LD_aDE_A(){
	memory->writeByte(DE.getValue(), a->getValue());
}
void CPU::LD_aHL_A(){
	memory->writeByte(HL.getValue(), a->getValue());
}
void CPU::LD_aHL_B(){
	memory->writeByte(HL.getValue(), b->getValue());
}
void CPU::LD_aHL_C(){
	memory->writeByte(HL.getValue(), c->getValue());
}
void CPU::LD_aHL_D(){
	memory->writeByte(HL.getValue(), c->getValue());
}
void CPU::LD_aHL_d8(){
	memory->writeByte(HL.getValue(), memory->readByte(PC->getValue()+1));
	PC->inc();
}
void CPU::LD_aHL_E(){
	memory->writeByte(HL.getValue(), e->getValue());
}
void CPU::LD_aHL_H(){
	memory->writeByte(HL.getValue(), h->getValue());
}
void CPU::LD_aHL_L(){
	memory->writeByte(HL.getValue(), l->getValue());
}
void CPU::LD_B_A(){
	b->setValue(a->getValue);
}
void CPU::LD_B_aHL(){
	b->setValue(memory->readByte(HL.getValue()));
}
void CPU::LD_B_B(){
	b->setValue(b->getValue);
}
void CPU::LD_B_d8() {
	b->setValue(memory->readByte(PC->getValue() + 1));
	PC->inc();
}
void CPU::LD_B_C(){
	b->setValue(c->getValue);
}
void CPU::LD_B_D(){
	b->setValue(d->getValue);
}
void CPU::LD_B_E(){
	b->setValue(e->getValue);
}
void CPU::LD_B_H(){
	b->setValue(h->getValue);
}
void CPU::LD_B_L(){
	b->setValue(l->getValue);
}
void CPU::LD_BC_d16(){
	BC.setValue(memory->readWord(get2Bytes()));
	PC->inc();
	PC->inc();
}
void CPU::LD_C_A(){
	c->setValue(a->getValue);
}
void CPU::LD_C_aHL(){
	c->setValue(memory->readByte(HL.getValue()));
}
void CPU::LD_C_B(){
	c->setValue(b->getValue);
}
void CPU::LD_C_C(){
	c->setValue(c->getValue);
}
void CPU::LD_C_D(){
	c->setValue(d->getValue);
}
void CPU::LD_C_d8(){
	c->setValue(PC->getValue());
	PC->inc();
}
void CPU::LD_C_E(){
	c->setValue(e->getValue);
}
void CPU::LD_C_H(){
	c->setValue(h->getValue);
}
void CPU::LD_C_L(){
	c->setValue(l->getValue);
}
void CPU::LD_D_A(){
	d->setValue(a->getValue);
}
void CPU::LD_D_aHL(){ 
	d->setValue(memory->readByte(HL.getValue()));
}
void CPU::LD_D_B(){
	d->setValue(b->getValue);
}
void CPU::LD_D_C(){
	d->setValue(c->getValue);
}
void CPU::LD_D_D(){
	d->setValue(d->getValue);
}
void CPU::LD_D_d8(){
	d->setValue(memory->readByte(PC->getValue()+1));
	PC->inc();
}
void CPU::LD_D_E(){
	d->setValue(e->getValue);
}
void CPU::LD_D_H(){
	d->setValue(h->getValue);
}
void CPU::LD_D_L(){
	d->setValue(l->getValue);
}
void CPU::LD_DE_d16(){
	DE.setValue(memory->readWord(get2Bytes()));
	PC->inc();
	PC->inc();
}
void CPU::LD_E_A(){
	e->setValue(a->getValue);
}
void CPU::LD_E_aHL(){
	e->setValue(memory->readByte(HL.getValue()));
}
void CPU::LD_E_B(){
	e->setValue(b->getValue);
}
void CPU::LD_E_C(){
	e->setValue(c->getValue);
}
void CPU::LD_E_D(){
	e->setValue(d->getValue);
}
void CPU::LD_E_d8(){
	e->setValue(memory->readByte(PC->getValue()+1));
}
void CPU::LD_E_E(){
	e->setValue(e->getValue);
}
void CPU::LD_E_H(){
	e->setValue(h->getValue);
}
void CPU::LD_E_L(){
	e->setValue(l->getValue);
}
void CPU::LD_H_A(){
	h->setValue(a->getValue);
}
void CPU::LD_H_aHL(){
	h->setValue(memory->readByte(HL.getValue()));
}
void CPU::LD_H_B(){
	h->setValue(b->getValue);
}
void CPU::LD_H_C(){
	h->setValue(c->getValue);
}
void CPU::LD_H_D(){
	h->setValue(d->getValue);
}
void CPU::LD_H_d8(){
	h->setValue(memory->readByte(PC->getValue()+1));
	PC->inc();
}
void CPU::LD_H_E(){
	h->setValue(e->getValue);
}
void CPU::LD_H_H(){
	h->setValue(h->getValue);
}
void CPU::LD_H_L(){
	h->setValue(l->getValue);
}
void CPU::LD_HL_d16(){
	HL.setValue(memory->readWord(get2Bytes()));
	PC->inc();
	PC->inc();
}
void CPU::LD_HL_SPpr8(){
	int n = memory->readByte(PC->getValue());
	uint16_t result = SP->getValue() + n;
	CC->clearFlags();
	if ((result >> 8) != 0) {
		CC->setCarry();
	}
	if (((n & 0x0F) + (SP->getValue & 0x000F) & 0x10) != 0) {
		CC->setHalfCarry();
	}
	HL.setValue(result);
	PC->inc();
}
void CPU::LD_HLhi_A(){
	memory->writeByte(HL.getValue(), a->getValue());
	HL.inc();
}
void CPU::LD_HLlo_A(){
	memory->writeByte(HL.getValue(), a->getValue());
	HL.dec();
}
void CPU::LD_L_A(){
	l->setValue(a->getValue());
}
void CPU::LD_L_aHL(){
	l->setValue(memory->readByte(HL.getValue()));
}
void CPU::LD_L_B(){
	l->setValue(b->getValue());
}
void CPU::LD_L_C(){
	l->setValue(c->getValue());
}
void CPU::LD_L_D(){
	l->setValue(d->getValue());
}
void CPU::LD_L_d8(){
	l->setValue(memory->readByte(PC->getValue()));
}
void CPU::LD_L_E(){
	l->setValue(e->getValue());
}
void CPU::LD_L_H(){
	l->setValue(h->getValue());
}
void CPU::LD_L_L(){
	l->setValue(l->getValue());
}
void CPU::LD_SP_d16(){
	SP->setValue(memory->read2Byte(PC));
}
void CPU::LD_SP_HL(){
	SP->setValue(HL.getValue());
}
void CPU::LDH_A_a8(){
	a->setValue(memory->readByte(0xFF00 + memory->readByte(PC->getValue())));
}
void CPU::LDH_a8_A(){
	memory->writeByte(0xFF00 + memory->readByte(PC->getValue()), a->getValue());
}

void CPU::ADC_A_A(){
	ADC(a->getValue());
}
void CPU::ADC_A_aHL(){
	ADC(memory->readByte(HL.getValue()));
}
void CPU::ADC_A_B(){
	ADC(b->getValue());
}
void CPU::ADC_A_C(){
	ADC(c->getValue());
}
void CPU::ADC_A_D(){
	ADC(d->getValue());
}
void CPU::ADC_A_d8(){
	ADC(memory->readByte(PC->getValue()));
}
void CPU::ADC_A_E(){
	ADC(e->getValue());
}
void CPU::ADC_A_H(){
	ADC(h->getValue());
}
void CPU::ADC_A_L(){
	ADC(l->getValue());
}
void CPU::ADD_A_A(){
	ADD(a->getValue());
}
void CPU::ADD_A_aHL(){
	ADD(memory->readByte(HL.getValue()));
}
void CPU::ADD_A_B(){
	ADD(b->getValue());
}
void CPU::ADD_A_C(){
	ADD(c->getValue());
}
void CPU::ADD_A_D(){
	ADD(d->getValue());
}
void CPU::ADD_A_d8(){
	ADD(memory->readByte(PC->getValue()));
}
void CPU::ADD_A_E(){
	ADD(e->getValue());
}
void CPU::ADD_A_H(){
	ADD(h->getValue());
}
void CPU::ADD_A_L(){
	ADD(l->getValue());
}
void CPU::ADD_HL_BC(){
	ADD_HL(BC.getValue());
}
void CPU::ADD_HL_DE(){
	ADD_HL(DE.getValue());
}
void CPU::ADD_HL_HL(){
	ADD_HL(HL.getValue());
}
void CPU::LD_A_HLP()
{
	a->setValue(memory->readByte(HL.getValue()));
	HL.inc();
}
void CPU::ADD_HL_SP(){
	ADD_HL(SP->getValue());
}
void CPU::LD_A_HLD()
{
	a->setValue(memory->readByte(HL.getValue()));
	HL.dec();
}
void CPU::ADD_SP_r8(){
	ADD_SP(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::AND_A(){
	AND(a->getValue());
}
void CPU::AND_aHL(){
	AND(memory->readByte(HL.getValue()));
}
void CPU::AND_B(){
	AND(b->getValue());
}
void CPU::AND_C(){
	AND(c->getValue());
}
void CPU::AND_D(){
	AND(d->getValue());
}
void CPU::AND_d8(){
	AND(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::AND_E(){
	AND(e->getValue());
}
void CPU::AND_H(){
	AND(h->getValue());
}
void CPU::AND_L(){
	AND(l->getValue());
}
void CPU::BIT_0_A(){
	BIT(a->getValue(), 0);
}
void CPU::BIT_1_A() {
	BIT(a->getValue(), 1);
}
void CPU::BIT_2_A() {
	BIT(a->getValue(), 2);
}
void CPU::BIT_3_A() {
	BIT(a->getValue(), 3);
}
void CPU::BIT_4_A() {
	BIT(a->getValue(), 4);
}
void CPU::BIT_5_A() {
	BIT(a->getValue(), 5);
}
void CPU::BIT_6_A() {
	BIT(a->getValue(), 6);
}
void CPU::BIT_7_A() {
	BIT(a->getValue(), 7);
}


void CPU::BIT_0_B(){
	BIT(b->getValue(), 0);
}
void CPU::BIT_1_B() {
	BIT(b->getValue(), 1);
}
void CPU::BIT_2_B() {
	BIT(a->getValue(), 2);
}
void CPU::BIT_3_B() {
	BIT(b->getValue(), 3);
}
void CPU::BIT_4_B() {
	BIT(b->getValue(), 4);
}
void CPU::BIT_5_B() {
	BIT(b->getValue(), 5);
}
void CPU::BIT_6_B() {
	BIT(b->getValue(), 6);
}
void CPU::BIT_7_B() {
	BIT(b->getValue(), 7);
}



void CPU::BIT_0_C(){
	BIT(c->getValue(), 0);
}
void CPU::BIT_1_C() {
	BIT(c->getValue(), 1);
}
void CPU::BIT_2_C() {
	BIT(c->getValue(), 2);
}
void CPU::BIT_3_C() {
	BIT(c->getValue(), 3);
}
void CPU::BIT_4_C() {
	BIT(c->getValue(), 4);
}
void CPU::BIT_5_C() {
	BIT(c->getValue(), 5);
}
void CPU::BIT_6_C() {
	BIT(c->getValue(), 6);
}
void CPU::BIT_7_C() {
	BIT(c->getValue(), 7);
}

void CPU::BIT_0_D(){
	BIT(d->getValue(), 0);
}
void CPU::BIT_1_D() {
	BIT(d->getValue(), 1);
}
void CPU::BIT_2_D() {
	BIT(d->getValue(), 2);
}
void CPU::BIT_3_D() {
	BIT(d->getValue(), 3);
}
void CPU::BIT_4_D() {
	BIT(d->getValue(), 4);
}
void CPU::BIT_5_D() {
	BIT(d->getValue(), 5);
}
void CPU::BIT_6_D() {
	BIT(d->getValue(), 6);
}
void CPU::BIT_7_D() { 
	BIT(d->getValue(), 7); 
}

void CPU::BIT_0_E(){
	BIT(e->getValue(), 0);
}
void CPU::BIT_1_E() {
	BIT(e->getValue(), 1);
}
void CPU::BIT_2_E() {
	BIT(e->getValue(), 2);
}
void CPU::BIT_3_E() {
	BIT(e->getValue(), 3);
}
void CPU::BIT_4_E() {
	BIT(e->getValue(), 4);
}
void CPU::BIT_5_E() {
	BIT(e->getValue(), 5);
}
void CPU::BIT_6_E() {
	BIT(e->getValue(), 6);
}
void CPU::BIT_7_E() {
	BIT(e->getValue(), 7);
}

void CPU::BIT_0_H(){
	BIT(h->getValue(), 0);
}
void CPU::BIT_1_H() {
	BIT(h->getValue(), 1);
}
void CPU::BIT_2_H() {
	BIT(h->getValue(), 2);
}
void CPU::BIT_3_H() {
	BIT(h->getValue(), 3);
}
void CPU::BIT_4_H() {
	BIT(h->getValue(), 4);
}
void CPU::BIT_5_H() {
	BIT(h->getValue(), 5);
}
void CPU::BIT_6_H() {
	BIT(h->getValue(), 6);
}
void CPU::BIT_7_H() {
	BIT(h->getValue(), 7);
}

void CPU::BIT_0_L(){
	BIT(l->getValue(), 0);
}
void CPU::BIT_1_L() {
	BIT(l->getValue(), 1);
}
void CPU::BIT_2_L() {
	BIT(l->getValue(), 2);
}
void CPU::BIT_3_L() {
	BIT(l->getValue(), 3);
}
void CPU::BIT_4_L() {
	BIT(l->getValue(), 4);
}
void CPU::BIT_5_L() {
	BIT(l->getValue(), 5);
}
void CPU::BIT_6_L() {
	BIT(l->getValue(), 6);
}
void CPU::BIT_7_L() {
	BIT(l->getValue(), 7);
}


void CPU::BIT_0_aHL() {
	BIT(memory->readByte(HL.getValue()), 0);
}
void CPU::BIT_1_aHL(){
	BIT(memory->readByte(HL.getValue()), 1);
}
void CPU::BIT_2_aHL(){
	BIT(memory->readByte(HL.getValue()), 2);
}
void CPU::BIT_3_aHL(){
	BIT(memory->readByte(HL.getValue()), 3);
}
void CPU::BIT_4_aHL(){
	BIT(memory->readByte(HL.getValue()), 4);
}
void CPU::BIT_5_aHL(){
	BIT(memory->readByte(HL.getValue()), 5);
}
void CPU::BIT_6_aHL(){
	BIT(memory->readByte(HL.getValue()), 6);
}
void CPU::BIT_7_aHL(){
	BIT(memory->readByte(HL.getValue()), 7);
}

void CPU::CALL_a16(){
	CALL();
}
void CPU::CALL_C_a16(){
	if (CC->getCarry()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::CALL_NC_a16(){
	if (!CC->getCarry()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::CALL_NZ_a16(){
	if (!CC->getZero()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::CALL_Z_a16(){
	if (CC->getZero()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}

}
void CPU::CCF(){
	if (CC->getCarry()) {
		CC->clearCarry();
	}
	else {
		CC->setCarry();
	}
	CC->clearHalfCarry();
	CC->clearSub();
}
void CPU::CP_A(){
	CP(a->getValue());
}
void CPU::CP_aHL(){
	CP(memory->readByte(HL.getValue()));
}
void CPU::CP_B(){
	CP(b->getValue());
}
void CPU::CP_C(){
	CP(c->getValue());
}
void CPU::CP_D(){
	CP(d->getValue());
}
void CPU::CP_d8(){
	CP(memory->readByte(PC->getValue()));
}
void CPU::CP_E(){
	CP(e->getValue());
}
void CPU::CP_H(){
	CP(h->getValue());
}
void CPU::CP_L(){
	CP(l->getValue());
}
void CPU::CPL(){
	a->setValue(~a->getValue());
	CC->setSub();
	CC->setHalfCarry();
}

//gets DCB value of a
void CPU::DAA(){
	
	if (CC->getZero)
	{
		if (CC->getCarry()){ 
			a->setValue( a->getValue - 0x60); 
		}
		if (CC->getHalfCarry) { 
			a->setValue(a->getValue() - 0x06);
		}
	}
	else
	{
		if (CC->getCarry() || a->getValue() > 0x99) { 
			a->setValue(a->getValue() + 0x60);
			CC->setCarry();
		}
		if (CC->getHalfCarry() || (a->getValue() & 0x0F) > 0x09) { 
			a->setValue(a->getValue() + 0x06); 
		}
	}

	if (a->getValue() == 0) {
		CC->setZero();
	}
	CC->clearHalfCarry();
}
void CPU::DEC_A(){
	DEC8b(a);
}
void CPU::DEC_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	DEC8b(temp);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::DEC_B(){
	DEC8b(b);
}
void CPU::DEC_BC(){
	BC.dec();
}
void CPU::DEC_C(){
	DEC8b(c);
}
void CPU::DEC_D(){
	DEC8b(d);
}
void CPU::DEC_DE(){
	DE.dec();
}
void CPU::DEC_E(){
	DEC8b(e);
}
void CPU::DEC_H(){
	DEC8b(h);
}
void CPU::DEC_HL(){
	HL.dec();
}
void CPU::DEC_L(){
	DEC8b(l);
}
void CPU::DEC_SP(){
	SP->dec();
}
void CPU::DI(){
	memory->disableInterupt();
}
void CPU::EI(){
	memory->enableInterupt();
}
void CPU::HALT(){
//TODO
	//Pffffffffffftttt

}
void CPU::INC_A(){
	INC8b(a);
}
void CPU::INC_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	INC8b(temp);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::INC_B(){
	INC8b(b);
}
void CPU::INC_BC(){
	BC.inc();
}
void CPU::INC_C(){
	INC8b(c);
}
void CPU::INC_D(){
	INC8b(d);
}
void CPU::INC_DE(){
	DE.inc();
}
void CPU::INC_E(){
	INC8b(e);
}
void CPU::INC_H(){
	INC8b(h);
}
void CPU::INC_HL(){
	HL.inc();
}
void CPU::INC_L(){
	INC8b(l);
}
void CPU::INC_SP(){
	SP->inc();
}
void CPU::JP_a16(){
	PC->setValue(get2Bytes());
}
void CPU::JP_aHL(){
	PC->setValue(memory->readByte(HL.getValue()));
}
void CPU::JP_C_a16(){
	if (CC->getCarry()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JP_NC_a16(){
	if (!CC->getCarry()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JP_NZ_a16(){
	if (!CC->getZero()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JP_Z_a16(){
	if (CC->getZero()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JR_C_r8(){
	if (CC->getCarry()) {
		PC->setValue(PC->getValue() + 1 + ((uint8_t)memory->readByte(PC->getValue())));
		//branch = true
	}
	else {
		PC->inc;
	}
}
void CPU::JR_NC_r8(){
	if (!CC->getCarry()) {
		PC->setValue(PC->getValue() + 1 + ((uint8_t)memory->readByte(PC->getValue())));
		//branch = true
	}
	else {
		PC->inc;
	}
}
void CPU::JR_NZ_r8(){
	if (!CC->getZero()) {
		PC->setValue(PC->getValue() + 1 + ((uint8_t)memory->readByte(PC->getValue())));
		//branch = true
	}
	else {
		PC->inc;
	}
}
void CPU::JR_r8(){
	PC->setValue(PC->getValue() + 1 + ((uint8_t)memory->readByte(PC->getValue())));
}
void CPU::JR_Z_r8(){
	if (CC->getZero()) {
		PC->setValue(PC->getValue() + 1 + ((uint8_t)memory->readByte(PC->getValue())));
		//branch = true
	}
	else {
		PC->inc;
	}
}

void CPU::NOP(){}
void CPU::OR_A(){
	OR(a->getValue());
}
void CPU::OR_aHL(){
	OR(memory->readByte(HL.getValue()));
}
void CPU::OR_B(){
	OR(b->getValue());
}
void CPU::OR_C(){
	OR(c->getValue());
}
void CPU::OR_D(){
	OR(d->getValue());
}
void CPU::OR_d8(){
	OR(memory->readByte(PC->getValue()));
}
void CPU::OR_E(){
	OR(e->getValue());
}
void CPU::OR_H(){
	OR(h->getValue());
}
void CPU::OR_L(){
	OR(l->getValue());
}
void CPU::POP_AF(){
	POP(&AF);
}
void CPU::POP_BC(){
	POP(&BC);
}
void CPU::POP_DE(){
	POP(&DE);
}
void CPU::POP_HL(){
	POP(&HL);
}



/////////////////////////////////////////
void CPU::PREFIX_CB(){}
////////////////////////////////////////



void CPU::PUSH_AF(){
	PUSH(&AF);
}
void CPU::PUSH_BC(){
	PUSH(&BC);
}
void CPU::PUSH_DE(){
	PUSH(&DE);
}
void CPU::PUSH_HL(){
	PUSH(&HL);
}
void CPU::RES_0_A(){
	RES(a, 0);
}
void CPU::RES_1_A() {
	RES(a, 1);
}
void CPU::RES_2_A() {
	RES(a, 2);
}
void CPU::RES_3_A() {
	RES(a, 3);
}
void CPU::RES_4_A() {
	RES(a, 4);
}
void CPU::RES_5_A() {
	RES(a, 5);
}
void CPU::RES_6_A() {
	RES(a, 6);
}
void CPU::RES_7_A() {
	RES(a, 7);
}

void CPU::RES_0_B() {
	RES(b, 0);
}
void CPU::RES_1_B() {
	RES(b, 1);
}
void CPU::RES_2_B() {
	RES(b, 2);
}
void CPU::RES_3_B() {
	RES(b, 3);
}
void CPU::RES_4_B() {
	RES(b, 4);
}
void CPU::RES_5_B() {
	RES(b, 5);
}
void CPU::RES_6_B() {
	RES(b, 6);
}
void CPU::RES_7_B() {
	RES(b, 7);
}

void CPU::RES_0_C(){
	RES(c, 0);
}
void CPU::RES_1_C() {
	RES(c, 1);
}
void CPU::RES_2_C() {
	RES(c, 2);
}
void CPU::RES_3_C() {
	RES(c, 3);
}
void CPU::RES_4_C() {
	RES(c, 4);
}
void CPU::RES_5_C() {
	RES(c, 5);
}
void CPU::RES_6_C() {
	RES(c, 6);
}
void CPU::RES_7_C() {
	RES(c, 7);
}

void CPU::RES_0_D() {
	RES(d, 0);
}
void CPU::RES_1_D() {
	RES(d, 1);
}
void CPU::RES_2_D() {
	RES(d, 2);
}
void CPU::RES_3_D() {
	RES(d, 3);
}
void CPU::RES_4_D() {
	RES(d, 4);
}
void CPU::RES_5_D() {
	RES(d, 5);
}
void CPU::RES_6_D() {
	RES(d, 6);
}
void CPU::RES_7_D() {
	RES(d, 7);
}

void CPU::RES_0_E(){
	RES(e, 0);
}
void CPU::RES_1_E() {
	RES(e, 1);
}
void CPU::RES_2_E() {
	RES(e, 2);
}
void CPU::RES_3_E() {
	RES(e, 3);
}
void CPU::RES_4_E() {
	RES(e, 4);
}
void CPU::RES_5_E() {
	RES(e, 5);
}
void CPU::RES_6_E() {
	RES(e, 6);
}
void CPU::RES_7_E() {
	RES(e, 7);
}

void CPU::RES_0_H(){
	RES(h, 0);
}
void CPU::RES_1_H() {
	RES(h, 1);
}
void CPU::RES_2_H() {
	RES(h, 2);
}
void CPU::RES_3_H() {
	RES(h, 3);
}
void CPU::RES_4_H() {
	RES(h, 4);
}
void CPU::RES_5_H() {
	RES(h, 5);
}
void CPU::RES_6_H() {
	RES(h, 6);
}
void CPU::RES_7_H() {
	RES(h, 7);
}

void CPU::RES_0_L() {
	RES(l, 0);
}
void CPU::RES_1_L() {
	RES(l, 0);
}
void CPU::RES_2_L() {
	RES(l, 0);
}
void CPU::RES_3_L() {
	RES(l, 0);
}
void CPU::RES_4_L() {
	RES(l, 0);
}
void CPU::RES_5_L() {
	RES(l, 0);
}
void CPU::RES_6_L() {
	RES(l, 0);
}
void CPU::RES_7_L() {
	RES(l, 0);
}

void CPU::RES_0_aHL() {
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 0);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::RES_1_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 1);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::RES_2_aHL() {
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 2);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::RES_3_aHL() {
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 3);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::RES_4_aHL() {
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 4);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::RES_5_aHL() {
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 5);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::RES_6_aHL() {
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 6);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::RES_7_aHL() {
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	RES(temp, 7);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}

void CPU::RES_D3(){}
void CPU::RES_DB(){}
void CPU::RES_DD(){}
void CPU::RES_E3(){}
void CPU::RES_E4(){}
void CPU::RES_EA(){}
void CPU::RES_EB(){}
void CPU::RES_EC(){}
void CPU::RES_F4(){}
void CPU::RES_FC(){}
void CPU::RES_FD(){}


void CPU::RET(){
	POP(PC);
}
void CPU::RET_C(){
	if (CC->getCarry()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RET_NC(){
	if (!CC->getCarry()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RET_NZ(){
	if (!CC->getZero()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RET_Z(){
	if (CC->getZero()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RETI(){
	POP(PC);
	memory->enableInterupt();
}
void CPU::RL_A(){
	RL(a);
}
void CPU::RL_aHL(){}
void CPU::RL_B(){
	RL(b);
}
void CPU::RL_C(){
	RL(c);
}
void CPU::RL_D(){
	RL(d);
}
void CPU::RL_E(){
	RL(e);
}
void CPU::RL_H(){
	RL(h);
}
void CPU::RL_L(){
	RL(l);
}
void CPU::RLA(){
	RL(a);
}
void CPU::RLC_A(){
	RLC(a);
}
void CPU::RLC_aHL(){}
void CPU::RLC_B(){
	RLC(b);
}
void CPU::RLC_C(){
	RLC(c);
}
void CPU::RLC_D(){
	RLC(d);
}
void CPU::RLC_E(){
	RLC(e);
}
void CPU::RLC_H(){
	RLC(h);
}
void CPU::RLC_L(){
	RLC(l);
}
void CPU::RLCA(){
	RLC(a);
}
void CPU::RR_A(){
	RR(a);
}
void CPU::RR_aHL(){}
void CPU::RR_B(){
	RR(b);
}
void CPU::RR_C(){
	RR(c);
}
void CPU::RR_D(){
	RR(d);
}
void CPU::RR_E(){
	RR(e);
}
void CPU::RR_H(){
	RR(h);
}
void CPU::RR_L(){
	RR(l);
}
void CPU::RRA(){
	RR(a);
}
void CPU::RRC_A(){
	RRC(a);
}
void CPU::RRC_aHL(){}
void CPU::RRC_B(){
	RRC(b);
}
void CPU::RRC_C(){
	RRC(c);
}
void CPU::RRC_D(){
	RRC(d);
}
void CPU::RRC_E(){
	RRC(e);
}
void CPU::RRC_H(){
	RRC(h);
}
void CPU::RRC_L(){
	RRC(l);
}
void CPU::RRCA(){
	RRC(a);
}

void CPU::RST_00H(){
	PUSH(PC);
	PC->setValue(0x0000);
}
void CPU::RST_08H(){
	PUSH(PC);
	PC->setValue(0x0008);
}
void CPU::RST_10H(){
	PUSH(PC);
	PC->setValue(0x0010);
}
void CPU::RST_18H(){
	PUSH(PC);
	PC->setValue(0x0018);
}
void CPU::RST_20H(){
	PUSH(PC);
	PC->setValue(0x0020);
}
void CPU::RST_28H(){
	PUSH(PC);
	PC->setValue(0x0028);
}
void CPU::RST_30H(){
	PUSH(PC);
	PC->setValue(0x0030);
}
void CPU::RST_38H(){
	PUSH(PC);
	PC->setValue(0x0038);
}
void CPU::SBC_A_A(){
	SBC(a->getValue());
}
void CPU::SBC_A_aHL(){
	SBC(memory->readByte(HL.getValue()));
}
void CPU::SBC_A_B(){
	SBC(b->getValue());
}
void CPU::SBC_A_C(){
	SBC(c->getValue());
}
void CPU::SBC_A_D(){
	SBC(d->getValue());
}
void CPU::SBC_A_d8(){
	SBC(memory->readByte(PC->getValue()));
}
void CPU::SBC_A_E(){
	SBC(e->getValue());
}
void CPU::SBC_A_H(){
	SBC(h->getValue());
}
void CPU::SBC_A_L(){
	SBC(l->getValue());
}
void CPU::SCF(){
	CC->setCarry();
	CC->clearSub();
	CC->clearHalfCarry();
}

void CPU::SET_0_A(){
	SET(a, 0);
}
void CPU::SET_1_A() {
	SET(a, 1);
}
void CPU::SET_2_A() {
	SET(a, 2);
}
void CPU::SET_3_A() {
	SET(a, 3);
}
void CPU::SET_4_A() {
	SET(a, 4);
}
void CPU::SET_5_A() {
	SET(a, 5);
}
void CPU::SET_6_A() {
	SET(a, 6);
}
void CPU::SET_7_A() {
	SET(a, 7);
}

void CPU::SET_0_B() {
	SET(b, 0);
}
void CPU::SET_1_B() {
	SET(b, 1);
}
void CPU::SET_2_B() {
	SET(b, 2);
}
void CPU::SET_3_B() {
	SET(b, 3);
}
void CPU::SET_4_B() {
	SET(b, 4);
}
void CPU::SET_5_B() {
	SET(b, 5);
}
void CPU::SET_6_B() {
	SET(b, 6);
}
void CPU::SET_7_B() {
	SET(b, 7);
}

void CPU::SET_0_C() {
	SET(c, 0);
}
void CPU::SET_1_C() {
	SET(c, 1);
}
void CPU::SET_2_C() {
	SET(c, 2);
}
void CPU::SET_3_C() {
	SET(c, 3);
}
void CPU::SET_4_C() {
	SET(c, 4);
}
void CPU::SET_5_C() {
	SET(c, 5);
}
void CPU::SET_6_C() {
	SET(c, 6);
}
void CPU::SET_7_C() {
	SET(c, 7);
}

void CPU::SET_0_D() {
	SET(d, 0);
}
void CPU::SET_1_D() {
	SET(d, 1);
}
void CPU::SET_2_D() {
	SET(d, 2);
}
void CPU::SET_3_D() {
	SET(d, 3);
}
void CPU::SET_4_D() {
	SET(d, 4);
}
void CPU::SET_5_D() {
	SET(d, 5);
}
void CPU::SET_6_D() {
	SET(d, 6);
}
void CPU::SET_7_D() {
	SET(d, 7);
}

void CPU::SET_0_E() {
	SET(e, 0);
}
void CPU::SET_1_E() {
	SET(e, 1);
}
void CPU::SET_2_E() {
	SET(e, 2);
}
void CPU::SET_3_E() {
	SET(e, 3);
}
void CPU::SET_4_E() {
	SET(e, 4);
}
void CPU::SET_5_E() {
	SET(e, 5);
}
void CPU::SET_6_E() {
	SET(e, 6);
}
void CPU::SET_7_E() {
	SET(e, 7);
}

void CPU::SET_0_H(){
	SET(h, 0);
}
void CPU::SET_1_H() {
	SET(h, 0);
}
void CPU::SET_2_H() {
	SET(h, 0);
}
void CPU::SET_3_H() {
	SET(h, 0);
}
void CPU::SET_4_H() {
	SET(h, 0);
}
void CPU::SET_5_H() {
	SET(h, 0);
}
void CPU::SET_6_H() {
	SET(h, 0);
}
void CPU::SET_7_H() {
	SET(h, 0);
}

void CPU::SET_0_L() {
	SET(h, 0);
}
void CPU::SET_1_L() {
	SET(h, 1);
}
void CPU::SET_2_L() {
	SET(h, 2);
}
void CPU::SET_3_L() {
	SET(h, 3);
}
void CPU::SET_4_L() {
	SET(h, 4);
}
void CPU::SET_5_L() {
	SET(h, 5);
}
void CPU::SET_6_L() {
	SET(h, 6);
}
void CPU::SET_7_L() {
	SET(h, 7);
}

void CPU::SET_0_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 0);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SET_1_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 1);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SET_2_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 2);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SET_3_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 3);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SET_4_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 4);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SET_5_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 5);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SET_6_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 6);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SET_7_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SET(temp, 7);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}

void CPU::SLA_A(){
	SLA(a);
}
void CPU::SLA_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SLA(temp);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SLA_B(){
	SLA(b);
}
void CPU::SLA_C(){
	SLA(c);
}
void CPU::SLA_D(){
	SLA(d);
}
void CPU::SLA_E(){
	SLA(e);
}
void CPU::SLA_H(){
	SLA(h);
}
void CPU::SLA_L(){
	SLA(l);
}
void CPU::SRA_A(){
	SRA(a);
}
void CPU::SRA_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SRA(temp);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SRA_B(){
	SRA(b);
}
void CPU::SRA_C(){
	SRA(c);
}
void CPU::SRA_D(){
	SRA(d);
}
void CPU::SRA_E(){
	SRA(e);
}
void CPU::SRA_H(){
	SRA(h);
}
void CPU::SRA_L(){
	SRA(l);
}
void CPU::SRL_A(){ 
	SRL(a);
}
void CPU::SRL_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	SRL(temp);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::SRL_B(){
	SRL(b);
}
void CPU::SRL_C(){
	SRL(c);
}
void CPU::SRL_D(){
	SRL(d);
}
void CPU::SRL_E(){
	SRL(e);
}
void CPU::SRL_H(){
	SRL(h);
}
void CPU::SRL_L(){
	SRL(l);
}

///////
void CPU::STOP(){}


void CPU::SUB_A(){
	sub(a->getValue());
}
void CPU::SUB_aHL(){
	memory->readByte(HL.getValue());
}
void CPU::SUB_B(){
	sub(b->getValue());
}
void CPU::SUB_C(){
	sub(c->getValue());
}
void CPU::SUB_D(){
	sub(d->getValue());
}
void CPU::SUB_d8(){
	memory->readByte(PC->getValue());
}
void CPU::SUB_E(){
	sub(e->getValue());
}
void CPU::SUB_H(){
	sub(h->getValue());
}
void CPU::SUB_L(){
	sub(l->getValue());
}
void CPU::SWAP_A(){
	a->setValue(SWAP(a->getValue()));
}
void CPU::SWAP_aHL(){
	memory->writeByte(HL.getValue(), memory->readByte(HL.getValue()));
}
void CPU::SWAP_B(){
	b->setValue(SWAP(b->getValue()));
}
void CPU::SWAP_C(){
	c->setValue(SWAP(c->getValue()));
}
void CPU::SWAP_D(){
	d->setValue(SWAP(d->getValue()));
}
void CPU::SWAP_E(){
	e->setValue(SWAP(e->getValue()));
}
void CPU::SWAP_H(){
	h->setValue(SWAP(h->getValue()));
}
void CPU::SWAP_L(){
	l->setValue(SWAP(l->getValue()));
}
void CPU::XOR_A(){
	XOR(a);
}
void CPU::XOR_aHL(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(HL.getValue()));
	XOR(temp);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::XOR_B(){
	XOR(b);
}
void CPU::XOR_C(){
	XOR(c);
}
void CPU::XOR_D(){
	XOR(d);
}
void CPU::XOR_d8(){
	Register* temp = new Register();
	temp->setValue(memory->readByte(PC->getValue()));
	SRL(temp);
	memory->writeByte(HL.getValue(), temp->getValue());
	delete temp;
}
void CPU::XOR_E(){
	XOR(e);
}
void CPU::XOR_H(){
	XOR(h);
}
void CPU::XOR_L(){
	XOR(l);
}
