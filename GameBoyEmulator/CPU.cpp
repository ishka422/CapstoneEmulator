#pragma once
#include "CPU.h"
#include "timings.h"
#include <iomanip>

CPU::CPU(MMU* memory)
{
	halted = false;
	int temp = 0;
	tCycles = &temp;
	mCycles = 0;
	cycles = 0;
	uint8_t temp2 = 0;
	opcode = &temp2;

	*tCycles = 1024;

	timerCycles = 0;
	divideRegister = 0;
	a = new Register();
	b = new Register();
	c = new Register();
	d = new Register();
	e = new Register();
	h = new Register();
	l = new Register();
	SP = new Register16();
	PC = new Register16();
	AF = new Register16();
	BC = new Register16();
	DE = new Register16();
	HL = new Register16();
	CC = new ConditionCodes();
	f = CC->getF();
	AF->setHighRegister(a);
	AF->setLowRegister(f);
	BC->setHighRegister(b);
	BC->setLowRegister(c);
	DE->setHighRegister(d);
	DE->setLowRegister(e);
	HL->setHighRegister(h);
	HL->setLowRegister(l);

	/*AF->setValue(0x01B0);
	BC->setValue(0x0013);
	DE->setValue(0x00D8);
	HL->setValue(0x014D);

	SP->setValue(0xFFFE);
	PC->setValue(0x0100);*/

	this->memory = memory;
	memory->setTCycles(tCycles);
	memory->setOpcode(opcode);
	now = clock();

	opcodes.insert({ 0x00, [this]() { NOP(); } });
	opcodes.insert({ 0x01, [this]() { LD_BC_d16(); } });
	opcodes.insert({ 0x02, [this]() { LD_aBC_A(); } });
	opcodes.insert({ 0x03, [this]() { INC_BC(); } });
	opcodes.insert({ 0x04, [this]() { INC_B(); } });
	opcodes.insert({ 0x05, [this]() { DEC_B(); } });
	opcodes.insert({ 0x06, [this]() { LD_B_d8(); } });
	opcodes.insert({ 0x07, [this]() { RLCA(); } });
	opcodes.insert({ 0x08, [this]() { LD_a16_SP(); } });
	opcodes.insert({ 0x09, [this]() { ADD_HL_BC(); } });
	opcodes.insert({ 0x0A, [this]() { LD_A_aBC(); } });
	opcodes.insert({ 0x0B, [this]() { DEC_BC(); } });
	opcodes.insert({ 0x0C, [this]() { INC_C(); } });
	opcodes.insert({ 0x0D, [this]() { DEC_C(); } });
	opcodes.insert({ 0x0E, [this]() { LD_C_d8(); } });
	opcodes.insert({ 0x0F, [this]() { RRCA(); } });
	opcodes.insert({ 0x10, [this]() { STOP(); } });
	opcodes.insert({ 0x11, [this]() { LD_DE_d16(); } });
	opcodes.insert({ 0x12, [this]() { LD_aDE_A(); } });
	opcodes.insert({ 0x13, [this]() { INC_DE(); } });
	opcodes.insert({ 0x14, [this]() { INC_D(); } });
	opcodes.insert({ 0x15, [this]() { DEC_D(); } });
	opcodes.insert({ 0x16, [this]() { LD_D_d8(); } });
	opcodes.insert({ 0x17, [this]() { RLA(); } });
	opcodes.insert({ 0x18, [this]() { JR_r8(); } });
	opcodes.insert({ 0x19, [this]() { ADD_HL_DE(); } });
	opcodes.insert({ 0x1A, [this]() { LD_A_aDE(); } });
	opcodes.insert({ 0x1B, [this]() { DEC_DE(); } });
	opcodes.insert({ 0x1C, [this]() { INC_E(); } });
	opcodes.insert({ 0x1D, [this]() { DEC_E(); } });
	opcodes.insert({ 0x1E, [this]() { LD_E_d8(); } });
	opcodes.insert({ 0x1F, [this]() { RRA(); } });
	opcodes.insert({ 0x20, [this]() { JR_NZ_r8(); } });
	opcodes.insert({ 0x21, [this]() { LD_HL_d16(); } });
	opcodes.insert({ 0x22, [this]() { LD_HLhi_A(); } });
	opcodes.insert({ 0x23, [this]() { INC_HL(); } });
	opcodes.insert({ 0x24, [this]() { INC_H(); } });
	opcodes.insert({ 0x25, [this]() { DEC_H(); } });
	opcodes.insert({ 0x26, [this]() { LD_H_d8(); } });
	opcodes.insert({ 0x27, [this]() { DAA(); } });
	opcodes.insert({ 0x28, [this]() { JR_Z_r8(); } });
	opcodes.insert({ 0x29, [this]() { ADD_HL_HL(); } });
	opcodes.insert({ 0x2A, [this]() { LD_A_HLP(); } });
	opcodes.insert({ 0x2B, [this]() { DEC_HL(); } });
	opcodes.insert({ 0x2C, [this]() { INC_L(); } });
	opcodes.insert({ 0x2D, [this]() { DEC_L(); } });
	opcodes.insert({ 0x2E, [this]() { LD_L_d8(); } });
	opcodes.insert({ 0x2F, [this]() { CPL(); } });
	opcodes.insert({ 0x30, [this]() { JR_NC_r8(); } });
	opcodes.insert({ 0x31, [this]() { LD_SP_d16(); } });
	opcodes.insert({ 0x32, [this]() { LD_HLlo_A(); } });
	opcodes.insert({ 0x33, [this]() { INC_SP(); } });
	opcodes.insert({ 0x34, [this]() { INC_aHL(); } });
	opcodes.insert({ 0x35, [this]() { DEC_aHL(); } });
	opcodes.insert({ 0x36, [this]() { LD_aHL_d8(); } });
	opcodes.insert({ 0x37, [this]() { SCF(); } });
	opcodes.insert({ 0x38, [this]() { JR_C_r8(); } });
	opcodes.insert({ 0x39, [this]() { ADD_HL_SP(); } });
	opcodes.insert({ 0x3A, [this]() { LD_A_HLD(); } });
	opcodes.insert({ 0x3B, [this]() { DEC_SP(); } });
	opcodes.insert({ 0x3C, [this]() { INC_A(); } });
	opcodes.insert({ 0x3D, [this]() { DEC_A(); } });
	opcodes.insert({ 0x3E, [this]() { LD_A_d8(); } });
	opcodes.insert({ 0x3F, [this]() { CCF(); } });
	opcodes.insert({ 0x40, [this]() { LD_B_B(); } });
	opcodes.insert({ 0x41, [this]() { LD_B_C(); } });
	opcodes.insert({ 0x42, [this]() { LD_B_D(); } });
	opcodes.insert({ 0x43, [this]() { LD_B_E(); } });
	opcodes.insert({ 0x44, [this]() { LD_B_H(); } });
	opcodes.insert({ 0x45, [this]() { LD_B_L(); } });
	opcodes.insert({ 0x46, [this]() { LD_B_aHL(); } });
	opcodes.insert({ 0x47, [this]() { LD_B_A(); } });
	opcodes.insert({ 0x48, [this]() { LD_C_B(); } });
	opcodes.insert({ 0x49, [this]() { LD_C_C(); } });
	opcodes.insert({ 0x4A, [this]() { LD_C_D(); } });
	opcodes.insert({ 0x4B, [this]() { LD_C_E(); } });
	opcodes.insert({ 0x4C, [this]() { LD_C_H(); } });
	opcodes.insert({ 0x4D, [this]() { LD_C_L(); } });
	opcodes.insert({ 0x4E, [this]() { LD_C_aHL(); } });
	opcodes.insert({ 0x4F, [this]() { LD_C_A(); } });
	opcodes.insert({ 0x50, [this]() { LD_D_B(); } });
	opcodes.insert({ 0x51, [this]() { LD_D_C(); } });
	opcodes.insert({ 0x52, [this]() { LD_D_D(); } });
	opcodes.insert({ 0x53, [this]() { LD_D_E(); } });
	opcodes.insert({ 0x54, [this]() { LD_D_H(); } });
	opcodes.insert({ 0x55, [this]() { LD_D_L(); } });
	opcodes.insert({ 0x56, [this]() { LD_D_aHL(); } });
	opcodes.insert({ 0x57, [this]() { LD_D_A(); } });
	opcodes.insert({ 0x58, [this]() { LD_E_B(); } });
	opcodes.insert({ 0x59, [this]() { LD_E_C(); } });
	opcodes.insert({ 0x5A, [this]() { LD_E_D(); } });
	opcodes.insert({ 0x5B, [this]() { LD_E_E(); } });
	opcodes.insert({ 0x5C, [this]() { LD_E_H(); } });
	opcodes.insert({ 0x5D, [this]() { LD_E_L(); } });
	opcodes.insert({ 0x5E, [this]() { LD_E_aHL(); } });
	opcodes.insert({ 0x5F, [this]() { LD_E_A(); } });
	opcodes.insert({ 0x60, [this]() { LD_H_B(); } });
	opcodes.insert({ 0x61, [this]() { LD_H_C(); } });
	opcodes.insert({ 0x62, [this]() { LD_H_D(); } });
	opcodes.insert({ 0x63, [this]() { LD_H_E(); } });
	opcodes.insert({ 0x64, [this]() { LD_H_H(); } });
	opcodes.insert({ 0x65, [this]() { LD_H_L(); } });
	opcodes.insert({ 0x66, [this]() { LD_H_aHL(); } });
	opcodes.insert({ 0x67, [this]() { LD_H_A(); } });
	opcodes.insert({ 0x68, [this]() { LD_L_B(); } });
	opcodes.insert({ 0x69, [this]() { LD_L_C(); } });
	opcodes.insert({ 0x6A, [this]() { LD_L_D(); } });
	opcodes.insert({ 0x6B, [this]() { LD_L_E(); } });
	opcodes.insert({ 0x6C, [this]() { LD_L_H(); } });
	opcodes.insert({ 0x6D, [this]() { LD_L_L(); } });
	opcodes.insert({ 0x6E, [this]() { LD_L_aHL(); } });
	opcodes.insert({ 0x6F, [this]() { LD_L_A(); } });
	opcodes.insert({ 0x70, [this]() { LD_aHL_B(); } });
	opcodes.insert({ 0x71, [this]() { LD_aHL_C(); } });
	opcodes.insert({ 0x72, [this]() { LD_aHL_D(); } });
	opcodes.insert({ 0x73, [this]() { LD_aHL_E(); } });
	opcodes.insert({ 0x74, [this]() { LD_aHL_H(); } });
	opcodes.insert({ 0x75, [this]() { LD_aHL_L(); } });
	opcodes.insert({ 0x76, [this]() { HALT(); } });
	opcodes.insert({ 0x77, [this]() { LD_aHL_A(); } });
	opcodes.insert({ 0x78, [this]() { LD_A_B(); } });
	opcodes.insert({ 0x79, [this]() { LD_A_C(); } });
	opcodes.insert({ 0x7A, [this]() { LD_A_D(); } });
	opcodes.insert({ 0x7B, [this]() { LD_A_E(); } });
	opcodes.insert({ 0x7C, [this]() { LD_A_H(); } });
	opcodes.insert({ 0x7D, [this]() { LD_A_L(); } });
	opcodes.insert({ 0x7E, [this]() { LD_A_aHL(); } });
	opcodes.insert({ 0x7F, [this]() { LD_A_A(); } });
	opcodes.insert({ 0x80, [this]() { ADD_A_B(); } });
	opcodes.insert({ 0x81, [this]() { ADD_A_C(); } });
	opcodes.insert({ 0x82, [this]() { ADD_A_D(); } });
	opcodes.insert({ 0x83, [this]() { ADD_A_E(); } });
	opcodes.insert({ 0x84, [this]() { ADD_A_H(); } });
	opcodes.insert({ 0x85, [this]() { ADD_A_L(); } });
	opcodes.insert({ 0x86, [this]() { ADD_A_aHL(); } });
	opcodes.insert({ 0x87, [this]() { ADD_A_A(); } });
	opcodes.insert({ 0x88, [this]() { ADC_A_B(); } });
	opcodes.insert({ 0x89, [this]() { ADC_A_C(); } });
	opcodes.insert({ 0x8A, [this]() { ADC_A_D(); } });
	opcodes.insert({ 0x8B, [this]() { ADC_A_E(); } });
	opcodes.insert({ 0x8C, [this]() { ADC_A_H(); } });
	opcodes.insert({ 0x8D, [this]() { ADC_A_L(); } });
	opcodes.insert({ 0x8E, [this]() { ADC_A_aHL(); } });
	opcodes.insert({ 0x8F, [this]() { ADC_A_A(); } });
	opcodes.insert({ 0x90, [this]() { SUB_B(); } });
	opcodes.insert({ 0x91, [this]() { SUB_C(); } });
	opcodes.insert({ 0x92, [this]() { SUB_D(); } });
	opcodes.insert({ 0x93, [this]() { SUB_E(); } });
	opcodes.insert({ 0x94, [this]() { SUB_H(); } });
	opcodes.insert({ 0x95, [this]() { SUB_L(); } });
	opcodes.insert({ 0x96, [this]() { SUB_aHL(); } });
	opcodes.insert({ 0x97, [this]() { SUB_A(); } });
	opcodes.insert({ 0x98, [this]() { SBC_A_B(); } });
	opcodes.insert({ 0x99, [this]() { SBC_A_C(); } });
	opcodes.insert({ 0x9A, [this]() { SBC_A_D(); } });
	opcodes.insert({ 0x9B, [this]() { SBC_A_E(); } });
	opcodes.insert({ 0x9C, [this]() { SBC_A_H(); } });
	opcodes.insert({ 0x9D, [this]() { SBC_A_L(); } });
	opcodes.insert({ 0x9E, [this]() { SBC_A_aHL(); } });
	opcodes.insert({ 0x9F, [this]() { SBC_A_A(); } });
	opcodes.insert({ 0xA0, [this]() { AND_B(); } });
	opcodes.insert({ 0xA1, [this]() { AND_C(); } });
	opcodes.insert({ 0xA2, [this]() { AND_D(); } });
	opcodes.insert({ 0xA3, [this]() { AND_E(); } });
	opcodes.insert({ 0xA4, [this]() { AND_H(); } });
	opcodes.insert({ 0xA5, [this]() { AND_L(); } });
	opcodes.insert({ 0xA6, [this]() { AND_aHL(); } });
	opcodes.insert({ 0xA7, [this]() { AND_A(); } });
	opcodes.insert({ 0xA8, [this]() { XOR_B(); } });
	opcodes.insert({ 0xA9, [this]() { XOR_C(); } });
	opcodes.insert({ 0xAA, [this]() { XOR_D(); } });
	opcodes.insert({ 0xAB, [this]() { XOR_E(); } });
	opcodes.insert({ 0xAC, [this]() { XOR_H(); } });
	opcodes.insert({ 0xAD, [this]() { XOR_L(); } });
	opcodes.insert({ 0xAE, [this]() { XOR_aHL(); } });
	opcodes.insert({ 0xAF, [this]() { XOR_A(); } });
	opcodes.insert({ 0xB0, [this]() { OR_B(); } });
	opcodes.insert({ 0xB1, [this]() { OR_C(); } });
	opcodes.insert({ 0xB2, [this]() { OR_D(); } });
	opcodes.insert({ 0xB3, [this]() { OR_E(); } });
	opcodes.insert({ 0xB4, [this]() { OR_H(); } });
	opcodes.insert({ 0xB5, [this]() { OR_L(); } });
	opcodes.insert({ 0xB6, [this]() { OR_aHL(); } });
	opcodes.insert({ 0xB7, [this]() { OR_A(); } });
	opcodes.insert({ 0xB8, [this]() { CP_B(); } });
	opcodes.insert({ 0xB9, [this]() { CP_C(); } });
	opcodes.insert({ 0xBA, [this]() { CP_D(); } });
	opcodes.insert({ 0xBB, [this]() { CP_E(); } });
	opcodes.insert({ 0xBC, [this]() { CP_H(); } });
	opcodes.insert({ 0xBD, [this]() { CP_L(); } });
	opcodes.insert({ 0xBE, [this]() { CP_aHL(); } });
	opcodes.insert({ 0xBF, [this]() { CP_A(); } });
	opcodes.insert({ 0xC0, [this]() { RET_NZ(); } });
	opcodes.insert({ 0xC1, [this]() { POP_BC(); } });
	opcodes.insert({ 0xC2, [this]() { JP_NZ_a16(); } });
	opcodes.insert({ 0xC3, [this]() { JP_a16(); } });
	opcodes.insert({ 0xC4, [this]() { CALL_NZ_a16(); } });
	opcodes.insert({ 0xC5, [this]() { PUSH_BC(); } });
	opcodes.insert({ 0xC6, [this]() { ADD_A_d8(); } });
	opcodes.insert({ 0xC7, [this]() { RST_00H(); } });
	opcodes.insert({ 0xC8, [this]() { RET_Z(); } });
	opcodes.insert({ 0xC9, [this]() { RET(); } });
	opcodes.insert({ 0xCA, [this]() { JP_Z_a16(); } });
	opcodes.insert({ 0xCB, [this]() { PREFIX_CB(); } });
	opcodes.insert({ 0xCC, [this]() { CALL_Z_a16(); } });
	opcodes.insert({ 0xCD, [this]() { CALL_a16(); } });
	opcodes.insert({ 0xCE, [this]() { ADC_A_d8(); } });
	opcodes.insert({ 0xCF, [this]() { RST_08H(); } });
	opcodes.insert({ 0xD0, [this]() { RET_NC(); } });
	opcodes.insert({ 0xD1, [this]() { POP_DE(); } });
	opcodes.insert({ 0xD2, [this]() { JP_NC_a16(); } });
	opcodes.insert({ 0xD3, [this]() { RES_D3(); } });
	opcodes.insert({ 0xD4, [this]() { CALL_NC_a16(); } });
	opcodes.insert({ 0xD5, [this]() { PUSH_DE(); } });
	opcodes.insert({ 0xD6, [this]() { SUB_d8(); } });
	opcodes.insert({ 0xD7, [this]() { RST_10H(); } });
	opcodes.insert({ 0xD8, [this]() { RET_C(); } });
	opcodes.insert({ 0xD9, [this]() { RETI(); } });
	opcodes.insert({ 0xDA, [this]() { JP_C_a16(); } });
	opcodes.insert({ 0xDB, [this]() { RES_DB(); } });
	opcodes.insert({ 0xDC, [this]() { CALL_C_a16(); } });
	opcodes.insert({ 0xDD, [this]() { RES_DD(); } });
	opcodes.insert({ 0xDE, [this]() { SBC_A_d8(); } });
	opcodes.insert({ 0xDF, [this]() { RST_18H(); } });
	opcodes.insert({ 0xE0, [this]() { LDH_a8_A(); } });
	opcodes.insert({ 0xE1, [this]() { POP_HL(); } });
	opcodes.insert({ 0xE2, [this]() { LD_Ca_A(); } });
	opcodes.insert({ 0xE3, [this]() { RES_E3(); } });
	opcodes.insert({ 0xE4, [this]() { RES_E4(); } });
	opcodes.insert({ 0xE5, [this]() { PUSH_HL(); } });
	opcodes.insert({ 0xE6, [this]() { AND_d8(); } });
	opcodes.insert({ 0xE7, [this]() { RST_20H(); } });
	opcodes.insert({ 0xE8, [this]() { ADD_SP_r8(); } });
	opcodes.insert({ 0xE9, [this]() { JP_aHL(); } });
	opcodes.insert({ 0xEA, [this]() { LD_a16_A(); } });
	opcodes.insert({ 0xEB, [this]() { RES_EA(); } });
	opcodes.insert({ 0xEC, [this]() { RES_EB(); } });
	opcodes.insert({ 0xED, [this]() { RES_EC(); } });
	opcodes.insert({ 0xEE, [this]() { XOR_d8(); } });
	opcodes.insert({ 0xEF, [this]() { RST_28H(); } });
	opcodes.insert({ 0xF0, [this]() { LDH_A_a8(); } });
	opcodes.insert({ 0xF1, [this]() { POP_AF(); } });
	opcodes.insert({ 0xF2, [this]() { LA_A_C(); } });
	opcodes.insert({ 0xF3, [this]() { DI(); } });
	opcodes.insert({ 0xF4, [this]() { RES_F4(); } });
	opcodes.insert({ 0xF5, [this]() { PUSH_AF(); } });
	opcodes.insert({ 0xF6, [this]() { OR_d8(); } });
	opcodes.insert({ 0xF7, [this]() { RST_30H(); } });
	opcodes.insert({ 0xF8, [this]() { LD_HL_SPpr8(); } });
	opcodes.insert({ 0xF9, [this]() { LD_SP_HL(); } });
	opcodes.insert({ 0xFA, [this]() { LD_A_a16(); } });
	opcodes.insert({ 0xFB, [this]() { EI(); } });
	opcodes.insert({ 0xFC, [this]() { RES_FC(); } });
	opcodes.insert({ 0xFD, [this]() { RES_FD(); } });
	opcodes.insert({ 0xFE, [this]() { CP_d8(); } });
	opcodes.insert({ 0xFF, [this]() { RST_38H(); } });

	CBopcodes.insert({ 0x00, [this]() {  RLC_B(); } });
	CBopcodes.insert({ 0x01, [this]() {  RLC_C(); } });
	CBopcodes.insert({ 0x02, [this]() {  RLC_D(); } });
	CBopcodes.insert({ 0x03, [this]() {  RLC_E(); } });
	CBopcodes.insert({ 0x04, [this]() {  RLC_H(); } });
	CBopcodes.insert({ 0x05, [this]() {  RLC_L(); } });
	CBopcodes.insert({ 0x06, [this]() {  RLC_aHL(); } });
	CBopcodes.insert({ 0x07, [this]() {  RLC_A(); } });
	CBopcodes.insert({ 0x08, [this]() {  RRC_B(); } });
	CBopcodes.insert({ 0x09, [this]() {  RRC_C(); } });
	CBopcodes.insert({ 0x0A, [this]() {  RRC_D(); } });
	CBopcodes.insert({ 0x0B, [this]() {  RRC_E(); } });
	CBopcodes.insert({ 0x0C, [this]() {  RRC_H(); } });
	CBopcodes.insert({ 0x0D, [this]() {  RRC_L(); } });
	CBopcodes.insert({ 0x0E, [this]() {  RRC_aHL(); } });
	CBopcodes.insert({ 0x0F, [this]() {  RRC_A(); } });
	CBopcodes.insert({ 0x10, [this]() {  RL_B(); } });
	CBopcodes.insert({ 0x11, [this]() {  RL_C(); } });
	CBopcodes.insert({ 0x12, [this]() {  RL_D(); } });
	CBopcodes.insert({ 0x13, [this]() {  RL_E(); } });
	CBopcodes.insert({ 0x14, [this]() {  RL_H(); } });
	CBopcodes.insert({ 0x15, [this]() {  RL_L(); } });
	CBopcodes.insert({ 0x16, [this]() {  RL_aHL(); } });
	CBopcodes.insert({ 0x17, [this]() {  RL_A(); } });
	CBopcodes.insert({ 0x18, [this]() {  RR_B(); } });
	CBopcodes.insert({ 0x19, [this]() {  RR_C(); } });
	CBopcodes.insert({ 0x1A, [this]() {  RR_D(); } });
	CBopcodes.insert({ 0x1B, [this]() {  RR_E(); } });
	CBopcodes.insert({ 0x1C, [this]() {  RR_H(); } });
	CBopcodes.insert({ 0x1D, [this]() {  RR_L(); } });
	CBopcodes.insert({ 0x1E, [this]() {  RR_aHL(); } });
	CBopcodes.insert({ 0x1F, [this]() {  RR_A(); } });
	CBopcodes.insert({ 0x20, [this]() {  SLA_B(); } });
	CBopcodes.insert({ 0x21, [this]() {  SLA_C(); } });
	CBopcodes.insert({ 0x22, [this]() {  SLA_D(); } });
	CBopcodes.insert({ 0x23, [this]() {  SLA_E(); } });
	CBopcodes.insert({ 0x24, [this]() {  SLA_H(); } });
	CBopcodes.insert({ 0x25, [this]() {  SLA_L(); } });
	CBopcodes.insert({ 0x26, [this]() {  SLA_aHL(); } });
	CBopcodes.insert({ 0x27, [this]() {  SLA_A(); } });
	CBopcodes.insert({ 0x28, [this]() {  SRA_B(); } });
	CBopcodes.insert({ 0x29, [this]() {  SRA_C(); } });
	CBopcodes.insert({ 0x2A, [this]() {  SRA_D(); } });
	CBopcodes.insert({ 0x2B, [this]() {  SRA_E(); } });
	CBopcodes.insert({ 0x2C, [this]() {  SRA_H(); } });
	CBopcodes.insert({ 0x2D, [this]() {  SRA_L(); } });
	CBopcodes.insert({ 0x2E, [this]() {  SRA_aHL(); } });
	CBopcodes.insert({ 0x2F, [this]() {  SRA_A(); } });
	CBopcodes.insert({ 0x30, [this]() {  SWAP_B(); } });
	CBopcodes.insert({ 0x31, [this]() {  SWAP_C(); } });
	CBopcodes.insert({ 0x32, [this]() {  SWAP_D(); } });
	CBopcodes.insert({ 0x33, [this]() {  SWAP_E(); } });
	CBopcodes.insert({ 0x34, [this]() {  SWAP_H(); } });
	CBopcodes.insert({ 0x35, [this]() {  SWAP_L(); } });
	CBopcodes.insert({ 0x36, [this]() {  SWAP_aHL(); } });
	CBopcodes.insert({ 0x37, [this]() {  SWAP_A(); } });
	CBopcodes.insert({ 0x38, [this]() {  SRL_B(); } });
	CBopcodes.insert({ 0x39, [this]() {  SRL_C(); } });
	CBopcodes.insert({ 0x3A, [this]() {  SRL_D(); } });
	CBopcodes.insert({ 0x3B, [this]() {  SRL_E(); } });
	CBopcodes.insert({ 0x3C, [this]() {  SRL_H(); } });
	CBopcodes.insert({ 0x3D, [this]() {  SRL_L(); } });
	CBopcodes.insert({ 0x3E, [this]() {  SRL_aHL(); } });
	CBopcodes.insert({ 0x3F, [this]() {  SRL_A(); } });
	CBopcodes.insert({ 0x40, [this]() {  BIT_0_B(); } });
	CBopcodes.insert({ 0x41, [this]() {  BIT_0_C(); } });
	CBopcodes.insert({ 0x42, [this]() {  BIT_0_D(); } });
	CBopcodes.insert({ 0x43, [this]() {  BIT_0_E(); } });
	CBopcodes.insert({ 0x44, [this]() {  BIT_0_H(); } });
	CBopcodes.insert({ 0x45, [this]() {  BIT_0_L(); } });
	CBopcodes.insert({ 0x46, [this]() {  BIT_0_aHL(); } });
	CBopcodes.insert({ 0x47, [this]() {  BIT_0_A(); } });
	CBopcodes.insert({ 0x48, [this]() {  BIT_1_B(); } });
	CBopcodes.insert({ 0x49, [this]() {  BIT_1_C(); } });
	CBopcodes.insert({ 0x4A, [this]() {  BIT_1_D(); } });
	CBopcodes.insert({ 0x4B, [this]() {  BIT_1_E(); } });
	CBopcodes.insert({ 0x4C, [this]() {  BIT_1_H(); } });
	CBopcodes.insert({ 0x4D, [this]() {  BIT_1_L(); } });
	CBopcodes.insert({ 0x4E, [this]() {  BIT_1_aHL(); } });
	CBopcodes.insert({ 0x4F, [this]() {  BIT_1_A(); } });
	CBopcodes.insert({ 0x50, [this]() {  BIT_2_B(); } });
	CBopcodes.insert({ 0x51, [this]() {  BIT_2_C(); } });
	CBopcodes.insert({ 0x52, [this]() {  BIT_2_D(); } });
	CBopcodes.insert({ 0x53, [this]() {  BIT_2_E(); } });
	CBopcodes.insert({ 0x54, [this]() {  BIT_2_H(); } });
	CBopcodes.insert({ 0x55, [this]() {  BIT_2_L(); } });
	CBopcodes.insert({ 0x56, [this]() {  BIT_2_aHL(); } });
	CBopcodes.insert({ 0x57, [this]() {  BIT_2_A(); } });
	CBopcodes.insert({ 0x58, [this]() {  BIT_3_B(); } });
	CBopcodes.insert({ 0x59, [this]() {  BIT_3_C(); } });
	CBopcodes.insert({ 0x5A, [this]() {  BIT_3_D(); } });
	CBopcodes.insert({ 0x5B, [this]() {  BIT_3_E(); } });
	CBopcodes.insert({ 0x5C, [this]() {  BIT_3_H(); } });
	CBopcodes.insert({ 0x5D, [this]() {  BIT_3_L(); } });
	CBopcodes.insert({ 0x5E, [this]() {  BIT_3_aHL(); } });
	CBopcodes.insert({ 0x5F, [this]() {  BIT_3_A(); } });
	CBopcodes.insert({ 0x60, [this]() {  BIT_4_B(); } });
	CBopcodes.insert({ 0x61, [this]() {  BIT_4_C(); } });
	CBopcodes.insert({ 0x62, [this]() {  BIT_4_D(); } });
	CBopcodes.insert({ 0x63, [this]() {  BIT_4_E(); } });
	CBopcodes.insert({ 0x64, [this]() {  BIT_4_H(); } });
	CBopcodes.insert({ 0x65, [this]() {  BIT_4_L(); } });
	CBopcodes.insert({ 0x66, [this]() {  BIT_4_aHL(); } });
	CBopcodes.insert({ 0x67, [this]() {  BIT_4_A(); } });
	CBopcodes.insert({ 0x68, [this]() {  BIT_5_B(); } });
	CBopcodes.insert({ 0x69, [this]() {  BIT_5_C(); } });
	CBopcodes.insert({ 0x6A, [this]() {  BIT_5_D(); } });
	CBopcodes.insert({ 0x6B, [this]() {  BIT_5_E(); } });
	CBopcodes.insert({ 0x6C, [this]() {  BIT_5_H(); } });
	CBopcodes.insert({ 0x6D, [this]() {  BIT_5_L(); } });
	CBopcodes.insert({ 0x6E, [this]() {  BIT_5_aHL(); } });
	CBopcodes.insert({ 0x6F, [this]() {  BIT_5_A(); } });
	CBopcodes.insert({ 0x70, [this]() {  BIT_6_B(); } });
	CBopcodes.insert({ 0x71, [this]() {  BIT_6_C(); } });
	CBopcodes.insert({ 0x72, [this]() {  BIT_6_D(); } });
	CBopcodes.insert({ 0x73, [this]() {  BIT_6_E(); } });
	CBopcodes.insert({ 0x74, [this]() {  BIT_6_H(); } });
	CBopcodes.insert({ 0x75, [this]() {  BIT_6_L(); } });
	CBopcodes.insert({ 0x76, [this]() {  BIT_6_aHL(); } });
	CBopcodes.insert({ 0x77, [this]() {  BIT_6_A(); } });
	CBopcodes.insert({ 0x78, [this]() {  BIT_7_B(); } });
	CBopcodes.insert({ 0x79, [this]() {  BIT_7_C(); } });
	CBopcodes.insert({ 0x7A, [this]() {  BIT_7_D(); } });
	CBopcodes.insert({ 0x7B, [this]() {  BIT_7_E(); } });
	CBopcodes.insert({ 0x7C, [this]() {  BIT_7_H(); } });
	CBopcodes.insert({ 0x7D, [this]() {  BIT_7_L(); } });
	CBopcodes.insert({ 0x7E, [this]() {  BIT_7_aHL(); } });
	CBopcodes.insert({ 0x7F, [this]() {  BIT_7_A(); } });
	CBopcodes.insert({ 0x80, [this]() {  RES_0_B(); } });
	CBopcodes.insert({ 0x81, [this]() {  RES_0_C(); } });
	CBopcodes.insert({ 0x82, [this]() {  RES_0_D(); } });
	CBopcodes.insert({ 0x83, [this]() {  RES_0_E(); } });
	CBopcodes.insert({ 0x84, [this]() {  RES_0_H(); } });
	CBopcodes.insert({ 0x85, [this]() {  RES_0_L(); } });
	CBopcodes.insert({ 0x86, [this]() {  RES_0_aHL(); } });
	CBopcodes.insert({ 0x87, [this]() {  RES_0_A(); } });
	CBopcodes.insert({ 0x88, [this]() {  RES_1_B(); } });
	CBopcodes.insert({ 0x89, [this]() {  RES_1_C(); } });
	CBopcodes.insert({ 0x8A, [this]() {  RES_1_D(); } });
	CBopcodes.insert({ 0x8B, [this]() {  RES_1_E(); } });
	CBopcodes.insert({ 0x8C, [this]() {  RES_1_H(); } });
	CBopcodes.insert({ 0x8D, [this]() {  RES_1_L(); } });
	CBopcodes.insert({ 0x8E, [this]() {  RES_1_aHL(); } });
	CBopcodes.insert({ 0x8F, [this]() {  RES_1_A(); } });
	CBopcodes.insert({ 0x90, [this]() {  RES_2_B(); } });
	CBopcodes.insert({ 0x91, [this]() {  RES_2_C(); } });
	CBopcodes.insert({ 0x92, [this]() {  RES_2_D(); } });
	CBopcodes.insert({ 0x93, [this]() {  RES_2_E(); } });
	CBopcodes.insert({ 0x94, [this]() {  RES_2_H(); } });
	CBopcodes.insert({ 0x95, [this]() {  RES_2_L(); } });
	CBopcodes.insert({ 0x96, [this]() {  RES_2_aHL(); } });
	CBopcodes.insert({ 0x97, [this]() {  RES_2_A(); } });
	CBopcodes.insert({ 0x98, [this]() {  RES_3_B(); } });
	CBopcodes.insert({ 0x99, [this]() {  RES_3_C(); } });
	CBopcodes.insert({ 0x9A, [this]() {  RES_3_D(); } });
	CBopcodes.insert({ 0x9B, [this]() {  RES_3_E(); } });
	CBopcodes.insert({ 0x9C, [this]() {  RES_3_H(); } });
	CBopcodes.insert({ 0x9D, [this]() {  RES_3_L(); } });
	CBopcodes.insert({ 0x9E, [this]() {  RES_3_aHL(); } });
	CBopcodes.insert({ 0x9F, [this]() {  RES_3_A(); } });
	CBopcodes.insert({ 0xA0, [this]() {  RES_4_B(); } });
	CBopcodes.insert({ 0xA1, [this]() {  RES_4_C(); } });
	CBopcodes.insert({ 0xA2, [this]() {  RES_4_D(); } });
	CBopcodes.insert({ 0xA3, [this]() {  RES_4_E(); } });
	CBopcodes.insert({ 0xA4, [this]() {  RES_4_H(); } });
	CBopcodes.insert({ 0xA5, [this]() {  RES_4_L(); } });
	CBopcodes.insert({ 0xA6, [this]() {  RES_4_aHL(); } });
	CBopcodes.insert({ 0xA7, [this]() {  RES_4_A(); } });
	CBopcodes.insert({ 0xA8, [this]() {  RES_5_B(); } });
	CBopcodes.insert({ 0xA9, [this]() {  RES_5_C(); } });
	CBopcodes.insert({ 0xAA, [this]() {  RES_5_D(); } });
	CBopcodes.insert({ 0xAB, [this]() {  RES_5_E(); } });
	CBopcodes.insert({ 0xAC, [this]() {  RES_5_H(); } });
	CBopcodes.insert({ 0xAD, [this]() {  RES_5_L(); } });
	CBopcodes.insert({ 0xAE, [this]() {  RES_5_aHL(); } });
	CBopcodes.insert({ 0xAF, [this]() {  RES_5_A(); } });
	CBopcodes.insert({ 0xB0, [this]() {  RES_6_B(); } });
	CBopcodes.insert({ 0xB1, [this]() {  RES_6_C(); } });
	CBopcodes.insert({ 0xB2, [this]() {  RES_6_D(); } });
	CBopcodes.insert({ 0xB3, [this]() {  RES_6_E(); } });
	CBopcodes.insert({ 0xB4, [this]() {  RES_6_H(); } });
	CBopcodes.insert({ 0xB5, [this]() {  RES_6_L(); } });
	CBopcodes.insert({ 0xB6, [this]() {  RES_6_aHL(); } });
	CBopcodes.insert({ 0xB7, [this]() {  RES_6_A(); } });
	CBopcodes.insert({ 0xB8, [this]() {  RES_7_B(); } });
	CBopcodes.insert({ 0xB9, [this]() {  RES_7_C(); } });
	CBopcodes.insert({ 0xBA, [this]() {  RES_7_D(); } });
	CBopcodes.insert({ 0xBB, [this]() {  RES_7_E(); } });
	CBopcodes.insert({ 0xBC, [this]() {  RES_7_H(); } });
	CBopcodes.insert({ 0xBD, [this]() {  RES_7_L(); } });
	CBopcodes.insert({ 0xBE, [this]() {  RES_7_aHL(); } });
	CBopcodes.insert({ 0xBF, [this]() {  RES_7_A(); } });
	CBopcodes.insert({ 0xC0, [this]() {  SET_0_B(); } });
	CBopcodes.insert({ 0xC1, [this]() {  SET_0_C(); } });
	CBopcodes.insert({ 0xC2, [this]() {  SET_0_D(); } });
	CBopcodes.insert({ 0xC3, [this]() {  SET_0_E(); } });
	CBopcodes.insert({ 0xC4, [this]() {  SET_0_H(); } });
	CBopcodes.insert({ 0xC5, [this]() {  SET_0_L(); } });
	CBopcodes.insert({ 0xC6, [this]() {  SET_0_aHL(); } });
	CBopcodes.insert({ 0xC7, [this]() {  SET_0_A(); } });
	CBopcodes.insert({ 0xC8, [this]() {  SET_1_B(); } });
	CBopcodes.insert({ 0xC9, [this]() {  SET_1_C(); } });
	CBopcodes.insert({ 0xCA, [this]() {  SET_1_D(); } });
	CBopcodes.insert({ 0xCB, [this]() {  SET_1_E(); } });
	CBopcodes.insert({ 0xCC, [this]() {  SET_1_H(); } });
	CBopcodes.insert({ 0xCD, [this]() {  SET_1_L(); } });
	CBopcodes.insert({ 0xCE, [this]() {  SET_1_aHL(); } });
	CBopcodes.insert({ 0xCF, [this]() {  SET_1_A(); } });
	CBopcodes.insert({ 0xD0, [this]() {  SET_2_B(); } });
	CBopcodes.insert({ 0xD1, [this]() {  SET_2_C(); } });
	CBopcodes.insert({ 0xD2, [this]() {  SET_2_D(); } });
	CBopcodes.insert({ 0xD3, [this]() {  SET_2_E(); } });
	CBopcodes.insert({ 0xD4, [this]() {  SET_2_H(); } });
	CBopcodes.insert({ 0xD5, [this]() {  SET_2_L(); } });
	CBopcodes.insert({ 0xD6, [this]() {  SET_2_aHL(); } });
	CBopcodes.insert({ 0xD7, [this]() {  SET_2_A(); } });
	CBopcodes.insert({ 0xD8, [this]() {  SET_3_B(); } });
	CBopcodes.insert({ 0xD9, [this]() {  SET_3_C(); } });
	CBopcodes.insert({ 0xDA, [this]() {  SET_3_D(); } });
	CBopcodes.insert({ 0xDB, [this]() {  SET_3_E(); } });
	CBopcodes.insert({ 0xDC, [this]() {  SET_3_H(); } });
	CBopcodes.insert({ 0xDD, [this]() {  SET_3_L(); } });
	CBopcodes.insert({ 0xDE, [this]() {  SET_3_aHL(); } });
	CBopcodes.insert({ 0xDF, [this]() {  SET_3_A(); } });
	CBopcodes.insert({ 0xE0, [this]() {  SET_4_B(); } });
	CBopcodes.insert({ 0xE1, [this]() {  SET_4_C(); } });
	CBopcodes.insert({ 0xE2, [this]() {  SET_4_D(); } });
	CBopcodes.insert({ 0xE3, [this]() {  SET_4_E(); } });
	CBopcodes.insert({ 0xE4, [this]() {  SET_4_H(); } });
	CBopcodes.insert({ 0xE5, [this]() {  SET_4_L(); } });
	CBopcodes.insert({ 0xE6, [this]() {  SET_4_aHL(); } });
	CBopcodes.insert({ 0xE7, [this]() {  SET_4_A(); } });
	CBopcodes.insert({ 0xE8, [this]() {  SET_5_B(); } });
	CBopcodes.insert({ 0xE9, [this]() {  SET_5_C(); } });
	CBopcodes.insert({ 0xEA, [this]() {  SET_5_D(); } });
	CBopcodes.insert({ 0xEB, [this]() {  SET_5_E(); } });
	CBopcodes.insert({ 0xEC, [this]() {  SET_5_H(); } });
	CBopcodes.insert({ 0xED, [this]() {  SET_5_L(); } });
	CBopcodes.insert({ 0xEE, [this]() {  SET_5_aHL(); } });
	CBopcodes.insert({ 0xEF, [this]() {  SET_5_A(); } });
	CBopcodes.insert({ 0xF0, [this]() {  SET_6_B(); } });
	CBopcodes.insert({ 0xF1, [this]() {  SET_6_C(); } });
	CBopcodes.insert({ 0xF2, [this]() {  SET_6_D(); } });
	CBopcodes.insert({ 0xF3, [this]() {  SET_6_E(); } });
	CBopcodes.insert({ 0xF4, [this]() {  SET_6_H(); } });
	CBopcodes.insert({ 0xF5, [this]() {  SET_6_L(); } });
	CBopcodes.insert({ 0xF6, [this]() {  SET_6_aHL(); } });
	CBopcodes.insert({ 0xF7, [this]() {  SET_6_A(); } });
	CBopcodes.insert({ 0xF8, [this]() {  SET_7_B(); } });
	CBopcodes.insert({ 0xF9, [this]() {  SET_7_C(); } });
	CBopcodes.insert({ 0xFA, [this]() {  SET_7_D(); } });
	CBopcodes.insert({ 0xFB, [this]() {  SET_7_E(); } });
	CBopcodes.insert({ 0xFC, [this]() {  SET_7_H(); } });
	CBopcodes.insert({ 0xFD, [this]() {  SET_7_L(); } });
	CBopcodes.insert({ 0xFE, [this]() {  SET_7_aHL(); } });
	CBopcodes.insert({ 0xFF, [this]() {  SET_7_A(); } });

	masterInterrupt = false;
}

void CPU::execute()
{
	*opcode = memory->readByte(PC->getValue());
	setCycles(*opcode);
	if (!halted) {
		PC->inc();

		opcodes[*opcode]();
	}
	else {
		cycles = 4; //basically a NOP
	}
	updateTimers();
}

void CPU::doInterrupt(int i, uint8_t req)
{
	halted = false;
	masterInterrupt = false;
	req &= (0xFF & ~(1 << i));
	memory->writeByte(0xFF0F, req);
	PUSH(PC);

	switch (i) {
	case 0:
		PC->setValue(0x40);
		break;
	case 1:
		PC->setValue(0x48);
		break;
	case 2:
		PC->setValue(0x50);
		break;
	case 3:
		PC->setValue(0x58);
		break;
	case 4:
		PC->setValue(0x60);
		break;
	}
}

void CPU::handleInterrupts()
{
	if (halted && memory->readByte(0xFF0F) != 0) {
		halted = false;
	}
	if (masterInterrupt) {
		uint8_t requestedInterrupts = memory->readByte(0xFF0F);
		uint8_t enabledInterrupts = memory->readByte(0xFFFF);
		if (requestedInterrupts > 0) {
			for (int i = 0; i < 5; i++) {
				if (((requestedInterrupts >> i) & 1) == 1) {
					if (((enabledInterrupts >> i) & 1) == 1) {
						doInterrupt(i, requestedInterrupts);
					}
				}
			}
		}
	}
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

uint8_t CPU::getOpcode()
{
	return *opcode;
}

void CPU::resetCycles()
{
	mCycles = 0;
}

void CPU::setCycles(uint8_t opcode)
{
	mCycles += machineCycles[opcode];
	cycles = machineCycles[opcode];
}

void CPU::addCBCycles(uint8_t opcode)
{
	mCycles += CBMachineCycles[opcode];
	cycles += CBMachineCycles[opcode];
}

void CPU::update()
{
	now = clock();
}

bool CPU::needScreenRefresh()
{
	clock_t diffTime = clock();
	double elapsed = (double)(diffTime - now) * 1000000.0 / CLOCKS_PER_SEC;
	if (elapsed > 1667.0) {
		now = clock();
		return true;
	}
	else {
		return false;
	}
}

void CPU::updateTimers()
{
	doDivideRegister();

	if (isClockEnabled()) {
		*tCycles -= cycles;
		if (*tCycles <= 0) {
			setClock();

			if (memory->readByte(TIMA) == 0xFF) {
				memory->writeByte(TIMA, memory->readByte(TMA));
				memory->requestInterupt(2);
			}
			else {
				memory->writeByte(TIMA, memory->readByte(TIMA) + 1);
			}
		}
	}
}

void CPU::doDivideRegister()
{
	divideRegister += cycles;
	if (divideRegister >= 255) {
		divideRegister = 0;
		memory->incDivider();
	}
}

bool CPU::isClockEnabled()
{
	return ((memory->readByte((TAC)) >> 2) & 1) == 1;
}

void CPU::setClock()
{
	uint8_t freq = memory->readByte(TAC) & 0x3;
	switch (freq)
	{
	case 0: *tCycles = 1024; break;
	case 1: *tCycles = 16; break;
	case 2: *tCycles = 64; break;
	case 3: *tCycles = 256; break;
	}
}
void CPU::RES_0_A() {
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

void CPU::RES_0_C() {
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

void CPU::RES_0_E() {
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

void CPU::RES_0_H() {
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
	RES(l, 1);
}
void CPU::RES_2_L() {
	RES(l, 2);
}
void CPU::RES_3_L() {
	RES(l, 3);
}
void CPU::RES_4_L() {
	RES(l, 4);
}
void CPU::RES_5_L() {
	RES(l, 5);
}
void CPU::RES_6_L() {
	RES(l, 6);
}
void CPU::RES_7_L() {
	RES(l, 7);
}

void CPU::RES_0_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 0);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RES_1_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 1);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RES_2_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 2);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RES_3_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 3);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RES_4_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 4);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RES_5_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 5);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RES_6_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 6);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RES_7_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RES(&temp, 7);
	memory->writeByte(HL->getValue(), temp.getValue());
}

void CPU::BIT_0_A() {
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

void CPU::BIT_0_B() {
	BIT(b->getValue(), 0);
}
void CPU::BIT_1_B() {
	BIT(b->getValue(), 1);
}
void CPU::BIT_2_B() {
	BIT(b->getValue(), 2);
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

void CPU::BIT_0_C() {
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

void CPU::BIT_0_D() {
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

void CPU::BIT_0_E() {
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

void CPU::BIT_0_H() {
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

void CPU::BIT_0_L() {
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
	BIT(memory->readByte(HL->getValue()), 0);
}
void CPU::BIT_1_aHL() {
	BIT(memory->readByte(HL->getValue()), 1);
}
void CPU::BIT_2_aHL() {
	BIT(memory->readByte(HL->getValue()), 2);
}
void CPU::BIT_3_aHL() {
	BIT(memory->readByte(HL->getValue()), 3);
}
void CPU::BIT_4_aHL() {
	BIT(memory->readByte(HL->getValue()), 4);
}
void CPU::BIT_5_aHL() {
	BIT(memory->readByte(HL->getValue()), 5);
}
void CPU::BIT_6_aHL() {
	BIT(memory->readByte(HL->getValue()), 6);
}
void CPU::BIT_7_aHL() {
	BIT(memory->readByte(HL->getValue()), 7);
}

void CPU::RLC_A() {
	RLC(a);
}
void CPU::RLC_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RLC(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RLC_B() {
	RLC(b);
}
void CPU::RLC_C() {
	RLC(c);
}
void CPU::RLC_D() {
	RLC(d);
}
void CPU::RLC_E() {
	RLC(e);
}
void CPU::RLC_H() {
	RLC(h);
}
void CPU::RLC_L() {
	RLC(l);
}

void CPU::RR_A() {
	RR(a);
}
void CPU::RR_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RR(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RR_B() {
	RR(b);
}
void CPU::RR_C() {
	RR(c);
}
void CPU::RR_D() {
	RR(d);
}
void CPU::RR_E() {
	RR(e);
}
void CPU::RR_H() {
	RR(h);
}
void CPU::RR_L() {
	RR(l);
}
void CPU::RRC_A() {
	RRC(a);
}
void CPU::RRC_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RRC(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RRC_B() {
	RRC(b);
}
void CPU::RRC_C() {
	RRC(c);
}
void CPU::RRC_D() {
	RRC(d);
}
void CPU::RRC_E() {
	RRC(e);
}
void CPU::RRC_H() {
	RRC(h);
}
void CPU::RRC_L() {
	RRC(l);
}
void CPU::SET_0_A() {
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

void CPU::SET_0_H() {
	SET(h, 0);
}
void CPU::SET_1_H() {
	SET(h, 1);
}
void CPU::SET_2_H() {
	SET(h, 2);
}
void CPU::SET_3_H() {
	SET(h, 3);
}
void CPU::SET_4_H() {
	SET(h, 4);
}
void CPU::SET_5_H() {
	SET(h, 5);
}
void CPU::SET_6_H() {
	SET(h, 6);
}
void CPU::SET_7_H() {
	SET(h, 7);
}

void CPU::SET_0_L() {
	SET(l, 0);
}
void CPU::SET_1_L() {
	SET(l, 1);
}
void CPU::SET_2_L() {
	SET(l, 2);
}
void CPU::SET_3_L() {
	SET(l, 3);
}
void CPU::SET_4_L() {
	SET(l, 4);
}
void CPU::SET_5_L() {
	SET(l, 5);
}
void CPU::SET_6_L() {
	SET(l, 6);
}
void CPU::SET_7_L() {
	SET(l, 7);
}

void CPU::SET_0_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 0);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SET_1_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 1);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SET_2_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 2);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SET_3_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 3);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SET_4_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 4);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SET_5_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 5);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SET_6_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 6);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SET_7_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SET(&temp, 7);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SLA_A() {
	SLA(a);
}
void CPU::SLA_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SLA(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SLA_B() {
	SLA(b);
}
void CPU::SLA_C() {
	SLA(c);
}
void CPU::SLA_D() {
	SLA(d);
}
void CPU::SLA_E() {
	SLA(e);
}
void CPU::SLA_H() {
	SLA(h);
}
void CPU::SLA_L() {
	SLA(l);
}
void CPU::SRA_A() {
	SRA(a);
}
void CPU::SRA_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SRA(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SRA_B() {
	SRA(b);
}
void CPU::SRA_C() {
	SRA(c);
}
void CPU::SRA_D() {
	SRA(d);
}
void CPU::SRA_E() {
	SRA(e);
}
void CPU::SRA_H() {
	SRA(h);
}
void CPU::SRA_L() {
	SRA(l);
}
void CPU::SRL_A() {
	SRL(a);
}
void CPU::SRL_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	SRL(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::SRL_B() {
	SRL(b);
}
void CPU::SRL_C() {
	SRL(c);
}
void CPU::SRL_D() {
	SRL(d);
}
void CPU::SRL_E() {
	SRL(e);
}
void CPU::SRL_H() {
	SRL(h);
}
void CPU::SRL_L() {
	SRL(l);
}
void CPU::SWAP_A() {
	a->setValue(SWAP(a->getValue()));
}
void CPU::SWAP_aHL() {
	memory->writeByte(HL->getValue(), SWAP(memory->readByte(HL->getValue())));
}
void CPU::SWAP_B() {
	b->setValue(SWAP(b->getValue()));
}
void CPU::SWAP_C() {
	c->setValue(SWAP(c->getValue()));
}
void CPU::SWAP_D() {
	d->setValue(SWAP(d->getValue()));
}
void CPU::SWAP_E() {
	e->setValue(SWAP(e->getValue()));
}
void CPU::SWAP_H() {
	h->setValue(SWAP(h->getValue()));
}
void CPU::SWAP_L() {
	l->setValue(SWAP(l->getValue()));
}
void CPU::RL_A() {
	RL(a);
}
void CPU::RL_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	RL(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::RL_B() {
	RL(b);
}
void CPU::RL_C() {
	RL(c);
}
void CPU::RL_D() {
	RL(d);
}
void CPU::RL_E() {
	RL(e);
}
void CPU::RL_H() {
	RL(h);
}
void CPU::RL_L() {
	RL(l);
}

void CPU::LA_A_C() {
	a->setValue(memory->readByte(0xFF00 + c->getValue()));
}
void CPU::LD_A_A() {
	a->setValue(a->getValue());
}
void CPU::LD_A_a16() {
	a->setValue(
		memory->readByte(get2Bytes())
	);
}
void CPU::LD_A_aBC() {
	a->setValue(memory->readByte(BC->getValue()));
}
void CPU::LD_A_aDE() {
	a->setValue(memory->readByte(DE->getValue()));
}
void CPU::LD_A_aHL() {
	a->setValue(memory->readByte(HL->getValue()));
}
void CPU::LD_A_B() {
	a->setValue(b->getValue());
}
void CPU::LD_A_C() {
	a->setValue(c->getValue());
}
void CPU::LD_A_D() {
	a->setValue(d->getValue());
}
void CPU::LD_A_d8() {
	a->setValue(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_A_E() {
	a->setValue(e->getValue());
}
void CPU::LD_A_H() {
	a->setValue(h->getValue());
}
void CPU::LD_A_L() {
	a->setValue(l->getValue());
}
void CPU::LD_a16_A() {
	Register16 temp;
	temp.setLow(memory->readByte(PC->getValue()));
	PC->inc();
	temp.setHigh(memory->readByte(PC->getValue()));
	PC->inc();
	memory->writeByte(temp.getValue(), a->getValue());
}
void CPU::LD_a16_SP() {
	uint16_t word = get2Bytes();
	memory->writeByte(word, SP->getLowReg()->getValue());
	memory->writeByte(word + 1, SP->getHighReg()->getValue());

	/*memory->writeByte(PC->getValue(), SP->getLowReg()->getValue());
	PC->inc();
	memory->writeByte(PC->getValue(), SP->getHighReg()->getValue());
	PC->inc();*/
}
void CPU::LD_aBC_A() {
	memory->writeByte(BC->getValue(), a->getValue());
}
void CPU::LD_aDE_A() {
	memory->writeByte(DE->getValue(), a->getValue());
}
void CPU::LD_aHL_A() {
	memory->writeByte(HL->getValue(), a->getValue());
}
void CPU::LD_aHL_B() {
	memory->writeByte(HL->getValue(), b->getValue());
}
void CPU::LD_aHL_C() {
	memory->writeByte(HL->getValue(), c->getValue());
}
void CPU::LD_aHL_D() {
	memory->writeByte(HL->getValue(), d->getValue());
}
void CPU::LD_aHL_d8() {
	memory->writeByte(HL->getValue(), memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_aHL_E() {
	memory->writeByte(HL->getValue(), e->getValue());
}
void CPU::LD_aHL_H() {
	memory->writeByte(HL->getValue(), h->getValue());
}
void CPU::LD_aHL_L() {
	memory->writeByte(HL->getValue(), l->getValue());
}
void CPU::LD_B_A() {
	b->setValue(a->getValue());
}
void CPU::LD_B_aHL() {
	b->setValue(memory->readByte(HL->getValue()));
}
void CPU::LD_B_B() {
	b->setValue(b->getValue());
}
void CPU::LD_B_d8() {
	b->setValue(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_B_C() {
	b->setValue(c->getValue());
}
void CPU::LD_B_D() {
	b->setValue(d->getValue());
}
void CPU::LD_B_E() {
	b->setValue(e->getValue());
}
void CPU::LD_B_H() {
	b->setValue(h->getValue());
}
void CPU::LD_B_L() {
	b->setValue(l->getValue());
}
void CPU::LD_BC_d16() {
	BC->setValue(memory->readWord(PC->getValue()));
	PC->inc();
	PC->inc();
}
void CPU::LD_C_A() {
	c->setValue(a->getValue());
}
void CPU::LD_C_aHL() {
	c->setValue(memory->readByte(HL->getValue()));
}
void CPU::LD_C_B() {
	c->setValue(b->getValue());
}
void CPU::LD_C_C() {
	c->setValue(c->getValue());
}
void CPU::LD_C_D() {
	c->setValue(d->getValue());
}
void CPU::LD_C_d8() {
	c->setValue(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_C_E() {
	c->setValue(e->getValue());
}
void CPU::LD_C_H() {
	c->setValue(h->getValue());
}
void CPU::LD_C_L() {
	c->setValue(l->getValue());
}
void CPU::LD_D_A() {
	d->setValue(a->getValue());
}
void CPU::LD_D_aHL() {
	d->setValue(memory->readByte(HL->getValue()));
}
void CPU::LD_D_B() {
	d->setValue(b->getValue());
}
void CPU::LD_D_C() {
	d->setValue(c->getValue());
}
void CPU::LD_D_D() {
	d->setValue(d->getValue());
}
void CPU::LD_D_d8() {
	d->setValue(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_D_E() {
	d->setValue(e->getValue());
}
void CPU::LD_D_H() {
	d->setValue(h->getValue());
}
void CPU::LD_D_L() {
	d->setValue(l->getValue());
}
void CPU::LD_DE_d16() {
	DE->setValue(memory->readWord(PC->getValue()));
	PC->inc();
	PC->inc();
}
void CPU::LD_E_A() {
	e->setValue(a->getValue());
}
void CPU::LD_E_aHL() {
	e->setValue(memory->readByte(HL->getValue()));
}
void CPU::LD_E_B() {
	e->setValue(b->getValue());
}
void CPU::LD_E_C() {
	e->setValue(c->getValue());
}
void CPU::LD_E_D() {
	e->setValue(d->getValue());
}
void CPU::LD_E_d8() {
	e->setValue(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_E_E() {
	e->setValue(e->getValue());
}
void CPU::LD_E_H() {
	e->setValue(h->getValue());
}
void CPU::LD_E_L() {
	e->setValue(l->getValue());
}
void CPU::LD_H_A() {
	h->setValue(a->getValue());
}
void CPU::LD_H_aHL() {
	h->setValue(memory->readByte(HL->getValue()));
}
void CPU::LD_H_B() {
	h->setValue(b->getValue());
}
void CPU::LD_H_C() {
	h->setValue(c->getValue());
}
void CPU::LD_H_D() {
	h->setValue(d->getValue());
}
void CPU::LD_H_d8() {
	h->setValue(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_H_E() {
	h->setValue(e->getValue());
}
void CPU::LD_H_H() {
	h->setValue(h->getValue());
}
void CPU::LD_H_L() {
	h->setValue(l->getValue());
}
void CPU::LD_HL_d16() {
	HL->setValue(memory->readWord(PC->getValue()));
	PC->inc();
	PC->inc();
}
void CPU::LD_HL_SPpr8() {
	char n = memory->readByte(PC->getValue());
	uint16_t result = SP->getValue() + n;
	CC->clearFlags();
	if (((SP->getValue() ^ n ^ result) & 0x100) == 0x100)
		CC->setCarry();
	if (((SP->getValue() ^ n ^ result) & 0x10) == 0x10)
		CC->setHalfCarry();
	HL->setValue(result);
	PC->inc();
}
void CPU::LD_HLhi_A() {
	memory->writeByte(HL->getValue(), a->getValue());
	HL->inc();
}
void CPU::LD_HLlo_A() {
	
	memory->writeByte(HL->getValue(), a->getValue());
	HL->dec();
}
void CPU::LD_L_A() {
	l->setValue(a->getValue());
}
void CPU::LD_L_aHL() {
	l->setValue(memory->readByte(HL->getValue()));
}
void CPU::LD_L_B() {
	l->setValue(b->getValue());
}
void CPU::LD_L_C() {
	l->setValue(c->getValue());
}
void CPU::LD_L_D() {
	l->setValue(d->getValue());
}
void CPU::LD_L_d8() {
	l->setValue(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::LD_L_E() {
	l->setValue(e->getValue());
}
void CPU::LD_L_H() {
	l->setValue(h->getValue());
}
void CPU::LD_L_L() {
	l->setValue(l->getValue());
}
void CPU::LD_SP_d16() {
	SP->setValue(get2Bytes());
}
void CPU::LD_SP_HL() {
	SP->setValue(HL->getValue());
}
void CPU::LDH_A_a8() {
	a->setValue(memory->readByte(0xFF00 + memory->readByte(PC->getValue())));
	PC->inc();
}
void CPU::LDH_a8_A() {
	memory->writeByte((uint16_t)(0xFF00 + memory->readByte(PC->getValue())), a->getValue());
	PC->inc();
}

void CPU::ADC_A_A() {
	ADC(a->getValue());
}
void CPU::ADC_A_aHL() {
	ADC(memory->readByte(HL->getValue()));
}
void CPU::ADC_A_B() {
	ADC(b->getValue());
}
void CPU::ADC_A_C() {
	ADC(c->getValue());
}
void CPU::ADC_A_D() {
	ADC(d->getValue());
}
void CPU::ADC_A_d8() {
	ADC(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::ADC_A_E() {
	ADC(e->getValue());
}
void CPU::ADC_A_H() {
	ADC(h->getValue());
}
void CPU::ADC_A_L() {
	ADC(l->getValue());
}
void CPU::ADD_A_A() {
	ADD(a->getValue());
}
void CPU::ADD_A_aHL() {
	ADD(memory->readByte(HL->getValue()));
}
void CPU::ADD_A_B() {
	ADD(b->getValue());
}
void CPU::ADD_A_C() {
	ADD(c->getValue());
}
void CPU::ADD_A_D() {
	ADD(d->getValue());
}
void CPU::ADD_A_d8() {
	ADD(memory->readByte(PC->getValue()));

	PC->inc();
}
void CPU::ADD_A_E() {
	ADD(e->getValue());
}
void CPU::ADD_A_H() {
	ADD(h->getValue());
}
void CPU::ADD_A_L() {
	ADD(l->getValue());
}
void CPU::ADD_HL_BC() {
	ADD_HL(BC->getValue());
}
void CPU::ADD_HL_DE() {
	ADD_HL(DE->getValue());
}
void CPU::ADD_HL_HL() {
	ADD_HL(HL->getValue());
}
void CPU::LD_A_HLP()
{
	a->setValue(memory->readByte(HL->getValue()));
	HL->inc();
}
void CPU::ADD_HL_SP() {
	ADD_HL(SP->getValue());
}
void CPU::LD_A_HLD()
{
	a->setValue(memory->readByte(HL->getValue()));
	HL->dec();
}
void CPU::ADD_SP_r8() {
	CC->clearFlags();
	uint16_t val = SP->getValue();
	char n = memory->readByte(PC->getValue());
	int result = val + n;
	if (((val ^ n ^ (result & 0xFFFF)) & 0x100) == 0x100) {
		CC->setCarry();
	}
	if (((val ^ n ^ (result & 0xFFFF)) & 0x10) == 0x10) {
		CC->setHalfCarry();
	}

	SP->setValue((uint16_t)(result));
	PC->inc();
}
void CPU::AND_A() {
	AND(a->getValue());
}
void CPU::AND_aHL() {
	AND(memory->readByte(HL->getValue()));
}
void CPU::AND_B() {
	AND(b->getValue());
}
void CPU::AND_C() {
	AND(c->getValue());
}
void CPU::AND_D() {
	AND(d->getValue());
}
void CPU::AND_d8() {
	AND(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::AND_E() {
	AND(e->getValue());
}
void CPU::AND_H() {
	AND(h->getValue());
}
void CPU::AND_L() {
	AND(l->getValue());
}

void CPU::CALL_a16() {
	CALL();
}
void CPU::CALL_C_a16() {
	if (CC->getCarry()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::CALL_NC_a16() {
	if (!CC->getCarry()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::CALL_NZ_a16() {
	if (!CC->getZero()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::CALL_Z_a16() {
	if (CC->getZero()) {
		CALL();
		//branch = true?
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::CCF() {
	if (CC->getCarry()) {
		CC->clearCarry();
	}
	else {
		CC->setCarry();
	}
	CC->clearHalfCarry();
	CC->clearSub();
}
void CPU::CP_A() {
	CP(a->getValue());
}
void CPU::CP_aHL() {
	CP(memory->readByte(HL->getValue()));
}
void CPU::CP_B() {
	CP(b->getValue());
}
void CPU::CP_C() {
	CP(c->getValue());
}
void CPU::CP_D() {
	CP(d->getValue());
}
void CPU::CP_d8() {
	CP(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::CP_E() {
	CP(e->getValue());
}
void CPU::CP_H() {
	CP(h->getValue());
}
void CPU::CP_L() {
	CP(l->getValue());
}
void CPU::CPL() {
	a->setValue(~a->getValue());
	CC->setSub();
	CC->setHalfCarry();
}

//gets DCB value of a
void CPU::DAA() {
	uint8_t aVal = a->getValue();
	if (!CC->getSub()) {
		if (CC->getCarry() || aVal > 0x99) {
			aVal += 0x60;
			CC->setCarry();
		}
		if (CC->getHalfCarry() || (aVal & 0xF) > 0x09) {
			aVal += 0x6;
		}
	}
	else {
		if (CC->getCarry()) {
			aVal -= 0x60;
		}
		if (CC->getHalfCarry()) {
			aVal -= 0x6;
		}
	}
	if (aVal == 0) {
		CC->setZero();
	}
	else {
		CC->clearZero();
	}
	CC->clearHalfCarry();
	a->setValue(aVal);
}
void CPU::DEC_A() {
	DEC8b(a);
}
void CPU::DEC_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	DEC8b(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::DEC_B() {
	DEC8b(b);
}
void CPU::DEC_BC() {
	BC->dec();
}
void CPU::DEC_C() {
	DEC8b(c);
}
void CPU::DEC_D() {
	DEC8b(d);
}
void CPU::DEC_DE() {
	DE->dec();
}
void CPU::DEC_E() {
	DEC8b(e);
}
void CPU::DEC_H() {
	DEC8b(h);
}
void CPU::DEC_HL() {
	HL->dec();
}
void CPU::DEC_L() {
	DEC8b(l);
}
void CPU::DEC_SP() {
	SP->dec();
}
void CPU::DI() {
	masterInterrupt = false;
}
void CPU::EI() {
	masterInterrupt = true;
}

void CPU::HALT() {
	halted = true;
}
void CPU::INC_A() {
	INC8b(a);
}
void CPU::INC_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	INC8b(&temp);
	memory->writeByte(HL->getValue(), temp.getValue());
}
void CPU::INC_B() {
	INC8b(b);
}
void CPU::INC_BC() {
	BC->inc();
}
void CPU::INC_C() {
	INC8b(c);
}
void CPU::INC_D() {
	INC8b(d);
}
void CPU::INC_DE() {
	DE->inc();
}
void CPU::INC_E() {
	INC8b(e);
}
void CPU::INC_H() {
	INC8b(h);
}
void CPU::INC_HL() {
	HL->inc();
}
void CPU::INC_L() {
	INC8b(l);
}
void CPU::INC_SP() {
	SP->inc();
}
void CPU::JP_a16() {
	uint16_t val = memory->readWord(PC->getValue());
	PC->setValue(val);
}
void CPU::JP_aHL() {
	PC->setValue(HL->getValue());
}
void CPU::JP_C_a16() {
	if (CC->getCarry()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JP_NC_a16() {
	if (!CC->getCarry()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JP_NZ_a16() {
	if (!CC->getZero()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JP_Z_a16() {
	if (CC->getZero()) {
		PC->setValue(get2Bytes());
		//brach = true
	}
	else {
		PC->inc();
		PC->inc();
	}
}
void CPU::JR_C_r8() {
	if (CC->getCarry()) {
		PC->setValue(PC->getValue() + 1 + (char)memory->readByte(PC->getValue()));
		//branch = true
	}
	else {
		PC->inc();
	}
}
void CPU::JR_NC_r8() {
	if (!CC->getCarry()) {
		PC->setValue(PC->getValue() + 1 + (char)memory->readByte(PC->getValue()));
		//branch = true
	}
	else {
		PC->inc();
	}
}
void CPU::JR_NZ_r8() {
	if (!CC->getZero()) {
		PC->setValue(PC->getValue() + 1 + (char)memory->readByte(PC->getValue()));

		//branch = true
	}
	else {
		PC->inc();
	}
}
void CPU::JR_r8() {
	PC->setValue(PC->getValue() + 1 + (char)memory->readByte(PC->getValue()));
}
void CPU::JR_Z_r8() {
	if (CC->getZero()) {
		PC->setValue(PC->getValue() + 1 + (char)memory->readByte(PC->getValue()));
		//branch = true
	}
	else {
		PC->inc();
	}
}

void CPU::NOP() {}
void CPU::OR_A() {
	OR(a->getValue());
}
void CPU::OR_aHL() {
	OR(memory->readByte(HL->getValue()));
}
void CPU::OR_B() {
	OR(b->getValue());
}
void CPU::OR_C() {
	OR(c->getValue());
}
void CPU::OR_D() {
	OR(d->getValue());
}
void CPU::OR_d8() {
	OR(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::OR_E() {
	OR(e->getValue());
}
void CPU::OR_H() {
	OR(h->getValue());
}
void CPU::OR_L() {
	OR(l->getValue());
}
void CPU::POP_AF() {
	POP(AF);
	f->setValue(f->getValue() & 0xF0);
}
void CPU::POP_BC() {
	POP(BC);
}
void CPU::POP_DE() {
	POP(DE);
}
void CPU::POP_HL() {
	POP(HL);
}

void CPU::LD_Ca_A()
{
	memory->writeByte(0xFF00 + c->getValue(), a->getValue());
}

void CPU::PREFIX_CB() {
	CBopcodes[memory->readByte(PC->getValue())]();
	addCBCycles(memory->readByte(PC->getValue()));
	PC->inc();
}

void CPU::PUSH_AF() {
	PUSH(AF);
}
void CPU::PUSH_BC() {
	PUSH(BC);
}
void CPU::PUSH_DE() {
	PUSH(DE);
}
void CPU::PUSH_HL() {
	PUSH(HL);
}

void CPU::RES_D3() {}
void CPU::RES_DB() {}
void CPU::RES_DD() {}
void CPU::RES_E3() {}
void CPU::RES_E4() {}
void CPU::RES_EA() {}
void CPU::RES_EB() {}
void CPU::RES_EC() {}
void CPU::RES_F4() {}
void CPU::RES_FC() {}
void CPU::RES_FD() {}

void CPU::RET() {
	POP(PC);
}
void CPU::RET_C() {
	if (CC->getCarry()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RET_NC() {
	if (!CC->getCarry()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RET_NZ() {
	if (!CC->getZero()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RET_Z() {
	if (CC->getZero()) {
		POP(PC);
		//branch = true
	}
}
void CPU::RETI() {
	POP(PC);
	masterInterrupt = true;
}
void CPU::RLA() {
	uint8_t carry = CC->getCarry() ? 1 : 0;
	uint8_t result = a -> getValue();
	CC->clearFlags();
	if ((result & 0x80) != 0) {
		CC->setCarry();
	}
	result <<= 1;
	result |= carry;
	a->setValue(result);
}

void CPU::RLCA() {
	uint8_t result = a->getValue();
	CC->clearFlags();
	if ((result & 0x80) != 0) {
		CC->setCarry();
		result <<= 1;
		result |= 0x1;
	}
	else {
		result <<= 1;
	}

	a->setValue(result);
}

void CPU::RRA() {

	bool carrySet = CC->getCarry();
	bool lsb = (a->getValue() & 1) == 1;

	CC->clearFlags();
	uint8_t result = a->getValue() >> 1;

	if (lsb) {
		CC->setCarry();
	}
	if (carrySet) {
		result |= (1 << 7);
	}
	a->setValue(result);
}

void CPU::RRCA() {
	uint8_t result = a->getValue();
	bool lsb = (result & 1) == 1;
	CC->clearFlags();

	result >>= 1;

	if (lsb) {
		CC->setCarry();
		result |= (1 << 7);
	}
	
	a->setValue(result);
}

void CPU::RST_00H() {
	PUSH(PC);
	PC->setValue(0x0000);
}
void CPU::RST_08H() {
	PUSH(PC);
	PC->setValue(0x0008);
}
void CPU::RST_10H() {
	PUSH(PC);
	PC->setValue(0x0010);
}
void CPU::RST_18H() {
	PUSH(PC);
	PC->setValue(0x0018);
}
void CPU::RST_20H() {
	PUSH(PC);
	PC->setValue(0x0020);
}
void CPU::RST_28H() {
	PUSH(PC);
	PC->setValue(0x0028);
}
void CPU::RST_30H() {
	PUSH(PC);
	PC->setValue(0x0030);
}
void CPU::RST_38H() {
	PUSH(PC);
	PC->setValue(0x0038);
}
void CPU::SBC_A_A() {
	SBC(a->getValue());
}
void CPU::SBC_A_aHL() {
	SBC(memory->readByte(HL->getValue()));
}
void CPU::SBC_A_B() {
	SBC(b->getValue());
}
void CPU::SBC_A_C() {
	SBC(c->getValue());
}
void CPU::SBC_A_D() {
	SBC(d->getValue());
}
void CPU::SBC_A_d8() {
	SBC(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::SBC_A_E() {
	SBC(e->getValue());
}
void CPU::SBC_A_H() {
	SBC(h->getValue());
}
void CPU::SBC_A_L() {
	SBC(l->getValue());
}
void CPU::SCF() {
	CC->setCarry();
	CC->clearSub();
	CC->clearHalfCarry();
}

///////
void CPU::STOP() {
	PC->inc();
}

void CPU::SUB_A() {
	sub(a->getValue());
}
void CPU::SUB_aHL() {
	sub(memory->readByte(HL->getValue()));
}
void CPU::SUB_B() {
	sub(b->getValue());
}
void CPU::SUB_C() {
	sub(c->getValue());
}
void CPU::SUB_D() {
	sub(d->getValue());
}
void CPU::SUB_d8() {
	sub(memory->readByte(PC->getValue()));
	PC->inc();
}
void CPU::SUB_E() {
	sub(e->getValue());
}
void CPU::SUB_H() {
	sub(h->getValue());
}
void CPU::SUB_L() {
	sub(l->getValue());
}

void CPU::XOR_A() {
	XOR(a);
}
void CPU::XOR_aHL() {
	Register temp;
	temp.setValue(memory->readByte(HL->getValue()));
	XOR(&temp);
}
void CPU::XOR_B() {
	XOR(b);
}
void CPU::XOR_C() {
	XOR(c);
}
void CPU::XOR_D() {
	XOR(d);
}
void CPU::XOR_d8() {
	Register temp;
	temp.setValue(memory->readByte(PC->getValue()));
	XOR(&temp);
	PC->inc();
}
void CPU::XOR_E() {
	XOR(e);
}
void CPU::XOR_H() {
	XOR(h);
}
void CPU::XOR_L() {
	XOR(l);
}