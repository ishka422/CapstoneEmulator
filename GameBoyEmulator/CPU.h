#include "mmu.h"
#include "Timer.h"
#include "cstdint";
#include <functional>
#include <map>
#include "Register16.h"
#include "ConditionCodes.h"

using namespace std;


class CPU
{
	mmu* memory;

	

public:

	Register* a = new Register();
	Register* b = new Register();
	Register* c = new Register();
	Register* d = new Register();
	Register* e = new Register();
	Register* h = new Register();
	Register* l = new Register();


	Register16 AF;
	Register16 BC;
	Register16 DE;
	Register16 HL;
	Register16* SP = new Register16();
	Register16* PC = new Register16();
	ConditionCodes* CC = new ConditionCodes();

	
	void load(Register* tar, uint8_t value);
	void ADD(uint8_t val);
	void sub(uint8_t val);
	void ADC(uint8_t value);
	void AND(uint8_t val);
	void OR(uint8_t val);
	void XOR(Register* reg);
	void SBC(uint8_t val);
	void ADD_HL(uint16_t);
	void ADD_SP(int8_t val);
	void CP(uint8_t val);
	void DEC8b(Register* reg);
	void INC8b(Register* reg);

	//CB Instructions
	void BIT(uint8_t val, int bit);
	void RES(Register* reg, int bit);
	void RL(Register* reg);
	void RLC(Register* reg);
	void RR(Register* reg);
	void RRC(Register* reg);
	void SET(Register* reg, int bit);
	void SLA(Register* reg);
	void SRA(Register* reg);
	void SRL(Register* reg);
	uint8_t SWAP(uint8_t val);


	//Stack instructions
	void PUSH(Register16* reg);
	void POP(Register16* reg);

	void CALL();
	CPU(mmu* memory);
	~CPU();


private:
	uint16_t get2Bytes();

	/////////////////////////////////////
	//Normal CPU Instructions


	//00
	void NOP();
	void LD_BC_d16();
	void LD_aBC_A();
	void INC_BC();
	void INC_B();
	void DEC_B();
	void LD_B_d8();
	void RLCA();
	void LD_a16_SP();
	void ADD_HL_BC();
	void LD_A_aBC();
	void DEC_BC();
	void INC_C();
	void DEC_C();
	void LD_C_d8();
	void RRCA();

	//10
	void STOP();
	void LD_DE_d16();
	void LD_aDE_A();
	void INC_DE();
	void INC_D();
	void DEC_D();
	void LD_D_d8();
	void RLA();
	void JR_r8();
	void ADD_HL_DE();
	void LD_A_aDE();
	void DEC_DE();
	void INC_E();
	void DEC_E();
	void LD_E_d8();
	void RRA();

	//20
	void JR_NZ_r8();
	void LD_HL_d16();
	void LD_HLhi_A();
	void INC_HL();
	void INC_H();
	void DEC_H();
	void LD_H_d8();
	void DAA();
	void JR_Z_r8();
	void ADD_HL_HL();
	void LD_A_HLP();
	void DEC_HL();
	void INC_L();
	void DEC_L();
	void LD_L_d8();
	void CPL();

	//30
	void JR_NC_r8();
	void LD_SP_d16();
	void LD_HLlo_A();
	void INC_SP();
	void INC_aHL();
	void DEC_aHL();
	void LD_aHL_d8();
	void SCF();
	void JR_C_r8();
	void ADD_HL_SP();
	void LD_A_HLD();
	void DEC_SP();
	void INC_A();
	void DEC_A();
	void LD_A_d8();
	void CCF();

	//40
	void LD_B_B();
	void LD_B_C();
	void LD_B_D();
	void LD_B_E();
	void LD_B_H();
	void LD_B_L();
	void LD_B_aHL();
	void LD_B_A();
	void LD_C_B(); 
	void LD_C_C();
	void LD_C_D();
	void LD_C_E();
	void LD_C_H();
	void LD_C_L();
	void LD_C_aHL();
	void LD_C_A();

	//50
	void LD_D_B();
	void LD_D_C();
	void LD_D_D();
	void LD_D_E();
	void LD_D_H();
	void LD_D_L();
	void LD_D_aHL();
	void LD_D_A();
	void LD_E_B();
	void LD_E_C();
	void LD_E_D();
	void LD_E_E();
	void LD_E_H();
	void LD_E_L();
	void LD_E_aHL();
	void LD_E_A();
	
	//60
	void LD_H_B();
	void LD_H_C();
	void LD_H_D();
	void LD_H_E();
	void LD_H_H();
	void LD_H_L();
	void LD_H_aHL();
	void LD_H_A();
	void LD_L_B();
	void LD_L_C();
	void LD_L_D();
	void LD_L_E();
	void LD_L_H();
	void LD_L_L();
	void LD_L_aHL();
	void LD_L_A();
	
	//70
	void LD_aHL_B();
	void LD_aHL_C();
	void LD_aHL_D();
	void LD_aHL_E();
	void LD_aHL_H();
	void LD_aHL_L();
	void HALT();
	void LD_aHL_A();
	void LD_A_B();
	void LD_A_C();
	void LD_A_D();
	void LD_A_E();
	void LD_A_H();
	void LD_A_L();
	void LD_A_aHL();
	void LD_A_A();

	//80
	void ADD_A_B();
	void ADD_A_C();
	void ADD_A_D();
	void ADD_A_E();
	void ADD_A_H();
	void ADD_A_L();
	void ADD_A_aHL();
	void ADD_A_A();
	void ADC_A_B();
	void ADC_A_C();
	void ADC_A_D();
	void ADC_A_E();
	void ADC_A_H();
	void ADC_A_L();
	void ADC_A_aHL();
	void ADC_A_A();

	//90
	void SUB_B();
	void SUB_C();
	void SUB_D();
	void SUB_E();
	void SUB_H();
	void SUB_L();
	void SUB_aHL();
	void SUB_A();
	void SBC_A_B();
	void SBC_A_C();
	void SBC_A_D();
	void SBC_A_E();
	void SBC_A_H();
	void SBC_A_L();
	void SBC_A_aHL();
	void SBC_A_A();

	//A0
	void AND_B();
	void AND_C();
	void AND_D();
	void AND_E();
	void AND_H();
	void AND_L();
	void AND_aHL();
	void AND_A();
	void XOR_B();
	void XOR_C();
	void XOR_D();
	void XOR_E();
	void XOR_H();
	void XOR_L();
	void XOR_aHL();
	void XOR_A();

	//B0
	void OR_B();
	void OR_C();
	void OR_D();
	void OR_E();
	void OR_H();
	void OR_L();
	void OR_aHL();
	void OR_A();
	void CP_B();
	void CP_C();
	void CP_D();
	void CP_E();
	void CP_H();
	void CP_L();
	void CP_aHL();
	void CP_A();
	
	//C0
	void RET_NZ();
	void POP_BC();
	void JP_NZ_a16();
	void JP_a16();
	void CALL_NZ_a16();
	void PUSH_BC();
	void ADD_A_d8();
	void RST_00H();
	void RET_Z();
	void RET();
	void JP_Z_a16();
	void PREFIX_CB();
	void CALL_Z_a16();
	void CALL_a16();
	void ADC_A_d8();
	void RST_08H();

	//D0
	void RET_NC();
	void POP_DE();
	void JP_NC_a16();
	void RES_D3();
	void CALL_NC_a16();
	void PUSH_DE();
	void SUB_d8();
	void RST_10H();
	void RET_C();
	void RETI();
	void JP_C_a16();
	void RES_DB();
	void CALL_C_a16();
	void RES_DD();
	void SBC_A_d8();
	void RST_18H();

	//E0
	void LDH_a8_A();
	void POP_HL();
	void LD_C_A();
	void RES_E3();
	void RES_E4();
	void PUSH_HL();
	void AND_d8();
	void RST_20H();
	void ADD_SP_r8();
	void JP_aHL();
	void LD_a16_A();
	void RES_EA();
	void RES_EB();
	void RES_EC();
	void XOR_d8();
	void RST_28H();

	//F0
	void LDH_A_a8();
	void POP_AF();
	void LA_A_C();
	void DI();
	void RES_F4();
	void PUSH_AF();
	void OR_d8();
	void RST_30H();
	void LD_HL_SPpr8();
	void LD_SP_HL();
	void LD_A_a16();
	void EI();
	void RES_FC();
	void RES_FD();
	void CP_d8();
	void RST_38H();
	
	//End of normal CPU instructions
	///////////////////////////////////////


	////////////////////////////////////
	//CB instructions

	//00
	void RLC_B();
	void RLC_C();
	void RLC_D();
	void RLC_E();
	void RLC_H();
	void RLC_L();
	void RLC_aHL();
	void RLC_A();
	void RRC_B();
	void RRC_C();
	void RRC_D();
	void RRC_E();
	void RRC_H();
	void RRC_L();
	void RRC_aHL();
	void RRC_A();

	//10
	void RL_B();
	void RL_C();
	void RL_D();
	void RL_E();
	void RL_H();
	void RL_L();
	void RL_aHL();
	void RL_A();
	void RR_B();
	void RR_C();
	void RR_D();
	void RR_E();
	void RR_H();
	void RR_L();
	void RR_aHL();
	void RR_A();

	//20
	void SLA_B();
	void SLA_C();
	void SLA_D();
	void SLA_E();
	void SLA_H();
	void SLA_L();
	void SLA_aHL();
	void SLA_A();
	void SRA_B();
	void SRA_C();
	void SRA_D();
	void SRA_E();
	void SRA_H();
	void SRA_L();
	void SRA_aHL();
	void SRA_A();

	//30
	void SWAP_B();
	void SWAP_C();
	void SWAP_D();
	void SWAP_E();
	void SWAP_H();
	void SWAP_L();
	void SWAP_aHL();
	void SWAP_A();
	void SRL_B();
	void SRL_C();
	void SRL_D();
	void SRL_E();
	void SRL_H();
	void SRL_L();
	void SRL_aHL();
	void SRL_A();

	//40
	void BIT_0_B();
	void BIT_0_C();
	void BIT_0_D();
	void BIT_0_E();
	void BIT_0_H();
	void BIT_0_L();
	void BIT_0_aHL();
	void BIT_0_A();
	void BIT_1_B();
	void BIT_1_C();
	void BIT_1_D();
	void BIT_1_E();
	void BIT_1_H();
	void BIT_1_L();
	void BIT_1_aHL();
	void BIT_1_A();

	//50
	void BIT_2_B();
	void BIT_2_C();
	void BIT_2_D();
	void BIT_2_E();
	void BIT_2_H();
	void BIT_2_L();
	void BIT_2_aHL();
	void BIT_2_A();
	void BIT_3_B();
	void BIT_3_C();
	void BIT_3_D();
	void BIT_3_E();
	void BIT_3_H();
	void BIT_3_L();
	void BIT_3_aHL();
	void BIT_3_A();

	//60
	void BIT_4_B();
	void BIT_4_C();
	void BIT_4_D();
	void BIT_4_E();
	void BIT_4_H();
	void BIT_4_L();
	void BIT_4_aHL();
	void BIT_4_A();
	void BIT_5_B();
	void BIT_5_C();
	void BIT_5_D();
	void BIT_5_E();
	void BIT_5_H();
	void BIT_5_L();
	void BIT_5_aHL();
	void BIT_5_A();

	//70
	void BIT_6_B();
	void BIT_6_C();
	void BIT_6_D();
	void BIT_6_E();
	void BIT_6_H();
	void BIT_6_L();
	void BIT_6_aHL();
	void BIT_6_A();
	void BIT_7_B();
	void BIT_7_C();
	void BIT_7_D();
	void BIT_7_E();
	void BIT_7_H();
	void BIT_7_L();
	void BIT_7_aHL();
	void BIT_7_A();

	//80
	void RES_0_B();
	void RES_0_C();
	void RES_0_D();
	void RES_0_E();
	void RES_0_H();
	void RES_0_L();
	void RES_0_aHL();
	void RES_0_A();
	void RES_1_B();
	void RES_1_C();
	void RES_1_D();
	void RES_1_E();
	void RES_1_H();
	void RES_1_L();
	void RES_1_aHL();
	void RES_1_A();

	//90
	void RES_2_B();
	void RES_2_C();
	void RES_2_D();
	void RES_2_E();
	void RES_2_H();
	void RES_2_L();
	void RES_2_aHL();
	void RES_2_A();
	void RES_3_B();
	void RES_3_C();
	void RES_3_D();
	void RES_3_E();
	void RES_3_H();
	void RES_3_L();
	void RES_3_aHL();
	void RES_3_A();

	//A0
	void RES_4_B();
	void RES_4_C();
	void RES_4_D();
	void RES_4_E();
	void RES_4_H();
	void RES_4_L();
	void RES_4_aHL();
	void RES_4_A();
	void RES_5_B();
	void RES_5_C();
	void RES_5_D();
	void RES_5_E();
	void RES_5_H();
	void RES_5_L();
	void RES_5_aHL();
	void RES_5_A();

	//B0
	void RES_6_B();
	void RES_6_C();
	void RES_6_D();
	void RES_6_E();
	void RES_6_H();
	void RES_6_L();
	void RES_6_aHL();
	void RES_6_A();
	void RES_7_B();
	void RES_7_C();
	void RES_7_D();
	void RES_7_E();
	void RES_7_H();
	void RES_7_L();
	void RES_7_aHL();
	void RES_7_A();

	//C0
	void SET_0_B();
	void SET_0_C();
	void SET_0_D();
	void SET_0_E();
	void SET_0_H();
	void SET_0_L();
	void SET_0_aHL();
	void SET_0_A();
	void SET_1_B();
	void SET_1_C();
	void SET_1_D();
	void SET_1_E();
	void SET_1_H();
	void SET_1_L();
	void SET_1_aHL();
	void SET_1_A();

	//D0
	void SET_2_B();
	void SET_2_C();
	void SET_2_D();
	void SET_2_E();
	void SET_2_H();
	void SET_2_L();
	void SET_2_aHL();
	void SET_2_A();
	void SET_3_B();
	void SET_3_C();
	void SET_3_D();
	void SET_3_E();
	void SET_3_H();
	void SET_3_L();
	void SET_3_aHL();
	void SET_3_A();

	//E0
	void SET_4_B();
	void SET_4_C();
	void SET_4_D();
	void SET_4_E();
	void SET_4_H();
	void SET_4_L();
	void SET_4_aHL();
	void SET_4_A();
	void SET_5_B();
	void SET_5_C();
	void SET_5_D();
	void SET_5_E();
	void SET_5_H();
	void SET_5_L();
	void SET_5_aHL();
	void SET_5_A();

	//F0
	void SET_6_B();
	void SET_6_C();
	void SET_6_D();
	void SET_6_E();
	void SET_6_H();
	void SET_6_L();
	void SET_6_aHL();
	void SET_6_A();
	void SET_7_B();
	void SET_7_C();
	void SET_7_D();
	void SET_7_E();
	void SET_7_H();
	void SET_7_L();
	void SET_7_aHL();
	void SET_7_A();


};

