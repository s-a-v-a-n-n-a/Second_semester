// #include "Instructions_realizations.h"
#include "Translation.h"


int main()
{
	translation_into_x86_64("binary.xex", "output");

	system("chmod +x ./output");

	return 0;
}

/*
#include "Translation.h"

const int REG_REGADDR = 0;
const int REG_REGADDR_8OFFSET = 1;
const int REG_REGADDR_32OFFSET = 2;
const int REG_REG = 3;

typedef struct Mod_reg
{
	char type;
	char mod;
	char reg[4];
	bool address_existing;
	size_t address;
}Mod_reg;

Mod_reg *reg_state_mode_new()
{
	Mod_reg *new_one = (Mod_reg*)calloc(1, sizeof(Mod_reg));
	if (!new_one)
		return NULL;

	return new_one;
}

void reg_state_mode_delete(Mod_reg *thus)
{
	free(thus);
}

const char *MODS[] = 
{
	"00",
	"01",
	"10",
	"11"
}

const char ONE_ARG_EXTENTION[] = {0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0};

void put_phrase(Code_state *state, const char *phrase)
{
	size_t phrase_length = strlen(phrase);

	while (phrase_length + state->code_length >= state->code_capacity)
		translation_resize(state);

	sprintf(state->x86_code + state->code_length, "%s", phrase);
	state->code_length += phrase_length;
}

void copy_phrase(Code_state *state, const char *phrase, const size_t phrase_length)
{
	while (phrase_length + state->code_length >= state->code_capacity)
		translation_resize(state);

	memcpy(state->x86_code + state->code_length, phrase, phrase_length * sizeof(char));
	state->code_length += phrase_length;
}

void put_number(Code_state *state, const long long value)
{
	(int*)(state->x86_code + state->code_length)[0] = value;
	state->code_length += 4;
}

Mod_reg *get_type(Code_state *state, int mode)
{
	if (mode && mode <= REG_RDX)//регистр
    {
        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->type = 0;
        return_parameters->mod = REG_REG;
        return_parameters->reg = AASM_REGISTERS_NUMBERS[mode - REG_RAX] + 4;//offset
        return_parameters->address_existing = false;

     //    char *phrase = (char*)calloc(4, sizeof(char));
    	// sprintf(phrase, "%s", aasm_registers[mode - REG_RAX]);
    	// //sprintf(phrase, "11%s", AASM_REGISTERS_NUMBERS[mode - REG_RAX]);
    	
    	// return phrase;
    	return return_parameters;
    }
    else if (mode && mode >= ADDRSS_RAX && mode <= ADDRSS_RDX)
    {
        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REGADDR;
        return_parameters->reg = AASM_REGISTERS_NUMBERS[mode - REG_RAX] + 4;//offset
        return_parameters->address_existing = false;

     //    char *phrase = (char*)calloc(6, sizeof(char));
    	// sprintf(phrase, "[%s]", aasm_registers[mode - ADDRSS_RAX]);
    	
    	// return phrase;
    	return return_parameters;
    }
    else if ((int)mode == ADDRSS)
    {
        long long value = 0;
    	memcpy((char*)&value, state->aasm_code + state->rip + 1, sizeof(long long));

		Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REGADDR_32OFFSET;
        return_parameters->reg = 7;//rdi
        return_parameters->address_existing = true;
        return_parameters->address = value;

		// char *phrase = (char*)calloc(24, sizeof(char));
  //   	sprintf(phrase, "[rdi + %lld * 8]", value);

  //   	return phrase;
    	return return_parameters;
    }
    else
    {
        long long value = 0;
    	memcpy((char*)&value, state->aasm_code + state->rip + 1, sizeof(long long));

        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REG;
        return_parameters->reg = 5;//rbp
        return_parameters->address_existing = true;
        return_parameters->address = value;

    	// char *phrase = (char*)calloc(20, sizeof(char));
    	// sprintf(phrase, "%lld", value);

    	// return phrase;
    	return return_parameters;
    }

    return NULL;
}

void translation_process_hlt(Code_state *state)
{
	/*
	mov rax, 0x3C ;exit64
	xor rdi, rdi
	syscall
	
	// if (state->passage_number)
	// {
		// char *message = (char*)calloc(max_string_length, sizeof(char));

		// sprintf(message, "mov rax, 0x3C\n");
		// put_phrase(state, message);

		// sprintf(message, "xor rdi, rdi\n");
		// put_phrase(state, message);

		// sprintf(message,"syscall\n");
		// put_phrase(state, message);

		copy_phrase(state, MY_HLT_MACHINE_CODE, 12);

		// free(message);

		state->addresses_counter += 12;
	// }

	state->rip++;
}

void translation_process_push(Code_state *state)
{
	size_t push_amount = 0;
	size_t index = state->rip;

	char *bufer = state->aasm_code;

	size_t bufer_length = state->aasm_code_length;
	//printf("size %lu, %d\n", bufer_length, bufer[index]);
	while(index < bufer_length && bufer[index] == 1)
	{
		//printf("while\n");
		push_amount++;
		index += stack_push_offsets[bufer[index + 1]];
	}

	//Теперь сначала пихаем в стек, а потом в регистеры
	//index = state->rip;
	size_t registers_in_stack = stack_registers - state->stack_registers_counter;
	size_t stack_amount = push_amount - stack_registers;
	
	size_t initial_reg_index = (state->stop_place + 1) % stack_registers;
	for (size_t i = 0, j = initial_reg_index; i < push_amount; i++, j++)
	{
		state->rip++;

		// if (state->passage_number)
		// {
			Mod_reg *command_state = get_type(state, bufer[state->rip]);//Обязательно освободить

			//char *phrase = get_type(state, bufer[state->rip]);
			char *message = (char*)calloc(max_string_length, sizeof(char));

			if (i >= registers_in_stack)
			{
				// sprintf(message, "push %s\n", registers_names[j % stack_registers]);//или всё-таки i?

				char WRXB = 8;
				if (j % stack_registers >= SECOND_REGISTERS_EIGHT_NUMBER)
					WRXB += 1;
				message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_PUSH_REG] + REGISTERS_NUMBERS[j % stack_registers];
				copy_phrase(state, message, 2);
				// sprintf(message, "%d%d", REX * 16 + WRXB, OPCODE[CODE_PUSH_REG] + REGISTERS_NUMBERS[j % stack_registers]);

				// put_phrase(state, message);
				state->addresses_counter += 2;
			}

			// sprintf(message, "mov %s, %s\n", registers_names[j % stack_registers], phrase);//или всё-таки i?

			char WRXB = 8;
			message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_MOV]; message[3] = command_state->mod * 64 + REGISTERS_NUMBERS[j % stack_registers] * 8 + command_state->reg;
			copy_phrase(state, message, 3);
			// sprintf(message, "%d%d%d", REX * 16 + WRXB, OPCODE[CODE_MOV], command_state->mod * 64 + REGISTERS_NUMBERS[j % stack_registers] * 8 + command_state->reg);

			// put_phrase(state, message);

			if (command_state->address_existing)
			{
				put_number(state, command_state->address);
				state->addresses_counter += 8;
			}

			free(message);
			//free(phrase);

			reg_state_mode_delete(command_state);

			state->addresses_counter += 3;
		// }

		state->rip += stack_push_offsets[bufer[state->rip]] - 1;

		state->stop_place = j;
	}

	state->stack_counter += push_amount;
	state->stop_place %= stack_registers;
}

// void translation_process_simple_arithmetics(Code_state *state, int mode)
// {
// 	if (state->passage_number)
// 	{
// 		char *message = (char*)calloc(max_string_length, sizeof(char));
// 		sprintf(message, "%s %s, %s\n", simple_arithmetics[mode - 2], registers_names[(state->stop_place - 1) % stack_registers], registers_names[(state->stop_place) % stack_registers]);
// 		put_phrase(state, message);
// 		free(message);
// 	}

// 	state->stop_place = (state->stop_place - 1) % stack_registers;
// 	state->stack_counter--;
// 	state->rip++;
// }

void translation_process_simple_arithmetics(Code_state *state, int mode)
{
	// if (state->passage_number)
	// {
		char *message = (char*)calloc(max_string_length, sizeof(char));
		//sprintf(message, "%s %s, %s\n", simple_arithmetics[mode - 2], registers_names[(state->stop_place - 1) % stack_registers], registers_names[(state->stop_place) % stack_registers]);
		char WRXB = 8;
		if (state->stop_place >= SECOND_REGISTERS_EIGHT_NUMBER)
			WRXB += 1;
		if ((state->stop_place - 1) % stack_registers >= SECOND_REGISTERS_EIGHT_NUMBER)
			WRXB += 4;
		message[0] = REX * 16 + WRXB; message[1] = OPCODE[translation_table[mode]]; message[2] = MODS[REG_REG] * 64 + REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers] * 8 + REGISTERS_NUMBERS[(state->stop_place) % stack_registers];
		//sprintf(message, "%d%d%d", REX * 16 + WRXB, OPCODE[translation_table[mode]], MODS[REG_REG] * 64 + REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers] * 8 + REGISTERS_NUMBERS[(state->stop_place) % stack_registers]);
		//put_phrase(state, message);
		copy_phrase(state, message, 3);
		free(message);
	// }

	state->addresses_counter += 3;

	state->stop_place = (state->stop_place - 1) % stack_registers;
	state->stack_counter--;
	state->rip++;
}

// void translation_process_complex_arithmetics(Code_state *state, int mode)
// {
// 	if (state->passage_number)
// 	{
// 		char *message = (char*)calloc(max_string_length, sizeof(char));
// 		sprintf(message, "mov rax, %s\n", registers_names[(state->stop_place - 1) % stack_registers]);
// 		put_phrase(state, message);

// 		sprintf(message, "%s %s\n", rax_arithmetics[mode - 4], registers_names[(state->stop_place) % stack_registers]);
// 		put_phrase(state, message);

// 		sprintf(message, "mov %s, rax\n", registers_names[(state->stop_place - 1) % stack_registers]);
// 		put_phrase(state, message);

// 		free(message);
// 	}

// 	state->stop_place = (state->stop_place - 1) % stack_registers;
// 	state->stack_counter--;
// 	state->rip++;
// }

void translation_process_complex_arithmetics(Code_state *state, int mode)
{
	// if (state->passage_number)
	// {
		char *message = (char*)calloc(max_string_length, sizeof(char));


		// sprintf(message, "%d%d", REX * 16 + WRXB, OPCODE[CODE_PUSH_REG] + REGISTERS_NUMBERS[0]);
		// put_phrase(message);
		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_PUSH_REG] + REGISTERS_NUMBERS[0];
		copy_phrase(state, message, 2);

// 		sprintf(message, "%d%d", REX * 16 + WRXB, OPCODE[CODE_PUSH_REG] + REGISTERS_NUMBERS[1]);
//		put_phrase(message);
		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_PUSH_REG] + REGISTERS_NUMBERS[1];
		copy_phrase(state, message, 2);

		char WRXB = 8;
		if ((state->stop_place - 1) % stack_registers >= SECOND_REGISTERS_EIGHT_NUMBER)
			WRXB += 4;
		//sprintf(message, "mov rax, %s\n", registers_names[(state->stop_place - 1) % stack_registers]);
		//sprintf(message, "%d%d%d", REX * 16 + WRXB, OPCODE[CODE_MOV], MODS[REG_REG] * 64 + REGISTERS_NUMBERS[0] * 8 + REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers]);
		//put_phrase(state, message);
		message[0] = REX * 16 + WRXB; message[1] = WRXB, OPCODE[CODE_MOV]; message[2] = MODS[REG_REG] * 64 + REGISTERS_NUMBERS[0] * 8 + REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers];
		copy_phrase(state, message, 3);
		WRXB = 8;

		//sprintf(message, "%s %s\n", rax_arithmetics[mode - 4], registers_names[(state->stop_place) % stack_registers]);
		//		sprintf(message, "%d%d%d", REX * 16 + WRXB, OPCODE[translation_table[mode]], MODS[REG_REG] * 64 + ONE_ARG_EXTENTION[translation_table[mode]] * 8 + REGISTERS_NUMBERS[(state->stop_place) % stack_registers]);
		// 		put_phrase(state, message);
		message[0] = REX * 16 + WRXB; message[1] = OPCODE[translation_table[mode]]; message[2] = MODS[REG_REG] * 64 + ONE_ARG_EXTENTION[translation_table[mode]] * 8 + REGISTERS_NUMBERS[(state->stop_place) % stack_registers];
		copy_phrase(state, message, 3);

		if ((state->stop_place - 1) % stack_registers >= SECOND_REGISTERS_EIGHT_NUMBER)
			WRXB += 1;
		//sprintf(message, "mov %s, rax\n", registers_names[(state->stop_place - 1) % stack_registers]);
//		sprintf(message, "%d%d%d", REX * 16 + WRXB, OPCODE[CODE_MOV], MODS[REG_REG] * 64 + REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers] * 8 + REGISTERS_NUMBERS[0]);
// 		put_phrase(state, message);
		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_MOV]; message[2] = MODS[REG_REG] * 64 + REGISTERS_NUMBERS[(state->stop_place - 1) % stack_registers] * 8 + REGISTERS_NUMBERS[0];
		copy_phrase(state, message, 3);

// 		sprintf(message, "%d%d", REX * 16 + WRXB, OPCODE[CODE_POP_REG] + REGISTERS_NUMBERS[1]);
//		put_phrase(message);
		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_POP_REG] + REGISTERS_NUMBERS[1];
		copy_phrase(state, message, 2);

// 		sprintf(message, "%d%d", REX * 16 + WRXB, OPCODE[CODE_POP_REG] + REGISTERS_NUMBERS[0]);
//		put_phrase(message);
		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_POP_REG] + REGISTERS_NUMBERS[0];
		copy_phrase(state, message, 2);

		free(message);
	// }

	state->addresses_counter += 17;

	state->stop_place = (state->stop_place - 1) % stack_registers;
	state->stack_counter--;
	state->rip++;
}

void translation_process_print(Code_state *state)
{
	// if (state->passage_number)
	// {
		char *message = (char*)calloc(max_string_length, sizeof(char));

		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_PUSH_REG] + REGISTERS_NUMBERS[0];
		copy_phrase(state, message, 2);

		// sprintf(message, "mov rax, %s\n", registers_names[(state->stop_place) % stack_registers]);

		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_MOV]; message[2] = MODS[REG_REG] * 64 + REGISTERS_NUMBERS[0] * 8 + REGISTERS_NUMBERS[(state->stop_place) % stack_registers];
		copy_phrase(state, message, 3);

		// sprintf(message, "call MY_PRINTF\n");

//		sprintf(message, "");

		// put_phrase(state, message);

		message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_POP_REG] + REGISTERS_NUMBERS[0];
		copy_phrase(state, message, 2);

		free(message);
	// }

	state->addresses_counter += 7;

	state->stop_place = (state->stop_place - 1) % stack_registers;
	state->rip++;
}

void translation_process_pop(Code_state *state)
{
	//Считаем количество пушей
	//потом обрабатываем их
	size_t pop_amount = 0;
	size_t index = state->rip;

	//printf("pop, %lu\n", state->rip);

	char *bufer = state->aasm_code;
	size_t bufer_length = state->aasm_code_length;
	//printf("command %d\n", bufer[index]);
	while(index < bufer_length && bufer[index] == 8)
	{
		pop_amount++;
		index += stack_pop_offsets[bufer[index + 1]];
	}

	assert(state->stack_counter >= pop_amount);
	//Теперь сначала пихаем в стек, а потом в регистеры
	//index = state->rip;
	size_t registers_in_stack = stack_registers - state->stack_registers_counter;
	
	long long initial_reg_index = ((long long)state->stop_place) % stack_registers;
	for (long long i = 0, j = initial_reg_index; i < pop_amount; i++, j--)
	{
		state->rip++;

		// if (state->passage_number)//Было нужно для печати меток, в машинном коде - нет
		// {
			if (bufer[state->rip])
			{
				Mod_reg *command_state = get_type(state, bufer[state->rip]);//Обязательно освободить

				char *message = (char*)calloc(max_string_length, sizeof(char));
				char *phrase = get_type(state, bufer[state->rip]);

				// sprintf(message, "mov %s, %s\n", phrase, registers_names[j % stack_registers]);

				//// sprintf(message, "%d%d%d", REX * 16 + WRXB, OPCODE[CODE_MOV], command_state->mod * 64 + command_state->reg * 8 + REGISTERS_NUMBERS[j % stack_registers]);

				char WRXB = 8;
				message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_MOV]; message[2] = command_state->mod * 64 + command_state->reg * 8 + REGISTERS_NUMBERS[j % stack_registers];
				copy_phrase(state, message, 3);

				put_phrase(state, message);

				if (command_state->address_existing)
				{
					put_number(state, command_state->address);
					state->addresses_counter++;
				}

				if (state->stack_counter > stack_registers)	
				{
					sprintf(message, "pop %s\n", registers_names[j % stack_registers]);

					char WRXB = 8;
					if (j % stack_registers >= SECOND_REGISTERS_EIGHT_NUMBER)
						WRXB += 1;
					// sprintf(message, "%d%d", REX * 16 + WRXB, OPCODE[CODE_POP_REG] + REGISTERS_NUMBERS[j % stack_registers]);

					message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_POP_REG] + REGISTERS_NUMBERS[j % stack_registers];
					copy_phrase(state, message, 2);

					// put_phrase(state, message);

					state->addresses_counter += 2;
				}

				// free(phrase);
				free(message);

				reg_state_mode_delete(command_state);
				state->addresses_counter += 3;
			}
		// }

		state->rip += stack_pop_offsets[bufer[state->rip]] - 1;

		state->stop_place = j;
		state->stack_counter--;
	}

	// state->stack_counter -= pop_amount;
	state->stop_place = (state->stop_place - 1) % stack_registers;
}


void translation_process_scan(Code_state *state)
{
	if (state->passage_number)
	{
		char *message = (char*)calloc(max_string_length, sizeof(char));
		
		sprintf(message, "push rax\n");

		sprintf(message, "call MY_SCANF\n");
		put_phrase(state, message);

		if (state->stack_counter >= stack_registers)
		{
			sprintf(message, "push %s\n", registers_names[state->stop_place]);//???мб +1 или что
			put_phrase(state, message);
		}

		sprintf(message, "mov %s, rax\n pop rax\n", registers_names[state->stop_place]);
		put_phrase(state, message);

		free(message);
	}

	state->stop_place = (state->stop_place + 1) % stack_registers;
	state->rip++;
}

void translation_process_jump(Code_state *state, int jump_number)
{
	jump_number -= 13;

	if (jump_number)
	{
		// if (state->passage_number)
		// {
			char *message = (char*)calloc(max_string_length, sizeof(char));
			// sprintf(message, "cmp %s, %s\n", registers_names[state->stop_place], registers_names[(stack_registers - state->stop_place) % stack_registers]);
			// put_phrase(state,  message);

			char WRXB = 8;
			if (state->stop_place >= SECOND_REGISTERS_EIGHT_NUMBER)
				WRXB += 1;
			if ((stack_registers - state->stop_place) % stack_registers >= SECOND_REGISTERS_EIGHT_NUMBER)
				WRXB += 4;

			message[0] = REX * 16 + WRXB; message[1] = OPCODE[CODE_CMP_REGS]; message[2] = 3 * 64 + REGISTERS_NUMBERS[state->stop_place] * 8 + REGISTERS_NUMBERS[(stack_registers - state->stop_place) % stack_registers];
			copy_phrase(state, message, 3);
			
			free(message);
		// }
		//????????????????????????????????????????????????????????????????????????????????????
		state->stack_counter -= 2;
		state->stop_place = (stack_registers - state->stop_place - 2) % stack_registers;
		//????????????????????????????????????????????????????????????????????????????????????
		state->addresses_counter += 3;
	}

	//printf("rip %lu\n", state->rip);
	//С днём отрицательного прыжка
	long long address = 1;
	state->rip++;
	address = (long long)*(state->aasm_code + state->rip);
	state->rip += 8;

	//Заюзать адресную вставку
	//state->addresses_storage->addrss_array[state->addresses_storage++] = address;
	(state->addresses_storage, address);//пока так и оставим
	address_insert(state->jump_addresses, state->addresses_counter + 2);

	// if (state->passage_number)
	// {
		char *message = (char*)calloc(max_string_length, sizeof(char));
		
		// sprintf(message, "%s l%ld\n", jmp_instructions[jump_number], (state->addresses_counter++));
		// put_phrase(state, message);

		message[0] = translation_table[jump_number]; message[1] = 0x0;
		copy_phrase(state, message, 2);

		free(message);
	// }

	state->addresses_counter += 2;
}

void translation_process_call(Code_state *state)
{
	size_t address = 0;
	state->rip++;
	sscanf(state->aasm_code, "%lu", &address);
	state->rip += 8;

	address_insert(state->addresses_storage, address);
	address_insert(state->jump_addresses, state->addresses_counter + 9);

	// if (state->passage_number)
	// {
		char *message = (char*)calloc(max_string_length, sizeof(char));
		
		// sprintf(message, "call %ld\n", state->addresses_counter - 1);//????? че это
		// put_phrase(state, message);

		message[0] = OPCODE[CODE_CALL];
		copy_phrase(state, message, 1);

		put_number(state, state->addresses_storage[state->addresses_counter - 1] - state->rip);

		free(message);
	// }

	state->addresses_counter += 9;
}

void translation_process_ret(Code_state *state)
{
	// if (state->passage_number)
	// {
		char message = OPCODE[CODE_RET];
		copy_phrase(state, message, 1);
		// put_phrase(state, "ret\n");
	// }

	state->addresses_counter++;
}

//вызываемые функции будут возвращать сдвиг rip
void translation_parser_code(Code_state *state)
{
	char *code_copy = state->x86_code;

	int counter = 0;

	// for (state->passage_number = 0; state->passage_number < 2; state->passage_number++)//Было нужно для печати меток, в машинном коде - нет
	// {
	// 	state->stop_place = stack_registers - 1;
	// 	state->stack_counter = 0;

	// 	state->addresses_counter = 0;

		for (state->rip = 0; state->rip < (long long)(state->aasm_code_length/sizeof(char)) - 1;)
		{
			char val = state->aasm_code[state->rip];
			
	        char mode = 0;

	        size_t addresses_amount = state->addresses_storage->array_length;
	        int   *addresses_array  = state->addresses_storage->addrss_array;
	        for (size_t k = 0; k < addresses_amount; k++)
			{
				if (addresses_array[k] == state->rip)
				{
					address_insert(state->label_addresses, state->addresses_counter);
					// char *message = (char*)calloc(max_string_length, sizeof(char));
					// sprintf(message, "l%ld:\n", k);
					// put_phrase(state, message);
					// free(message);

					break;
				}
			}

			printf("%d\n", val);

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

	    	counter++;
		}
		
		size_t addresses_amount = state->addresses_storage->array_length;
		// for (state->rip = 0; state->rip < (long long)(state->aasm_code_length/sizeof(char)) - 1;)
		// {
		// 	char val = state->aasm_code[state->rip];
		// 	if ((int)val >= COMMAND_JMP && (int)val < COMMAND_RET)
		// 	{
		// 		state->state->x86_code
		// 	}
		// }
		for (size_t i = 0; i < addresses_amount; i++)
		{
			(int*)state->state->x86_code = state->jump_addresses[i] - state->label_addresses[i];
		}
	// }
}

void translation_text_header(Code_state *state)
{
	char message[] = "section .text\nglobal _start\n";
	put_phrase(state, message);

	put_phrase(state, PRINTF);

	char message_start[] = "\n_start:\nmov rdi, buffer\n";
	put_phrase(state, message_start);

	// char message = OPCODE[CODE_JMP];
	// copy_phrase(state, message, 1);

	// put_number(state, START_JUMP);

	// copy_phrase(state, MY_PRINTF_MACHINE_CODE, SCANF_ADDRESS - PRINTF_ADDRESS);

	// copy_phrase(state, MY_SCANF_MACHINE_CODE, START_JUMP - SCANF_ADDRESS);
}

void translation_text_conclusion(Code_state *state)
{
	char message[] = "section .bss\nbuffer	resb	10000\noutput_buffer	resb	10000\n";
	put_phrase(state, message);
}

void translation_into_x86_64(const char *file_name, const char *output_file_name)
{
	Code_state *state = translation_new(max_string_length, file_name);
	translation_text_header(state);
	translation_parser_code(state);
	translation_text_conclusion(state);

	translation_delete(state, output_file_name);
}

int main()
{
	translation_into_x86_64("binary.xex", "output.asm");

	return 0;
}
*/
