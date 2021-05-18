#ifndef TRANSLATION_STRUCTURES_FILE
#define TRANSLATION_STRUCTURES_FILE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "Executable_elf.h"

#include "ReadingFromFile.h"
#include "Consts.h"

//#include "Instructions_realizations.h"

extern const size_t FILE_LENGTH;
extern const size_t TMP_DATA_SIZE;

extern const size_t stack_registers;
extern const size_t max_string_length;

// extern const char *registers_names[];

// extern const char *aasm_registers[];

// extern const char *simple_arithmetics[];

// extern const char *rax_arithmetics[];

// extern const char *jmp_instructions[];

//extern const char PRINTF[];


extern const unsigned char REGISTERS_NUMBERS[];
extern const unsigned char AASM_REGISTERS_NUMBERS[];

extern const int translation_table[];

extern const unsigned char MY_PRINTF_MACHINE_CODE[];
extern const unsigned char MY_SCANF_MACHINE_CODE[];
extern const unsigned char MY_HLT_MACHINE_CODE[];

extern const int PRINTF_SIZE;
extern const int SCANF_SIZE;
extern const int HLT_SIZE;

extern const int PRINTF_ADDRESS;
extern const int SCANF_ADDRESS;
extern const int HLT_ADDRESS;
extern const int START_JUMP;

typedef enum commands_names
{
    COMMAND_HLT,
    COMMAND_PUSH,
    COMMAND_ADD,
    COMMAND_SUB,
    COMMAND_MUL,
    COMMAND_OUT,
    COMMAND_SIN,
    COMMAND_COS,
    COMMAND_POP,
    COMMAND_SQRT,
    COMMAND_IN,
    COMMAND_DUMP,
    COMMAND_DIV,
    COMMAND_JMP,
    COMMAND_JAE,
    COMMAND_JA,
    COMMAND_JB,
    COMMAND_JBE,
    COMMAND_JE,
    COMMAND_JNE,
    COMMAND_JM,
    COMMAND_CALL,
    COMMAND_RET
} commands_names;

extern const int stack_push_offsets[];
extern const int stack_pop_offsets[];

enum Simple_list_state_signs
{
	SLIST_OK,
	SLIST_INVALID_POINTER,
	SLIST_MEMORY_ERROR
};

typedef struct Simple_list
{
	int *array;

	size_t array_length;
	size_t array_capacity;
} Simple_list;

enum Code_state_signs
{
	CODE_OK,
	CODE_INVALID_POINTER,
	CODE_MEMORY_ERROR,
	CODE_FILE_ERROR
};

typedef struct Code_state
{
	unsigned char *x86_code;
	char *aasm_code;

	int stop_place;

	size_t stack_counter;
	size_t stack_registers_counter;

	size_t aasm_code_length;
	size_t rip;

	size_t code_length;
	size_t code_capacity;

	//int passage_number;

	Simple_list *addresses_storage;
	size_t addresses_counter;

	Simple_list *label_addresses;
	Simple_list *jump_addresses;
} Code_state;

Simple_list 		   *simple_list_new(size_t array_length);
Simple_list_state_signs simple_list_construct(Simple_list *thus, size_t array_length);
Simple_list_state_signs simple_list_delete(Simple_list *thus);
Simple_list_state_signs simple_list_destruct(Simple_list *thus);
Simple_list_state_signs simple_list_insert(Simple_list *thus, int value);
Simple_list_state_signs simple_list_resize(Simple_list *thus);
Simple_list_state_signs simple_list_check_pointers(Simple_list *thus);

Code_state *translation_new(size_t length, const char *file_name);
Code_state_signs translation_construct(Code_state *state, size_t length, const char *file_name);
Code_state_signs translation_delete(Code_state *state, const char *file_name);
Code_state_signs translation_destruct(Code_state *state);
Code_state_signs translation_resize(Code_state *state);;
Code_state_signs translation_check_pointers(Code_state *state);
//char *get_type(Code_state *state, int mode);

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
	CODE_SHR
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

extern const unsigned char UNARY_EXTEND;

extern const unsigned char SHL_EXTEND; 

extern const unsigned char OPCODE[];
extern const unsigned char JUMPS_EXTENDED_OPCODES[];

void making_stack_register_command(Code_state *state, int command_code, int register_number);
void making_command_mov_from_reg(Code_state *state, int register_number_from, int register_number_to);
void making_command_mov_imm(Code_state *state, int register_number_from, int register_number_to, int immediate);
void making_command_mov_from_mem(Code_state *state, int operation_code, int mod, int register_number_from, int register_number_to, int immediate);
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
