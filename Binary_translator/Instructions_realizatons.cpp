#include "Instructions_realizations.h"

const int MAX_REGISTERS_NUMBER = 7;

const int REG_REGADDR 		   = 0;
const int REG_REGADDR_8OFFSET  = 1;
const int REG_REGADDR_32OFFSET = 2;
const int REG_REG 			   = 3;

const char REX = 4;

const int REX_W = 0b1000;
const int REX_R = 0b0100;
const int REX_X = 0b0010;
const int REX_B = 0b0001;

const unsigned char UNARY_EXTEND = 0b0011;
const unsigned char SHL_EXTEND = 0x4;

const char SHIFT_EXTENSIONS[]  = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x04, 0x05, 0};
const char ONE_ARG_EXTENTION[] = {0, 0, 0, 0, 0x04, 0, 0, 0, 0, 0, 0, 0, 0x06, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

extern const size_t MAX_MESSAGE_LENGTH = 10;

const unsigned char OPCODE[] = 
{
	0x89,//mov
	0x8b,//mov_from_memory
	0xc7,//mov_imm
	0x68,//push
	0x50,//push_reg
	0xff,//push_from_mem
	0x8f,//pop
	0x58,//pop_reg
	0x8f,//pop__to_mem
	0x01,//add
	0x29,//sub
	0xf7,//mul
	0xf7,//div
	0xe9,//jmp, rel32
	0x0f,//jae, rel32
	0x0f,//ja
	0x0f,//jb
	0x0f,//jbe
	0x0f,//je
	0xe8,//call
	0x39,//cmp two registers
	0xc3,//ret
	0x90,//nop
	0xc1,//shl
	0xc1,//shr
	0xcc //int 3
};

const unsigned char JUMPS_EXTENDED_OPCODES[] =
{
	0x83,
	0x87,
	0x82,
	0x86,
	0x84
};

const unsigned char STACK_COMMANDS_EXTENDED_OPCODES[] =
{
	0x00,
	0x00,
	0x06,
	0x00,
	0x00,
	0x00
};

void making_stack_register_command(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_REG || command_code == CODE_POP_REG);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (register_number > MAX_REGISTERS_NUMBER)
	{
		unsigned char WRXB = 0;
		WRXB |= REX_B;

		message[0] = (REX << 4) | WRXB; 
		copy_phrase(state, message, 1);
	}

	message[0] = OPCODE[command_code] + register_number % (MAX_REGISTERS_NUMBER + 1);
	copy_phrase(state, message, 1);
}
					
void making_additional_stack_register_command(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_REG || command_code == CODE_POP_REG);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = REX_W;

	if (command_code == CODE_POP_REG)
	{
		WRXB = REX_W;
		// message[0] = (REX << 4) | WRXB;
		// message[1] = OPCODE[CODE_PUSH_MEM];

		// message[2] = 0xcb;
		// copy_phrase(state, message, 3);
		making_command_mov_imm(state, RAX, R8, sizeof(long long));
		making_simple_arithmetics(state, CODE_SUB, R8, RBX);
	}

	if (register_number > MAX_REGISTERS_NUMBER)
	{
		WRXB |= REX_R;
	}

	message[0] = (REX << 4) | WRXB; 
	if (command_code == CODE_POP_REG)
	{
		message[1] = OPCODE[CODE_MOV_FROM_MEM];
	}
	else
	{
		message[1] = OPCODE[CODE_MOV];
	}
	message[2] = (REG_REGADDR << 6) |
				 (register_number % (MAX_REGISTERS_NUMBER + 1) << 3) |
				 4;
	message[3] = 0x1e;
	copy_phrase(state, message, 4);

	
	if (command_code != CODE_POP_REG)
	{
		// WRXB = REX_W;
		// message[0] = (REX << 4) | WRXB;
		// message[1] = OPCODE[CODE_PUSH_MEM];
		// //copy_phrase(state, message, 2);

		// message[2] = 0xc3;
		// copy_phrase(state, message, 3);
		making_command_mov_imm(state, RAX, R8, sizeof(long long));
		making_simple_arithmetics(state, CODE_ADD, R8, RBX);
	}

}

void making_stack_imm_command(Code_state *state, int command_code, int value)
{
	assert(command_code == CODE_PUSH_IMM || command_code == CODE_POP_IMM);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (command_code == CODE_PUSH_IMM)
	{
		message[0] = OPCODE[command_code];
		copy_phrase(state, message, 1);
		put_number(state, value);
	}
	else
	{
		unsigned char WRXB = 0;
		WRXB |= REX_B;

		message[0] = (REX << 4) | WRXB; 
		message[1] = OPCODE[CODE_POP_REG] + R11 % (MAX_REGISTERS_NUMBER + 1);

		copy_phrase(state, message, 2);
	}
}

void making_additional_stack_imm_command(Code_state *state, int command_code, int value)
{
	assert(command_code == CODE_PUSH_IMM || command_code == CODE_POP_IMM);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (command_code == CODE_PUSH_IMM)
	{
		making_command_mov_imm(state, RAX, RAX, value);//bad
		making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
	}
	else
	{
		making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	}
}

void making_stack_register_address(Code_state *state, int command_code, int mod, int value)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = OPCODE[command_code];
	message[1] = (mod << 6) |
				 (STACK_COMMANDS_EXTENDED_OPCODES[command_code - CODE_PUSH_IMM] << 3) |
				 RDI;

	copy_phrase(state, message, 2);

	if(mod == REG_REGADDR_32OFFSET)
		put_number(state, value);
}

void making_additional_stack_register_address(Code_state *state, int command_code, int mod, int value)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	making_additional_stack_register_command(state, command_code, RDI);
}

void making_stack_register_address_in_register(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);

	making_simple_arithmetics(state, CODE_ADD, RDI, register_number);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	if (register_number > MAX_REGISTERS_NUMBER)
	{
		unsigned char WRXB = REX_B;

		message[0] = (REX << 4) | WRXB;
		copy_phrase(state, message, 1);
	}
	
	message[0] = OPCODE[command_code];
	message[1] = (REG_REGADDR << 6) |
				 (STACK_COMMANDS_EXTENDED_OPCODES[command_code - CODE_PUSH_IMM] << 3) |
				 (register_number % (MAX_REGISTERS_NUMBER + 1));

	copy_phrase(state, message, 2);

	making_simple_arithmetics(state, CODE_SUB, RDI, register_number);
}

void making_left_shift(Code_state *state, int register_number, char shift)
{
	//48 c1 e0 03 shl rax, 3
	unsigned char WRXB = REX_W;
	if (register_number > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = (REX << 4) | WRXB;
	message[1] = OPCODE[CODE_SHL];
	message[2] = (UNARY_EXTEND << 6) | 
				 (SHIFT_EXTENSIONS[CODE_SHL] << 3) | 
				 (register_number % (MAX_REGISTERS_NUMBER + 1));
	message[3] = shift;
	copy_phrase(state, message, 4);
}

int making_valid_address_offset(Code_state *state, int register_number)
{
	making_command_mov_from_reg(state, register_number, R9);
	making_left_shift(state, R9, 3);

	return R9;
}

void making_additional_stack_register_address_in_register(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);

	int register_offset = making_valid_address_offset(state, register_number);

	making_simple_arithmetics(state, CODE_ADD, register_offset, RDI);
	if (command_code == CODE_PUSH_MEM)
		making_command_mov_from_mem(state, CODE_MOV_FROM_MEM, REG_REGADDR, RDI, RAX, 0);//bad

	// unsigned char message[3] = {};
	// if (register_number > MAX_REGISTERS_NUMBER)
	// {
	// 	unsigned char WRXB = REX_B;

	// 	message[0] = (REX << 4) | WRXB;
	// 	copy_phrase(state, message, 1);
	// }
	
	// message[0] = OPCODE[command_code];
	// message[1] = (REG_REGADDR << 6) |
	// 			 (STACK_COMMANDS_EXTENDED_OPCODES[command_code - CODE_PUSH_IMM] << 3) |
	// 			 (register_number % (MAX_REGISTERS_NUMBER + 1));
	// copy_phrase(state, message, 2);
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	if (command_code == CODE_PUSH_MEM)
	{
		making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
	}
	else
	{
		making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	}
	if (command_code == CODE_POP_MEM)
		making_command_mov_from_mem(state, CODE_MOV, REG_REGADDR, RDI, RAX, 0);

	making_simple_arithmetics(state, CODE_SUB, register_offset, RDI);
}

//mov register_to, register_from
void making_command_mov_from_reg(Code_state *state, int register_number_from, int register_number_to)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	if (register_number_from > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_MOV]; 
	message[2] = (REG_REG << 6) | 
				 (register_number_from % (MAX_REGISTERS_NUMBER + 1) << 3) | 
				 (register_number_to % (MAX_REGISTERS_NUMBER + 1));

	copy_phrase(state, message, 3);
}

void making_command_mov_imm(Code_state *state, int register_number_from, int register_number_to, int immediate)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;

	if (register_number_from > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_MOV_IMM]; 
	message[2] = (REG_REG << 6) | 
			 	 (register_number_from % (MAX_REGISTERS_NUMBER + 1) << 3) | 
			     (register_number_to % (MAX_REGISTERS_NUMBER + 1));
	
	copy_phrase(state, message, 3);
	
	put_number(state, immediate);
}

void making_command_mov_from_mem(Code_state *state, int operation_code, int mod, int register_number_from, int register_number_to, int immediate)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	if (register_number_from > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[operation_code]; 
	message[2] = (mod << 6) | 
			 	 (register_number_to % (MAX_REGISTERS_NUMBER + 1) << 3) | 
			     (register_number_from % (MAX_REGISTERS_NUMBER + 1));
	
	copy_phrase(state, message, 3);
	
	if (mod == REG_REGADDR_32OFFSET)
		put_number(state, immediate);
}

//add second_register, first_register
void making_simple_arithmetics(Code_state *state, int operation_code, int first_register, int second_register)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	
	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (first_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	if (second_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	message[0] = REX << 4 | WRXB; 
	message[1] = OPCODE[operation_code]; 
	message[2] = (REG_REG << 6) | 
				 (first_register % (MAX_REGISTERS_NUMBER + 1) << 3) | 
				 (second_register % (MAX_REGISTERS_NUMBER + 1));
	
	copy_phrase(state, message, 3);
}

void making_simple_arithmetics_with_stack(Code_state *state, int operation_code)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	making_stack_register_command(state, CODE_POP_REG, RAX);
	making_stack_register_command(state, CODE_POP_REG, RCX);

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[operation_code];
	message[2] = (REG_REG << 6) | 
				 (RCX << 3) | 
				 (RAX);
	copy_phrase(state, message, 3);

	making_stack_register_command(state, CODE_PUSH_REG, RAX);
}

void making_simple_arithmetics_with_additional_stack(Code_state *state, int operation_code)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	making_additional_stack_register_command(state, CODE_POP_REG, RCX);

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[operation_code];
	message[2] = (REG_REG << 6) | 
				 (RCX << 3) | 
				 (RAX);
	copy_phrase(state, message, 3);

	making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
}

void making_complex_arithmetics(Code_state *state, int mode, int register_number)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[translation_table[mode]]; 
	message[2] = (REG_REG << 6) | 
				 (ONE_ARG_EXTENTION[mode] << 3) |
				 (register_number % (MAX_REGISTERS_NUMBER + 1));
	copy_phrase(state, message, 3);
}

void making_complex_arithmetics_with_stack(Code_state *state, int mode)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	making_stack_register_command(state, CODE_POP_REG, RAX);
	making_stack_register_command(state, CODE_POP_REG, RCX);

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[translation_table[mode]]; 
	message[2] = (REG_REG << 6) | 
				 (ONE_ARG_EXTENTION[mode] << 3) |
				 (RCX);
	copy_phrase(state, message, 3);

	making_stack_register_command(state, CODE_PUSH_REG, RAX);
}

void making_complex_arithmetics_with_additional_stack(Code_state *state, int mode)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	making_additional_stack_register_command(state, CODE_POP_REG, RCX);

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[translation_table[mode]]; 
	message[2] = (REG_REG << 6) | 
				 (ONE_ARG_EXTENTION[mode] << 3) |
				 (RCX);
	copy_phrase(state, message, 3);

	making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
}

void making_comparing(Code_state *state, int first_register, int second_register)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (first_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_B;
	if (second_register > MAX_REGISTERS_NUMBER)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_CMP_REGS]; 
	message[2] = (3 << 6) | 
				 (first_register % (MAX_REGISTERS_NUMBER + 1) << 3) | 
				 (second_register % (MAX_REGISTERS_NUMBER + 1));
	
	copy_phrase(state, message, 3);
}

void making_comparing_with_stack(Code_state *state)
{
	making_stack_register_command(state, CODE_POP_REG, RAX);
	making_stack_register_command(state, CODE_POP_REG, RCX);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_CMP_REGS]; 
	message[2] = (0x3 << 6) | 
				 (RAX << 3) | 
				 (RCX);
	
	copy_phrase(state, message, 3);
}

void making_comparing_with_additional_stack(Code_state *state)
{
	making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	making_additional_stack_register_command(state, CODE_POP_REG, RCX);

	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_CMP_REGS]; 
	message[2] = (0x3 << 6) | 
				 (RAX << 3) | 
				 (RCX);
	
	copy_phrase(state, message, 3);
}


void making_command_jump(Code_state *state, int jump_length)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	message[0] = OPCODE[CODE_JMP];
	copy_phrase(state, message, 1);

	put_number(state, jump_length);
}

void making_conditional_jump(Code_state *state, int jump_number, int address)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = OPCODE[jump_number];
	message[1] = JUMPS_EXTENDED_OPCODES[jump_number - CODE_JAE];
	copy_phrase(state, message, 2);

	put_number(state, address);
}

void making_call(Code_state *state, int address)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};
	
	message[0] = OPCODE[CODE_CALL];
	copy_phrase(state, message, 1);

	put_number(state, address);
}

void making_ret(Code_state *state)
{
	unsigned char message[MAX_MESSAGE_LENGTH] = {};

	message[0] = OPCODE[CODE_RET];
	copy_phrase(state, message, 1);
}