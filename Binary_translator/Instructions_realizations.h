#ifndef INSTRUCTIONS_TRANSLATION
#define INSTRUCTIONS_TRANSLATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum REGISTERS_NUMERATION
{
	RAX,
	RCX,
	RDX,
	RBX,
	RSP,
	RBP,
	RSI,
	RDI,

	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15
};

extern const int MAX_REGISTERS_NUMBER;

extern const int REG_REGADDR;
extern const int REG_REGADDR_8OFFSET;
extern const int REG_REGADDR_32OFFSET;
extern const int REG_REG;

extern const char REX;

extern const int REX_W;
extern const int REX_R;
extern const int REX_X;
extern const int REX_B;

extern const char ONE_ARG_EXTENTION[];

extern const unsigned char UNARY_EXTEND;

extern const unsigned char SHL_EXTEND; 

extern const unsigned char OPCODE[];

extern const char SHIFT_EXTENSIONS[];
extern const unsigned char JUMPS_EXTENDED_OPCODES[];

extern const size_t MAX_MESSAGE_LENGTH;

typedef enum OPCODE_NAMES
{
	CODE_MOV,
	CODE_MOV_FROM_MEM,
	CODE_MOV_IMM,
	CODE_PUSH_IMM,
	CODE_PUSH_REG,
	CODE_PUSH_MEM,
	CODE_POP_IMM,
	CODE_POP_REG,
	CODE_POP_MEM,
	CODE_ADD,
	CODE_SUB,
	CODE_MUL,
	CODE_DIV,
	CODE_JMP,
	CODE_JAE,
	CODE_JA,
	CODE_JB,
	CODE_JBE,
	CODE_JE,
	CODE_CALL,
	CODE_CMP_REGS,
	CODE_RET,
	CODE_NOP,
	CODE_SHL,
	CODE_SHR,
	CODE_INT
}OPCODE_NAMES;

#include "Translation.h"
#include "Translation_structures.h"


void making_stack_register_command(Code_state *state, int command_code, int register_number);
void making_command_mov_from_reg(Code_state *state, int register_number_from, int register_number_to);
void making_command_mov_imm(Code_state *state, int register_number_from, int register_number_to, int immediate);
void making_command_mov_from_mem(Code_state *state, int operation_code, int mod, int register_number_from, int register_number_to, int immediate);
void making_stack_imm_command(Code_state *state, int command_code, int value);
void making_simple_arithmetics(Code_state *state, int operation_code, int first_register, int second_register);
void making_complex_arithmetics(Code_state *state, int mode, int register_number);
void making_comparing(Code_state *state, int first_register, int second_register);
void making_command_jump(Code_state *state, int jump_length);
void making_conditional_jump(Code_state *state, int jump_number, int address);
void making_call(Code_state *state, int address);
void making_ret(Code_state *state);

void making_additional_stack_register_command(Code_state *state, int command_code, int register_number);
void making_additional_stack_imm_command(Code_state *state, int command_code, int value);
void making_additional_stack_register_address_in_register(Code_state *state, int command_code, int register_number);
void making_additional_stack_register_address(Code_state *state, int command_code, int mod, int value);
void making_additional_stack_register_command(Code_state *state, int command_code, int register_number);
void making_additional_stack_imm_command(Code_state *state, int command_code, int value);
void making_simple_arithmetics_with_additional_stack(Code_state *state, int operation_code);
void making_complex_arithmetics_with_additional_stack(Code_state *state, int mode);
void making_comparing_with_additional_stack(Code_state *state);

#endif
