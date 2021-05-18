#ifndef INSTRUCTIONS_TRANSLATION
#define INSTRUCTIONS_TRANSLATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Translation.h"

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

extern const int NEW_REGISTERS;

typedef enum OPCODE_NAMES
{
	CODE_MOV,
	CODE_MOV_FROM_MEM,
	CODE_MOV_IMM,
	CODE_PUSH_IMM,
	CODE_PUSH_REG,
	CODE_POP_IMM,
	CODE_POP_REG,
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
	CODE_NOP
}OPCODE_NAMES;

typedef struct Mod_reg
{
	char type;
	char mod;
	char reg;
	bool address_existing;
	size_t address;
}Mod_reg;

Mod_reg *reg_state_mode_new();
void reg_state_mode_delete(Mod_reg *thus);

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
extern const unsigned char OPCODE[];
extern const unsigned char JUMPS_EXTENDED_OPCODES[];

void making_stack_register_command(Code_state *state, int command_code, int register_number);
void making_command_mov_from_reg(Code_state *state, int register_number_from, int register_number_to);
void making_command_mov_imm(Code_state *state, int register_number_from, int register_number_to, int immediate);
void making_command_mov_from_mem(Code_state *state, int mod, int register_number_from, int register_number_to, int immediate);
void making_simple_arithmetics(Code_state *state, int operation_code, int first_register, int second_register);
void making_complex_arithmetics(Code_state *state, int mode, int register_number);
void making_comparing(Code_state *state, int first_register, int second_register);
void making_command_jump(Code_state *state, int jump_length);
void making_conditional_jump(Code_state *state, int jump_number, int address);
void making_call(Code_state *state, int address);
void making_ret(Code_state *state);

void copy_phrase(Code_state *state, const unsigned char *phrase, const size_t phrase_length);
void put_number(Code_state *state, const int value);

Mod_reg *get_type(Code_state *state, int mode);
void translation_process_push(Code_state *state);
void translation_process_pop(Code_state *state);

void translation_process_simple_arithmetics(Code_state *state, int mode);
void translation_process_complex_arithmetics(Code_state *state, int mode);

void translation_process_print(Code_state *state);
void translation_process_scan(Code_state *state);
void translation_process_hlt(Code_state *state);

bool is_conditional_jump(int instruction_index);
int extract_long_value(Code_state *state);
void translation_process_jump(Code_state *state, int jump_number);
void translation_process_call(Code_state *state);
void translation_process_ret(Code_state *state);

void translation_text_header(Code_state *state);
void translation_counting_addresses(Code_state *state);
void translation_parser_code(Code_state *state);

void translation_into_x86_64(const char *file_name, const char *output_file_name);

#endif
