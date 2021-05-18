#include "Translation.h"
#include "My_library.h"

//const size_t stack_registers = 7;
// const unsigned char REGISTERS_NUMBERS[] = 
// {
// 	RAX,
// 	RCX,
// 	RDX,
// 	RBX,

// 	R8,
// 	R9,
// 	R10
// };

const unsigned char AASM_REGISTERS_NUMBERS[] = 
{
	R12,
	R13,
	R14,
	R15
};

const int translation_table[] = 
{
	CODE_JMP, 
	CODE_PUSH_REG, 
	CODE_ADD, 
	CODE_SUB, 
	CODE_MUL, 
	CODE_JMP, 
	CODE_NOP, 
	CODE_NOP, 
	CODE_POP_REG, 
	CODE_JMP, 
	CODE_JMP, 
	CODE_NOP,
	CODE_DIV, 
	CODE_JMP, 
	CODE_JAE, 
	CODE_JA, 
	CODE_JB, 
	CODE_JBE, 
	CODE_JE, 
	CODE_CALL, 
	CODE_RET
};


const int STACK_PUSH_OFFSETS[] = {10, 2, 2, 2, 2, 0, 10, 2, 2, 2, 2};
const int STACK_POP_OFFSETS[]  = { 2, 2, 2, 2, 2, 0,  9, 2, 2, 2, 2};



void copy_phrase(Code_state *state, const unsigned char *phrase, const size_t phrase_length)
{
	while (phrase_length + state->code_length >= state->code_capacity)
		translation_resize(state);

	memcpy(state->x86_code + state->code_length, phrase, phrase_length * sizeof(unsigned char));
	state->code_length += phrase_length;
}

void put_number(Code_state *state, const int value)
{
	while (sizeof(int) + state->code_length >= state->code_capacity)
		translation_resize(state);

	memcpy((int*)(state->x86_code + state->code_length), &value, sizeof(int));
	state->code_length += 4;
}

Mod_reg *get_type(Code_state *state, int mode)
{
	if (mode && mode <= REG_RDX)
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
        return_parameters->reg = AASM_REGISTERS_NUMBERS[mode - ADDRSS_RAX];
        return_parameters->address_existing = false;

    	return return_parameters;
    }
    else if ((int)mode == ADDRSS)
    {
        long long value = 0;
    	memcpy((char*)&value, state->aasm_code + state->rip + 1, sizeof(long long));

		Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REGADDR_32OFFSET;
        return_parameters->reg = RDI;
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
        return_parameters->reg = RAX;
        return_parameters->address_existing = true;
        return_parameters->address = (int)value;

    	return return_parameters;
    }

    return NULL;
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
		index += STACK_PUSH_OFFSETS[bufer[index + 1]];
	}

	// size_t registers_in_stack = stack_registers - state->stack_registers_counter;
	// size_t stack_amount       = push_amount - stack_registers;
	
	// long long initial_reg_index = (state->stop_place + 1) % stack_registers;
	// long long j = initial_reg_index;
	for (long long i = 0; i < push_amount; i++/*, j++*/)
	{
		state->rip++;

		Mod_reg *command_state = get_type(state, bufer[state->rip]);

		/*
		if (i >= registers_in_stack)
		{
			making_stack_register_command(state, CODE_PUSH_REG, REGISTERS_NUMBERS[j % stack_registers]);
		}
		*/

		if (command_state->mod == REG_REG && !command_state->address_existing)
		{
			//making_command_mov_from_reg(state, command_state->reg, REGISTERS_NUMBERS[j % stack_registers]);
			making_additional_stack_register_command(state, CODE_PUSH_REG, command_state->reg);
		}
		else if (command_state->mod == REG_REG && command_state->address_existing)
		{
			//making_command_mov_imm(state, command_state->reg, REGISTERS_NUMBERS[j % stack_registers], command_state->address);
			making_additional_stack_imm_command(state, CODE_PUSH_IMM, command_state->address);
		}
		else if (command_state->mod == REG_REGADDR)
		{
			making_additional_stack_register_address_in_register(state, CODE_PUSH_MEM, command_state->reg);
		}
		else 
		{
			//making_command_mov_from_mem(state, command_state->mod, REGISTERS_NUMBERS[j % stack_registers], command_state->reg, command_state->address);
			making_additional_stack_register_address(state, CODE_PUSH_MEM, command_state->mod, command_state->address);
		}
		
		
		reg_state_mode_delete(command_state);

		state->rip += STACK_PUSH_OFFSETS[bufer[state->rip]] - 1;

		// state->stop_place = j;
	}

	state->stack_counter += push_amount;
	//state->stop_place %= stack_registers;
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
		index += STACK_POP_OFFSETS[bufer[index + 1]];
	}

	//assert(state->stack_counter >= pop_amount);
	
	// size_t registers_in_stack = stack_registers - state->stack_registers_counter;
	
	// ong long initial_reg_index = ((long long)state->stop_place) % stack_registers;
	for (long long i = 0/*, j = initial_reg_index*/; i < pop_amount; i++/*, j--*/)
	{
		state->rip++;

		if (bufer[state->rip])
		{
			Mod_reg *command_state = get_type(state, bufer[state->rip]);
			
			unsigned char message[3] = {};
			
			if (command_state->mod == REG_REG && !command_state->address_existing)
			{
				//making_command_mov_from_reg(state, REGISTERS_NUMBERS[j % stack_registers], command_state->reg);
				making_additional_stack_register_command(state, CODE_POP_REG, command_state->reg);
			}
			else if (command_state->mod != REG_REG && command_state->address_existing)
			{
				making_additional_stack_imm_command(state, CODE_POP_IMM, 0);
			}
			else if (command_state->mod == REG_REGADDR)
			{
				making_additional_stack_register_address_in_register(state, CODE_POP_MEM, command_state->reg);
			}
			else 
			{
				//making_command_mov_from_mem(state, command_state->mod, command_state->reg, REGISTERS_NUMBERS[j % stack_registers], command_state->address);
				making_additional_stack_register_address(state, CODE_POP_MEM, command_state->mod, command_state->address);
			}

			/*
			if (state->stack_counter > stack_registers)	
				making_stack_register_command(state, CODE_POP_REG, REGISTERS_NUMBERS[j % stack_registers]);
			*/

			reg_state_mode_delete(command_state);
		}
		else
			making_stack_imm_command(state, CODE_POP_IMM, 0);

		state->rip += STACK_POP_OFFSETS[bufer[state->rip]] - 1;

		//state->stop_place = j;
		state->stack_counter--;
	}

	//state->stop_place = (state->stop_place - 1) % stack_registers;
}

void translation_process_simple_arithmetics(Code_state *state, int mode)
{
	assert(mode == COMMAND_ADD || mode == COMMAND_SUB);

	/*making_simple_arithmetics(state, translation_table[mode], REGISTERS_NUMBERS[state->stop_place], REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers]);

	state->stop_place = (state->stop_place - 1) % stack_registers;
	*/
	state->stack_counter--;
	
	making_simple_arithmetics_with_additional_stack(state, translation_table[mode]);
	state->rip++;
}



void translation_process_complex_arithmetics(Code_state *state, int mode)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);

	/*
	making_stack_register_command(state, CODE_PUSH_REG, RAX);

	making_stack_register_command(state, CODE_PUSH_REG, RDX);

	making_command_mov_from_reg(state, RAX, REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers]);
	
	making_complex_arithmetics(state, mode, REGISTERS_NUMBERS[(state->stop_place) % stack_registers]);

	making_command_mov_from_reg(state, REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers], RAX);

	making_stack_register_command(state, CODE_POP_REG, RDX);

	making_stack_register_command(state, CODE_POP_REG, RAX);

	state->stop_place = (state->stop_place - 1) % stack_registers;
	*/
	state->stack_counter--;
	
	making_complex_arithmetics_with_additional_stack(state, mode);
	state->rip++;
}

void translation_process_print(Code_state *state)
{
	/*
	making_stack_register_command(state, CODE_PUSH_REG, RAX);
	
	making_command_mov_from_reg(state, RAX, REGISTERS_NUMBERS[state->stop_place]);
	*/
	making_additional_stack_register_command(state, CODE_POP_REG, RAX);
	
	making_call(state, PRINTF_ADDRESS - state->code_length - 5);
	
	//making_stack_register_command(state, CODE_POP_REG, RAX);

	
	//state->stop_place = (state->stop_place - 1) % stack_registers;
	//state->stack_counter--;
	
	state->rip++;
}

void translation_process_scan(Code_state *state)
{
	unsigned char message[3] = {};

	//making_stack_register_command(state, CODE_PUSH_REG, RAX);

	making_call(state, SCANF_ADDRESS - state->code_length - 5);

	/*
	if (state->stack_counter >= stack_registers)
	{
		making_stack_register_command(state, CODE_PUSH_REG, state->stop_place);
	}
	*/

	/*
	if ((state->stop_place + 1) % stack_registers)
	{
		making_command_mov_from_reg(state, REGISTERS_NUMBERS[(state->stop_place + 1) % stack_registers], RAX);
	
		making_stack_register_command(state, CODE_POP_REG, RAX);
	}

	state->stop_place = (state->stop_place + 1) % stack_registers;
	*/
	state->stack_counter++;

	making_additional_stack_register_command(state, CODE_PUSH_REG, RAX);
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
	long long address = 0;
	memcpy(&address, (long long*)(state->aasm_code + state->rip), sizeof(long long));
	state->rip += 8;

	return (int)address;
}

void translation_process_jump(Code_state *state, int jump_number)
{
	if (is_conditional_jump(jump_number))
	{
		//making_comparing(state, REGISTERS_NUMBERS[state->stop_place], REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers]);
		making_comparing_with_additional_stack(state);
		/*
		state->stop_place = (state->stop_place - 2) % stack_registers;
		*/
		state->stack_counter -= 2;

		making_conditional_jump(state, translation_table[jump_number], 0);
	}
	else
	{
		making_command_jump(state, 0);
	}

	state->rip++;
	int address = extract_long_value(state);

	simple_list_insert(state->jump_addresses, state->code_length, state->jump_addresses->array_length);
}

void translation_process_call(Code_state *state)
{
	int address = extract_long_value(state);

	making_call(state, 0);
	
	simple_list_insert(state->jump_addresses, state->code_length, state->jump_addresses->array_length);

	state->rip++;
}



void translation_process_ret(Code_state *state)
{
	making_ret(state);

	state->rip++;
}

void translation_text_header(Code_state *state)
{
	making_command_mov_imm(state, RAX, RSI, DATA_ADDRESS_START + TMP_DATA_SIZE);
	making_command_mov_imm(state, RAX, RBX, 0);
	making_command_mov_imm(state, RAX, RDI, DATA_ADDRESS_START + TMP_DATA_SIZE * 2);
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
    			state->rip += STACK_PUSH_OFFSETS[state->aasm_code[state->rip + 1]];
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
    		case COMMAND_IN:
    			state->rip++;
    			break;
    		case COMMAND_POP:
    			state->rip += STACK_POP_OFFSETS[state->aasm_code[state->rip + 1]];
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
				simple_list_insert(state->addresses_storage, address, state->addresses_storage->array_length);
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
				simple_list_insert(state->label_addresses, state->code_length, k);
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
    		case COMMAND_IN:
    			translation_process_scan(state);
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
	Code_state *state = translation_new(MAX_MESSAGE_LENGTH, file_name);
	translation_text_header(state);
	translation_counting_addresses(state);
	translation_parser_code(state);

	translation_delete(state, output_file_name);
}

