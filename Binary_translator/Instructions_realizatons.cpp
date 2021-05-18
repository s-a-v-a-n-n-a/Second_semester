#include "Instructions_realizations.h"

const int NEW_REGISTERS = 7;

const int REG_REGADDR = 0;
const int REG_REGADDR_8OFFSET = 1;
const int REG_REGADDR_32OFFSET = 2;
const int REG_REG = 3;

const char REX = 4;

const int REX_W = 0b1000;
const int REX_R = 0b0100;
const int REX_X = 0b0010;
const int REX_B = 0b0001;

const char ONE_ARG_EXTENTION[] = {0, 0, 0, 0, 0x04, 0, 0, 0, 0, 0, 0, 0, 0x06, 0, 0, 0, 0, 0, 0, 0, 0};

const unsigned char OPCODE[] = 
{
	0x89,//mov
	0x8b,//mov_from_memory
	0xc7,//mov_imm
	0x06,//push
	0x50,//push_reg
	0x07,//pop
	0x58,//pop_reg
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
	0x90//nop
};

const unsigned char JUMPS_EXTENDED_OPCODES[] =
{
	0x83,
	0x87,
	0x82,
	0x86,
	0x84
};

void making_stack_register_command(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_REG || command_code == CODE_POP_REG);

	unsigned char message[3] = {};

	if (register_number > NEW_REGISTERS)
	{
		unsigned char WRXB = 0;
		WRXB |= REX_B;

		message[0] = (REX << 4) | WRXB; 
		copy_phrase(state, message, 1);
	}

	message[0] = OPCODE[command_code] + register_number % (NEW_REGISTERS + 1);
	copy_phrase(state, message, 1);
}

void making_command_mov_from_reg(Code_state *state, int register_number_from, int register_number_to)
{
	unsigned char message[3] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > NEW_REGISTERS)
		WRXB |= REX_B;
	if (register_number_from > NEW_REGISTERS)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_MOV]; 
	message[2] = (REG_REG << 6) | 
				 (register_number_from % (NEW_REGISTERS + 1) << 3) | 
				 (register_number_to % (NEW_REGISTERS + 1));

	copy_phrase(state, message, 3);
}

void making_command_mov_imm(Code_state *state, int register_number_from, int register_number_to, int immediate)
{
	unsigned char message[3] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > NEW_REGISTERS)
		WRXB |= REX_B;

	if (register_number_from > NEW_REGISTERS)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_MOV_IMM]; 
	message[2] = (REG_REG << 6) | 
			 	 (register_number_from % (NEW_REGISTERS + 1) << 3) | 
			     (register_number_to % (NEW_REGISTERS + 1));
	
	copy_phrase(state, message, 3);
	
	put_number(state, immediate);
}

void making_command_mov_from_mem(Code_state *state, int mod, int register_number_from, int register_number_to, int immediate)
{
	unsigned char message[3] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > NEW_REGISTERS)
		WRXB |= REX_R;
	if (register_number_from > NEW_REGISTERS)
		WRXB |= REX_B;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_MOV_FROM_MEM]; 
	message[2] = (mod << 6) | 
			 	 (register_number_to % (NEW_REGISTERS + 1) << 3) | 
			     (register_number_from % (NEW_REGISTERS + 1));
	
	copy_phrase(state, message, 3);
	
	if (mod == REG_REGADDR_32OFFSET)
		put_number(state, immediate);
}

void making_simple_arithmetics(Code_state *state, int operation_code, int first_register, int second_register)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);

	unsigned char message[3] = {};
	
	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (first_register > NEW_REGISTERS)
		WRXB |= REX_B;
	if (second_register > NEW_REGISTERS)
		WRXB |= REX_R;
	message[0] = REX << 4 | WRXB; 
	message[1] = OPCODE[operation_code]; 
	message[2] = (REG_REG << 6) | 
				 (first_register % (NEW_REGISTERS + 1) << 3) | 
				 (second_register % (NEW_REGISTERS + 1));
	
	copy_phrase(state, message, 3);
}

void making_complex_arithmetics(Code_state *state, int mode, int register_number)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);

	unsigned char message[3] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;

	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[translation_table[mode]]; 
	message[2] = (REG_REG << 6) | 
				 (ONE_ARG_EXTENTION[mode] << 3) |
				 (register_number % (NEW_REGISTERS + 1));
	copy_phrase(state, message, 3);
}

void making_comparing(Code_state *state, int first_register, int second_register)
{
	unsigned char message[3] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (first_register > NEW_REGISTERS)
		WRXB |= REX_B;
	if (second_register > NEW_REGISTERS)
		WRXB |= REX_R;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[CODE_CMP_REGS]; 
	message[2] = (3 << 6) | 
				 (first_register % (NEW_REGISTERS + 1) << 3) | 
				 (second_register % (NEW_REGISTERS + 1));
	
	copy_phrase(state, message, 3);
}

void making_command_jump(Code_state *state, int jump_length)
{
	unsigned char message[3] = {};
	message[0] = OPCODE[CODE_JMP];
	copy_phrase(state, message, 1);

	put_number(state, jump_length);
}

void making_conditional_jump(Code_state *state, int jump_number, int address)
{
	unsigned char message[6] = {};

	message[0] = OPCODE[jump_number];
	message[1] = JUMPS_EXTENDED_OPCODES[jump_number - CODE_JAE];
	copy_phrase(state, message, 2);

	put_number(state, address);
}

void making_call(Code_state *state, int address)
{
	unsigned char message[1] = {};
	
	message[0] = OPCODE[CODE_CALL];
	copy_phrase(state, message, 1);

	put_number(state, address);
}

void making_ret(Code_state *state)
{
	unsigned char message[1] = {};
	message[0] = OPCODE[CODE_RET];
	copy_phrase(state, message, 1);
}



Mod_reg *reg_state_mode_new()
{
	Mod_reg *new_one = (Mod_reg*)calloc(1, sizeof(Mod_reg));
	if (!new_one)
		return NULL;

	return new_one;
}

void reg_state_mode_delete(Mod_reg *thus)
{
	if (thus)
		free(thus);
}



void copy_phrase(Code_state *state, const unsigned char *phrase, const size_t phrase_length)
{
	while (phrase_length + state->code_length >= state->code_capacity)
		translation_resize(state);

	memcpy(state->x86_code + state->code_length, phrase, phrase_length * sizeof(unsigned char));
	
	state->code_length += phrase_length;
}

void put_number(Code_state *state, const int value)
{
	*(int*)(state->x86_code + state->code_length) = value;
	state->code_length += 4;
}

Mod_reg *get_type(Code_state *state, int mode)
{
	if (mode && mode <= REG_RDX)//регистр
    {
        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->type = 0;
        return_parameters->mod = REG_REG;
        return_parameters->reg = AASM_REGISTERS_NUMBERS[mode - REG_RAX];
        return_parameters->address_existing = false;

    	return return_parameters;
    }
    else if (mode && mode >= ADDRSS_RAX && mode <= ADDRSS_RDX)
    {
        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REGADDR;
        return_parameters->reg = AASM_REGISTERS_NUMBERS[mode - REG_RAX];
        return_parameters->address_existing = false;

    	return return_parameters;
    }
    else if ((int)mode == ADDRSS)
    {
        long long value = 0;
    	memcpy((char*)&value, state->aasm_code + state->rip + 1, sizeof(long long));

		Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REGADDR_32OFFSET;
        return_parameters->reg = RDI;//rdi
        return_parameters->address_existing = true;
        return_parameters->address = (int)value;

		return return_parameters;
    }
    else
    {
        long long value = 0;
    	memcpy((char*)&value, state->aasm_code + state->rip + 1, sizeof(long long));

        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REG;
        return_parameters->reg = RAX;//rax
        return_parameters->address_existing = true;
        return_parameters->address = (int)value;

    	return return_parameters;
    }

    return NULL;
}

int get_register_number(int register_number)
{
	return REGISTERS_NUMBERS[register_number % stack_registers] % (NEW_REGISTERS + 1);
}


void translation_process_push(Code_state *state)
{
	size_t push_amount = 0;
	size_t index = state->rip;

	char *bufer = state->aasm_code;
	size_t bufer_length = state->aasm_code_length;
	
	while(index < bufer_length && bufer[index] == 1)
	{
		push_amount++;
		index += stack_push_offsets[bufer[index + 1]];
	}

	size_t registers_in_stack = stack_registers - state->stack_registers_counter;
	size_t stack_amount       = push_amount - stack_registers;
	
	size_t initial_reg_index = (state->stop_place + 1) % stack_registers;
	for (long long i = 0, j = initial_reg_index; i < push_amount; i++, j++)
	{
		state->rip++;

		Mod_reg *command_state = get_type(state, bufer[state->rip]);

		if (i >= registers_in_stack)
		{
			making_stack_register_command(state, CODE_PUSH_REG, REGISTERS_NUMBERS[j % stack_registers]);
		}

		if (command_state->mod == REG_REG && !command_state->address_existing)
			making_command_mov_from_reg(state, command_state->reg, REGISTERS_NUMBERS[j % stack_registers]);
		else if (command_state->mod == REG_REG && command_state->address_existing)
		{
			making_command_mov_imm(state, command_state->reg, REGISTERS_NUMBERS[j % stack_registers], command_state->address);
		}
		else 
		{
			making_command_mov_from_mem(state, command_state->mod, REGISTERS_NUMBERS[j % stack_registers], command_state->reg, command_state->address);
		}
		
		reg_state_mode_delete(command_state);

		state->rip += stack_push_offsets[bufer[state->rip]] - 1;

		state->stop_place = j;
	}

	state->stack_counter += push_amount;
	state->stop_place %= stack_registers;
}

void translation_process_pop(Code_state *state)
{
	size_t pop_amount = 0;
	size_t index = state->rip;

	char *bufer = state->aasm_code;
	size_t bufer_length = state->aasm_code_length;
	while(index < bufer_length && bufer[index] == 8)
	{
		pop_amount++;
		index += stack_pop_offsets[bufer[index + 1]];
	}

	assert(state->stack_counter >= pop_amount);
	
	size_t registers_in_stack = stack_registers - state->stack_registers_counter;
	
	long long initial_reg_index = ((long long)state->stop_place) % stack_registers;
	for (long long i = 0, j = initial_reg_index; i < pop_amount; i++, j--)
	{
		state->rip++;

		if (bufer[state->rip])
		{
			Mod_reg *command_state = get_type(state, bufer[state->rip]);

			unsigned char message[3] = {};
			
			if (command_state->mod == REG_REG && !command_state->address_existing)
				making_command_mov_from_reg(state, REGISTERS_NUMBERS[j % stack_registers], command_state->reg);
			else 
				making_command_mov_from_mem(state, command_state->mod, command_state->reg, REGISTERS_NUMBERS[j % stack_registers], command_state->address);

			if (command_state->address_existing)
				put_number(state, command_state->address);

			if (state->stack_counter > stack_registers)	
				making_stack_register_command(state, CODE_POP_REG, REGISTERS_NUMBERS[j % stack_registers]);

			reg_state_mode_delete(command_state);
		}

		state->rip += stack_pop_offsets[bufer[state->rip]] - 1;

		state->stop_place = j;
		state->stack_counter--;
	}

	state->stop_place = (state->stop_place - 1) % stack_registers;
}

void translation_process_simple_arithmetics(Code_state *state, int mode)
{
	assert(mode == COMMAND_ADD || mode == COMMAND_SUB);

	making_simple_arithmetics(state, translation_table[mode], REGISTERS_NUMBERS[state->stop_place], REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers]);

	state->stop_place = (state->stop_place - 1) % stack_registers;
	state->stack_counter--;
	state->rip++;
}



void translation_process_complex_arithmetics(Code_state *state, int mode)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);

	making_stack_register_command(state, CODE_PUSH_REG, RAX);

	making_stack_register_command(state, CODE_PUSH_REG, RDX);

	making_command_mov_from_reg(state, RAX, REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers]);
	
	making_complex_arithmetics(state, mode, REGISTERS_NUMBERS[(state->stop_place) % stack_registers]);

	making_command_mov_from_reg(state, REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers], RAX);

	making_stack_register_command(state, CODE_POP_REG, RDX);

	making_stack_register_command(state, CODE_POP_REG, RAX);

	state->stop_place = (state->stop_place - 1) % stack_registers;
	state->stack_counter--;
	state->rip++;
}

void translation_process_print(Code_state *state)
{
	unsigned char message[1] = {};

	making_stack_register_command(state, CODE_PUSH_REG, RAX);
	
	making_command_mov_from_reg(state, RAX, REGISTERS_NUMBERS[state->stop_place]);
	
	making_call(state, PRINTF_ADDRESS - state->code_length - 4);
	
	//put_number(state, PRINTF_ADDRESS - state->code_length - 4);

	making_stack_register_command(state, CODE_POP_REG, RAX);

	state->stop_place = (state->stop_place - 1) % stack_registers;
	state->rip++;
}

void translation_process_scan(Code_state *state)
{
	unsigned char message[3] = {};

	making_stack_register_command(state, CODE_PUSH_REG, RAX);

	// message[0] = OPCODE[CODE_CALL];
	// copy_phrase(state, message, 1);

	// put_number(state, SCANF_ADDRESS - state->code_length - 4);
	making_call(state, SCANF_ADDRESS - state->code_length - 4);

	if (state->stack_counter >= stack_registers)
	{
		making_stack_register_command(state, CODE_PUSH_REG, state->stop_place);
	}

	making_command_mov_from_reg(state, REGISTERS_NUMBERS[state->stop_place], RAX);
	
	making_stack_register_command(state, CODE_POP_REG, RAX);

	state->stop_place = (state->stop_place + 1) % stack_registers;
	state->rip++;
}

void translation_process_hlt(Code_state *state)
{
	making_command_jump(state, HLT_ADDRESS - state->code_length - 5);

	state->rip++;
}

bool is_conditional_jump(int instruction_index) 
{
	return instruction_index != COMMAND_JMP;
}

int extract_long_value(Code_state *state)
{
	long long address = (long long)*(state->aasm_code + state->rip);
	state->rip += 8;

	return (int)address;
}

void translation_process_jump(Code_state *state, int jump_number)
{
	if (is_conditional_jump(jump_number))
	{
		making_comparing(state, REGISTERS_NUMBERS[state->stop_place], REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers]);
	
		state->stack_counter -= 2;
		state->stop_place = (state->stop_place - 2) % stack_registers;

		making_conditional_jump(state, translation_table[jump_number], 0);
	}
	else
	{
		making_command_jump(state, 0);
	}

	state->rip++;
	int address = extract_long_value(state);

	simple_list_insert(state->jump_addresses, state->code_length);
}

void translation_process_call(Code_state *state)
{
	state->rip++;
	int address = extract_long_value(state);

	making_call(state, 0);
	
	simple_list_insert(state->jump_addresses, state->code_length);
}



void translation_process_ret(Code_state *state)
{
	making_ret(state);
}

void translation_text_header(Code_state *state)
{
	making_command_mov_imm(state, RAX, RDI, DATA_ADDRESS_START);
	making_command_jump(state, START_JUMP - PRINTF_ADDRESS);

	copy_phrase(state, MY_PRINTF_MACHINE_CODE, SCANF_ADDRESS - PRINTF_ADDRESS);
	state->addresses_counter += SCANF_ADDRESS - PRINTF_ADDRESS;

	copy_phrase(state, MY_SCANF_MACHINE_CODE, HLT_ADDRESS - SCANF_ADDRESS);
	state->addresses_counter += HLT_ADDRESS - SCANF_ADDRESS;

	copy_phrase(state, MY_HLT_MACHINE_CODE, START_JUMP - HLT_ADDRESS);
	state->addresses_counter += START_JUMP - HLT_ADDRESS;
}

void translation_counting_addresses(Code_state *state)
{
	for (state->rip = 0; state->rip < (long long)(state->aasm_code_length/sizeof(char)) - 1;)
	{
		char val = state->aasm_code[state->rip];
		
        char mode = 0;
        int address = 0;

        switch((int)val)
    	{
    		case COMMAND_HLT:
    			state->rip++;
    			break;
    		case COMMAND_PUSH:
    			state->rip += stack_push_offsets[state->aasm_code[state->rip + 1]];
    			break;
    		case COMMAND_ADD:
    		case COMMAND_SUB:
    			state->rip++;
    			break;
    		case COMMAND_MUL:
    		case COMMAND_DIV:
    			state->rip++;
    			break;
    		case COMMAND_OUT:
    			state->rip++;
    			break;
    		case COMMAND_POP:
    			state->rip += stack_pop_offsets[state->aasm_code[state->rip + 1]];
    			break;
    		case COMMAND_JMP:
    		case COMMAND_JAE:
    		case COMMAND_JA:
    		case COMMAND_JB:
    		case COMMAND_JBE:
    		case COMMAND_JE:
    		case COMMAND_JNE:
    		case COMMAND_CALL:
    			state->rip++;
				address = extract_long_value(state);
    			simple_list_insert(state->addresses_storage, address);
    			break;
    		case COMMAND_RET:
    			state->rip++;
    			break;
    		default:
    			break;
    	}
	}
}

void translation_parser_code(Code_state *state)
{
	for (state->rip = 0; state->rip < (long long)(state->aasm_code_length/sizeof(char)) - 1;)
	{
		char val = state->aasm_code[state->rip];

        size_t addresses_amount = state->addresses_storage->array_length;
        int   *addresses_array  = state->addresses_storage->array;
        for (size_t k = 0; k < addresses_amount; k++)
		{
			if (addresses_array[k] == state->rip)
			{
				simple_list_insert(state->label_addresses, state->code_length);

				break;
			}
		}

        switch((int)val)
    	{
    		case COMMAND_HLT:
    			translation_process_hlt(state);
    			break;
    		case COMMAND_PUSH:
    			translation_process_push(state);
    			break;
    		case COMMAND_ADD:
    		case COMMAND_SUB:
    			translation_process_simple_arithmetics(state, val);
    			break;
    		case COMMAND_MUL:
    		case COMMAND_DIV:
    			translation_process_complex_arithmetics(state, val);
    			break;
    		case COMMAND_OUT:
    			translation_process_print(state);
    			break;
    		case COMMAND_POP:
    			translation_process_pop(state);
    			break;
    		case COMMAND_JMP:
    		case COMMAND_JAE:
    		case COMMAND_JA:
    		case COMMAND_JB:
    		case COMMAND_JBE:
    		case COMMAND_JE:
    		case COMMAND_JNE:
    			translation_process_jump(state, val);
    			break;
    		case COMMAND_CALL:
    			translation_process_call(state);
    			break;
    		case COMMAND_RET:
    			translation_process_ret(state);
    		default:
    			break;
    	}
	}
	
	size_t addresses_amount = state->addresses_storage->array_length;

	for (size_t i = 0; i < addresses_amount; i++)
	{
		*(int*)(state->x86_code + state->jump_addresses->array[i] - 4) = 
			state->label_addresses->array[i] - state->jump_addresses->array[i];
	}
}

void translation_into_x86_64(const char *file_name, const char *output_file_name)
{
	Code_state *state = translation_new(max_string_length, file_name);
	translation_text_header(state);
	translation_counting_addresses(state);
	translation_parser_code(state);

	translation_delete(state, output_file_name);
}

