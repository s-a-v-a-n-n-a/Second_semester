#include "Translation.h"

const size_t FILE_LENGTH = 7000;
const size_t TMP_DATA_SIZE = 0x1000;

const size_t stack_registers   = 7;
const size_t max_string_length = 100;

const unsigned char REGISTERS_NUMBERS[] = 
{
	RAX,
	RCX,
	RDX,
	RBX,
	//RSI,
	//"rdi",
	R8,
	R9,
	R10
};


const unsigned char AASM_REGISTERS_NUMBERS[] = 
{
	R12,
	R13,
	R14,
	R15
};




const int translation_table[] = {CODE_JMP, CODE_PUSH_REG, CODE_ADD, CODE_SUB, CODE_MUL, CODE_JMP, CODE_NOP, CODE_NOP, CODE_POP_REG, CODE_JMP, CODE_JMP, CODE_NOP,
								 CODE_DIV, CODE_JMP, CODE_JAE, CODE_JA, CODE_JB, CODE_JBE, CODE_JE, CODE_CALL, CODE_RET};

const unsigned char MY_PRINTF_MACHINE_CODE[] =
{
// 0x53, 0x51, 0x52, 0x41, 0x51, 0x41, 0x52, 0x57, 
// 0xe8, 0x08, 0x00, 0x00, 0x00, 0x5f, 0x41, 0x5a, 
// 0x59, 0x5a, 0x59, 0x5b, 0xc3, 0xe8, 0x1e, 0x00, 
// 0x00, 0x00, 0x48, 0x89, 0xc1, 0x48, 0xc7, 0xc0, 
// 0x01, 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc7, 0x01, 
// 0x00, 0x00, 0x00, 0x48, 0xc7, 0xc6, 0x00, //here 48 c7 c7 00 30 40 00
// 0x30, 0x40, 0x00, 0x48, 0x89, 0xca, 0x0f, 0x05, 
// 0xc3, 0x49, 0x89, 0xc2, 0x48, 0x31, 0xc9, 0x4d, 
// 0x31, 0xc9, 0xfd, 0x48, 0x31, 0xd2, 0x48, 0xc7, 
// 0xc3, 0x0a, 0x00, 0x00, 0x00, 0x48, 0xf7, 0xf3, 
// 0x49, 0xff, 0xc1, 0x48, 0x83, 0xf8, 0x00, 0x77, 
// 0xea, 0x4c, 0x89, 0xd0, 0x48, 0xc7, 0xc7, ////88 07
// 0x00, 0x30, 0x40, 0x00, 0x4c, 0x01, 0xcf, 0x48, 
// 0xff, 0xcf, 0x48, 0x31, 0xd2, 0x48, 0xc7, 0xc3, 
// 0x0a, 0x00, 0x00, 0x00, 0x48, 0xf7, 0xf3, 0x50,      
// 0x48, 0x89, 0xd0, 0x04, 0x30, 0x88, 0x07, 
// 0x48, 0xff, 0xcf, 0x58, 0x48, 0x83, 0xf8, 0x00, 
// 0x77, 0xe1, 0x4c, 0x89, 0xc8, 0xc3 

// 0x53,                      //push   rbx
// 0x51,                    //  push   rcx
// 0x52,                    //  push   rdx
// 0x41, 0x51,                 //  push   r9
// 0x41, 0x52,                 //  push   r10
// 0x57,                    //  push   rdi
// 0xe8, 0x09, 0x00, 0x00, 0x00,        //  call   0x2a
// 0x5f,                    //  pop    rdi
// 0x41, 0x5a,                 //  pop    r10
// 0x41, 0x59,                  // pop    r9
// 0x5a,                     // pop    rdx
// 0x59,                     // pop    rcx
// 0x5b,                      //pop    rbx
// 0xc3,                     // ret
// 0xe8, 0x1d, 0x00, 0x00, 0x00,          //call   0x4c
// 0x48, 0x89, 0xc1,                //mov    rcx,rax
// 0xb8, 0x01, 0x00, 0x00, 0x00,          //mov    eax,0x1
// 0xbf, 0x01, 0x00, 0x00, 0x00,          //mov    edi,0x1
// 0x48, 0xbe, 0x00, 0x20, 0x40, 0x00, 0x00,    //movabs rsi,0x402000
// 0x00, 0x00, 0x00,
// 0x48, 0x89, 0xca,                //mov    rdx,rcx
// 0x0f, 0x05,                   //syscall
// 0xc3,                      //ret
// 0x49, 0x89, 0xc2,               // mov    r10,rax
// 0x48, 0x31, 0xc9,               // xor    rcx,rcx
// 0x4d, 0x31, 0xc9,                //xor    r9,r9
// 0xfd,                     // std
// 0x48, 0x31, 0xd2,                //xor    rdx,rdx
// 0xbb, 0x0a, 0x00, 0x00, 0x00,          //mov    ebx,0xa
// 0x48, 0xf7, 0xf3,                //div    rbx
// 0x49, 0xff, 0xc1,                //inc    r9
// 0x48, 0x83, 0xf8, 0x00,             //cmp    rax,0x0
// 0x77, 0xec,                   //ja     0x56
// 0x4c, 0x89, 0xd0,                //mov    rax,r10
// 0x48, 0xbf, 0x00, 0x20, 0x40, 0x00, 0x00,    //movabs rdi,0x402000
// 0x00, 0x00, 0x00,
// 0x49, 0x81, 0xf9, 0x00, 0x10, 0x00, 0x00,    //cmp    r9,0x1000
// 0x76, 0x09,                   //jbe    0x89
// 0x48, 0x81, 0xc7, 0x00, 0x10, 0x00, 0x00,    //add    rdi,0x1000
// 0xeb, 0x03,                   //jmp    0x8c
// 0x4c, 0x01, 0xcf,                //add    rdi,r9
// 0x48, 0xff, 0xcf,                //dec    rdi
// 0x48, 0x31, 0xd2,                //xor    rdx,rdx
// 0xbb, 0x0a, 0x00, 0x00, 0x00,          //mov    ebx,0xa
// 0x48, 0xf7, 0xf3,                //div    rbx
// 0x50,                      //push   rax
// 0x48, 0x89, 0xd0,                //mov    rax,rdx
// 0x04, 0x30,                   //add    al,0x30
// 0x88, 0x07,                   //mov    BYTE PTR [rdi],al
// 0x48, 0xff, 0xcf,                //dec    rdi
// 0x58,                      //pop    rax
// 0x48, 0x83, 0xf8, 0x00,             //cmp    rax,0x0
// 0x77, 0xe3,                   //ja     0x8f
// 0x4c, 0x89, 0xc8,                //mov    rax,r9
// 0xc3                        //ret
// 0xb8, 0x0a, 0x00, 0x00, 0x00,          //mov    eax,0xa
// 0xe8, 0x0a, 0x00, 0x00, 0x00,          //call   0x14
// 0xb8, 0x3c, 0x00, 0x00, 0x00,          //mov    eax,0x3c
// 0x48, 0x31, 0xff,                	   //xor    rdi,rdi
// 0x0f, 0x05,                   // syscall
0x53,                      // push   rbx
0x51,                      // push   rcx
0x52,                      // push   rdx
0x41, 0x51,                   // push   r9
0x41, 0x52,                   // push   r10
0x57,                      // push   rdi
0x56,
0xe8, 0x0a, 0x00, 0x00, 0x00,          // call   0x2a
0x5e,
0x5f,                      // pop    rdi
0x41, 0x5a,                   // pop    r10
0x41, 0x59,                   // pop    r9
0x5a,                      // pop    rdx
0x59,                      // pop    rcx
0x5b,                      // pop    rbx
0xc3,                      // ret
0xe8, 0x1d, 0x00, 0x00, 0x00,          // call   0x4c
0x48, 0x89, 0xc1,                // mov    rcx,rax
0xb8, 0x01, 0x00, 0x00, 0x00,          // mov    eax,0x1
0xbf, 0x01, 0x00, 0x00, 0x00,          // mov    edi,0x1
0x48, 0xbe, 0x00, 0x20, 0x40, 0x00, 0x00,    // movabs rsi,0x402000
0x00, 0x00, 0x00,
0x48, 0x89, 0xca,                // mov    rdx,rcx
0x0f, 0x05,                   // syscall
0xc3,                      // ret
0x49, 0x89, 0xc2,                // mov    r10,rax
0x48, 0x31, 0xc9,                // xor    rcx,rcx
0x4d, 0x31, 0xc9,                // xor    r9,r9
0xfd,                      // std
0x48, 0x31, 0xd2,                // xor    rdx,rdx
0xbb, 0x0a, 0x00, 0x00, 0x00,          // mov    ebx,0xa
0x48, 0xf7, 0xf3,                // div    rbx
0x49, 0xff, 0xc1,                // inc    r9
0x48, 0x83, 0xf8, 0x00,             // cmp    rax,0x0
0x77, 0xec,                   // ja     0x56
0x4c, 0x89, 0xd0,                // mov    rax,r10
0x48, 0xbf, 0x00, 0x20, 0x40, 0x00, 0x00,    // movabs rdi,0x402000
0x00, 0x00, 0x00,
0x49, 0x81, 0xf9, 0x00, 0x10, 0x00, 0x00,    // cmp    r9,0x1000
0x76, 0x09,                   // jbe    0x89
0x48, 0x81, 0xc7, 0x00, 0x10, 0x00, 0x00,    // add    rdi,0x1000
0xeb, 0x06,                   // jmp    0x8f
0x4c, 0x01, 0xcf,                // add    rdi,r9
0xc6, 0x07, 0x0a,                // mov    BYTE PTR [rdi],0xa
0x48, 0xff, 0xcf,                // dec    rdi
0x48, 0x31, 0xd2,                // xor    rdx,rdx
0xbb, 0x0a, 0x00, 0x00, 0x00,          // mov    ebx,0xa
0x48, 0xf7, 0xf3,                // div    rbx
0x50,                      // push   rax
0x48, 0x89, 0xd0,                // mov    rax,rdx
0x04, 0x30,                   // add    al,0x30
0x88, 0x07,                   // mov    BYTE PTR [rdi],al
0x48, 0xff, 0xcf,                // dec    rdi
0x58,                      // pop    rax
0x48, 0x83, 0xf8, 0x00,             // cmp    rax,0x0
0x77, 0xe3,                   // ja     0x92
0x49, 0xff, 0xc1,                // inc    r9
0x4c, 0x89, 0xc8,                // mov    rax,r9
0xc3,                      // ret
};

const int PRINTF_ADDRESS = 26;

const unsigned char MY_SCANF_MACHINE_CODE[] =
{
// 0x53, 0x51, 0x52, 0x57, 0x56, 0x41, 0x52, 0xe8, 
// 0x08, 0x00, 0x00, 0x00, 0x41, 0x5a, 0x5e, 0x5f, 
// 0x5a, 0x59, 0x5b, 0xc3, 0xe8, 0x01, 0x00, 0x00, 
// 0x00, 0xc3, 0x48, 0x31, 0xc0, 0x49, 0x89, 0xc2, 
// 0x48, 0x31, 0xc0, 0x48, 0xc7, 0xc7, 0x00, 0x00, 
// 0x00, 0x00, 0x48, 0xc7, 0xc6, 0x00, 0x20, //48 c7 c6 00 30 40 00 
// 0x40, 0x00, 0x48, 0xc7, 0xc2, 0x01, 0x00, 0x00, 
// 0x00, 0x0f, 0x05, 0x4c, 0x89, 0xd0, 0x48, 0x8b, 
// 0x1e, 0x48, 0x89, 0xde, 0x48, 0x83, 0xfb, 0x0a,
// 0x76, 0x13, 0x48, 0xc7, 0xc3, 0x0a, 0x00, 0x00, 
// 0x00, 0x48, 0xf7, 0xe3, 0x48, 0x83, 0xee, 0x30, 
// 0x48, 0x01, 0xf0, 0xeb, 0xc1, 0xc3 

0x53,                      //push   rbx
0x51,                      //push   rcx
0x52,                      //push   rdx
0x57,                      //push   rdi
0x56,                      //push   rsi
0x41, 0x52,                   //push   r10
0x41, 0x51,                   //push   r9
0xe8, 0x0a, 0x00, 0x00, 0x00,          //call   0x27
0x41, 0x59,                   //pop    r9
0x41, 0x5a,                   //pop    r10
0x5e,                      //pop    rsi
0x5f,                      //pop    rdi
0x5a,                      //pop    rdx
0x59,                      //pop    rcx
0x5b,                      //pop    rbx
0xc3,                      //ret
0xe8, 0x01, 0x00, 0x00, 0x00,          //call   0x2d
0xc3,                      //ret
0x48, 0x31, 0xc0,                //xor    rax,rax
0x4d, 0x31, 0xc9,                //xor    r9,r9
0x49, 0xff, 0xc1,                //inc    r9
0x49, 0x81, 0xf9, 0x00, 0x10, 0x00, 0x00,    //cmp    r9,0x1000
0x73, 0x3c,                   //jae    0x7b
0x49, 0x89, 0xc2,                //mov    r10,rax
0x48, 0x31, 0xc0,                //xor    rax,rax
0xbf, 0x00, 0x00, 0x00, 0x00,          //mov    edi,0x0
0x48, 0xbe, 0x00, 0x20, 0x40, 0x00, 0x00,    //movabs rsi,0x402000
0x00, 0x00, 0x00,
0xba, 0x01, 0x00, 0x00, 0x00,          //mov    edx,0x1
0x0f, 0x05,                   //syscall
0x4c, 0x89, 0xd0,                //mov    rax,r10
0x48, 0x8b, 0x1e,                //mov    rbx,QWORD PTR [rsi]
0x48, 0x89, 0xde,                //mov    rsi,rbx
0x48, 0x83, 0xfb, 0x0a,             //cmp    rbx,0xa
0x76, 0x11,                   //jbe    0x7b
0xbb, 0x0a, 0x00, 0x00, 0x00,          //mov    ebx,0xa
0x48, 0xf7, 0xe3,                //mul    rbx
0x48, 0x83, 0xee, 0x30,             //sub    rsi,0x30
0x48, 0x01, 0xf0,                //add    rax,rsi
0xeb, 0xb8,                   //jmp    0x33
0xc3                      //ret
};

const unsigned char MY_HLT_MACHINE_CODE[] =
{
0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, 0x48, 
0x31, 0xff, 0x0f, 0x05
};

const int PRINTF_SIZE = sizeof(MY_PRINTF_MACHINE_CODE)/sizeof(unsigned char);
const int SCANF_SIZE = sizeof(MY_SCANF_MACHINE_CODE)/sizeof(unsigned char);
const int HLT_SIZE = sizeof(MY_HLT_MACHINE_CODE)/sizeof(unsigned char);

const int HLT_ADDRESS = SCANF_ADDRESS + SCANF_SIZE;//277;

const int SCANF_ADDRESS = PRINTF_ADDRESS + PRINTF_SIZE;//168;

const int START_JUMP = HLT_ADDRESS + HLT_SIZE;//289;

const int stack_push_offsets[] = {10, 2, 2, 2, 2, 0, 10, 2, 2, 2, 2};
const int stack_pop_offsets[]  = { 2, 2, 2, 2, 2, 0,  9, 2, 2, 2, 2};

Simple_list *simple_list_new(size_t array_length)
{
	Simple_list *new_storage = (Simple_list*)calloc(1, sizeof(Simple_list));
	if (!new_storage)
		return NULL;
	
	Simple_list_state_signs report = simple_list_construct(new_storage, array_length);
	if (report != SLIST_OK)
	{
		free(new_storage);
		return NULL;
	}

	return new_storage;
}

Simple_list_state_signs simple_list_construct(Simple_list *thus, size_t array_length)
{
	if (!thus)
		return SLIST_INVALID_POINTER;

	thus->array = (int*)calloc(array_length, sizeof(int));
	if (!thus->array)
		return SLIST_MEMORY_ERROR;

	thus->array_length = 0;
	thus->array_capacity = array_length;

	return SLIST_OK;
}

Simple_list_state_signs simple_list_delete(Simple_list *thus)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	report = simple_list_destruct(thus);
	if (report != SLIST_OK)
		return report;
	free(thus);

	return SLIST_OK;
}

Simple_list_state_signs simple_list_destruct(Simple_list *thus)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	free(thus->array);

	return SLIST_OK;
}

Simple_list_state_signs simple_list_insert(Simple_list *thus, int value, int index)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	if (thus->array_length + 1 >= thus->array_capacity)
	{
		report = simple_list_resize(thus);
		if (report != SLIST_OK)
			return report;
	}

	thus->array[index] = value;
	thus->array_length++;

	return SLIST_OK;
}

Simple_list_state_signs simple_list_resize(Simple_list *thus)
{
	Simple_list_state_signs report = simple_list_check_pointers(thus);
	if (report != SLIST_OK)
		return report;

	size_t new_array_capacity = thus->array_capacity * 2;
	int *new_array = (int*)realloc(thus->array, new_array_capacity * sizeof(int));
	if (!new_array)
		return SLIST_INVALID_POINTER;

	if (new_array)
	{
		thus->array = new_array;
		thus->array_capacity = new_array_capacity;
	}	

	return SLIST_OK;
}

Simple_list_state_signs simple_list_check_pointers(Simple_list *thus)
{
	if (!thus || !thus->array)
		return SLIST_INVALID_POINTER;

	return SLIST_OK;
}



Code_state *translation_new(size_t length, const char *file_name)
{
	Code_state *new_state = (Code_state*)calloc(1, sizeof(Code_state));
	if (!new_state)
		return NULL;

	Code_state_signs report = translation_construct(new_state, length, file_name);
	if (report != CODE_OK)
	{
		free(new_state);
		return NULL;
	}
	return new_state;
}

Code_state_signs translation_construct(Code_state *state, size_t length, const char *file_name)
{
	if (!state)
		return CODE_INVALID_POINTER;

	size_t n_lines = 0;
	state->aasm_code = reading_file(file_name, &state->aasm_code_length, &n_lines, 0);
	if (!state->aasm_code)
		return CODE_MEMORY_ERROR;

	state->x86_code = (unsigned char*)calloc(length, sizeof(unsigned char));
	if (!state->x86_code)
		return CODE_MEMORY_ERROR;
	state->code_capacity = length;

	state->code_length   = 0;
	state->stack_counter = 0;
	state->stack_registers_counter = 0;
	state->stop_place    = stack_registers - 1;

	state->rip = 0;

	state->addresses_storage = simple_list_new(10);
	state->jump_addresses    = simple_list_new(10);
	state->label_addresses   = simple_list_new(10);

	if (!state->addresses_storage || !state->jump_addresses || !state->label_addresses)
		return CODE_MEMORY_ERROR;

	return CODE_OK;
}

Code_state_signs translation_delete(Code_state *state, const char *file_name)
{
	Code_state_signs report = translation_check_pointers(state);
	if (report != CODE_OK)
		return report;

	FILE *output = fopen(file_name, "wb");
	if (!output)
		return CODE_FILE_ERROR;
	
	unsigned char *final_byte_code = (unsigned char*)calloc(FILE_LENGTH, sizeof(unsigned char));
	if (!final_byte_code)
		return CODE_MEMORY_ERROR;

	size_t memory_loaded = elf_generate_elf_header(final_byte_code, FILE_LENGTH);
	if (memory_loaded == 0)
		printf("Memory error\n");
	else
	{
		size_t new_memory_loaded = elf_generate_program_header(final_byte_code + memory_loaded, FILE_LENGTH - memory_loaded, state->code_length);
		if (new_memory_loaded == 0)
			printf("Memory error\n");
		else
		{
			memory_loaded += new_memory_loaded;
			new_memory_loaded = elf_generate_program_header_for_data(final_byte_code + memory_loaded, FILE_LENGTH - memory_loaded, 0x4000);
			if (new_memory_loaded == 0)
				printf("Memory error\n");
			else
				memory_loaded += new_memory_loaded;
		}
	}
	//printf("%p; %d, %d, %u\n", final_byte_code + ENTRY_ADDRESS, FILE_LENGTH - ENTRY_ADDRESS, ENTRY_ADDRESS, state->code_length);
	memcpy(final_byte_code + ENTRY_ADDRESS, state->x86_code, state->code_length * sizeof(unsigned char));

	put_line(output, final_byte_code, FILE_LENGTH);
	free(final_byte_code);
	fclose(output);

	report = translation_destruct(state);
	if (report != CODE_OK)
		return report;

	free(state);

	return CODE_OK;
}

#define CORRELATE_ADDRESS_ARRAY_AND_CODE\
	if (address_report == SLIST_MEMORY_ERROR)\
		return CODE_MEMORY_ERROR;\
	else if (address_report == SLIST_INVALID_POINTER)\
		return CODE_INVALID_POINTER;\

Code_state_signs translation_destruct(Code_state *state)
{
	Code_state_signs report = translation_check_pointers(state);
	if (report != CODE_OK)
		return report;

	Simple_list_state_signs address_report = SLIST_OK;

	free(state->aasm_code);
	free(state->x86_code);
	
	address_report = simple_list_delete(state->addresses_storage);
	CORRELATE_ADDRESS_ARRAY_AND_CODE
	
	address_report = simple_list_delete(state->label_addresses);
	CORRELATE_ADDRESS_ARRAY_AND_CODE
	
	address_report = simple_list_delete(state->jump_addresses);
	CORRELATE_ADDRESS_ARRAY_AND_CODE

	return CODE_OK;
}

Code_state_signs translation_resize(Code_state *state)
{
	size_t new_capacity = state->code_capacity * 2;
	unsigned char *new_code = (unsigned char*)realloc(state->x86_code, new_capacity * sizeof(unsigned char));
	if (!new_code)
		return CODE_MEMORY_ERROR;

	if (new_code)
	{
		state->x86_code = new_code;
		state->code_capacity = new_capacity;
	}

	return CODE_OK;
}

Code_state_signs translation_check_pointers(Code_state *state)
{
	if (!state || !state->x86_code || !state->aasm_code)
		return CODE_INVALID_POINTER;

	if (simple_list_check_pointers(state->addresses_storage) != SLIST_OK ||
		simple_list_check_pointers(state->label_addresses) != SLIST_OK  ||
		simple_list_check_pointers(state->jump_addresses) != SLIST_OK)
		return CODE_INVALID_POINTER;

	return CODE_OK;
}




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

const unsigned char UNARY_EXTEND = 0b0011;

const unsigned char SHL_EXTEND = 0x4; 

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
	0xc1//shr
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

void making_additional_stack_pop_register(Code_state *state, int register_number)
{

}
					
void making_additional_stack_register_command(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_REG || command_code == CODE_POP_REG);

	unsigned char message[4] = {};

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

	if (register_number > NEW_REGISTERS)
	{
		WRXB |= REX_R;//???
	}

	message[0] = (REX << 4) | WRXB; 
	if (command_code == CODE_POP_REG)
	{
		printf("POP REG: %x\n", OPCODE[CODE_MOV_FROM_MEM]);
		message[1] = OPCODE[CODE_MOV_FROM_MEM];
	}
	else
	{
		printf("PUSH REG: %x\n", OPCODE[CODE_MOV]);
		message[1] = OPCODE[CODE_MOV];
	}
	printf("message %x\n", message[1]);
	message[2] = (REG_REGADDR << 6) |
				 (register_number % (NEW_REGISTERS + 1) << 3) |
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

	unsigned char message[3] = {};

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
		message[1] = OPCODE[CODE_POP_REG] + R11 % (NEW_REGISTERS + 1);

		copy_phrase(state, message, 2);
	}
}

void making_additional_stack_imm_command(Code_state *state, int command_code, int value)
{
	unsigned char message[3] = {};

	if (command_code == CODE_PUSH_IMM)
	{
		printf("work with value %d\n", value);
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

	unsigned char message[3] = {};

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
	unsigned char message[3] = {};

	//making_command_mov_from_mem(state, mod, RDI, RAX);//bad

	making_additional_stack_register_command(state, command_code, RDI);
}

void making_stack_register_address_in_register(Code_state *state, int command_code, int register_number)
{
	assert(command_code == CODE_PUSH_MEM || command_code == CODE_POP_MEM);

	making_simple_arithmetics(state, CODE_ADD, RDI, register_number);

	unsigned char message[3] = {};
	if (register_number > NEW_REGISTERS)
	{
		unsigned char WRXB = REX_B;

		message[0] = (REX << 4) | WRXB;
		copy_phrase(state, message, 1);
	}
	
	message[0] = OPCODE[command_code];
	message[1] = (REG_REGADDR << 6) |
				 (STACK_COMMANDS_EXTENDED_OPCODES[command_code - CODE_PUSH_IMM] << 3) |
				 (register_number % (NEW_REGISTERS + 1));

	copy_phrase(state, message, 2);

	making_simple_arithmetics(state, CODE_SUB, RDI, register_number);
}

void making_left_shift(Code_state *state, int register_number, char shift)
{
	//48 c1 e0 03 shl rax, 3
	unsigned char WRXB = REX_W;
	if (register_number > NEW_REGISTERS)
		WRXB |= REX_B;

	unsigned char message[4] = {};

	message[0] = (REX << 4) | WRXB;
	message[1] = OPCODE[CODE_SHL];
	message[2] = (UNARY_EXTEND << 6) | (SHL_EXTEND << 3) | (register_number % (NEW_REGISTERS + 1));
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
	// if (register_number > NEW_REGISTERS)
	// {
	// 	unsigned char WRXB = REX_B;

	// 	message[0] = (REX << 4) | WRXB;
	// 	copy_phrase(state, message, 1);
	// }
	
	// message[0] = OPCODE[command_code];
	// message[1] = (REG_REGADDR << 6) |
	// 			 (STACK_COMMANDS_EXTENDED_OPCODES[command_code - CODE_PUSH_IMM] << 3) |
	// 			 (register_number % (NEW_REGISTERS + 1));
	// copy_phrase(state, message, 2);
	unsigned char message[3] = {};

	//making_command_mov_from_mem(state, REG_REG, RAX, RDI);//bad
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
	
	printf("imm %d\n", immediate);
	put_number(state, immediate);
}

void making_command_mov_from_mem(Code_state *state, int operation_code, int mod, int register_number_from, int register_number_to, int immediate)
{
	unsigned char message[3] = {};

	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (register_number_to > NEW_REGISTERS)
		WRXB |= REX_R;
	if (register_number_from > NEW_REGISTERS)
		WRXB |= REX_B;
	message[0] = (REX << 4) | WRXB; 
	message[1] = OPCODE[operation_code]; 
	message[2] = (mod << 6) | 
			 	 (register_number_to % (NEW_REGISTERS + 1) << 3) | 
			     (register_number_from % (NEW_REGISTERS + 1));
	
	copy_phrase(state, message, 3);
	
	if (mod == REG_REGADDR_32OFFSET)
		put_number(state, immediate);
}

//add second_register, first_register
void making_simple_arithmetics(Code_state *state, int operation_code, int first_register, int second_register)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);

	unsigned char message[3] = {};
	
	unsigned char WRXB = 0;
	WRXB |= REX_W;
	if (first_register > NEW_REGISTERS)
		WRXB |= REX_R;
	if (second_register > NEW_REGISTERS)
		WRXB |= REX_B;
	message[0] = REX << 4 | WRXB; 
	message[1] = OPCODE[operation_code]; 
	message[2] = (REG_REG << 6) | 
				 (first_register % (NEW_REGISTERS + 1) << 3) | 
				 (second_register % (NEW_REGISTERS + 1));
	
	copy_phrase(state, message, 3);
}

void making_simple_arithmetics_with_stack(Code_state *state, int operation_code)
{
	assert(operation_code == CODE_ADD || operation_code == CODE_SUB);

	unsigned char message[3] = {};
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

	unsigned char message[3] = {};
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

void making_complex_arithmetics_with_stack(Code_state *state, int mode)
{
	assert(mode == COMMAND_MUL || mode == COMMAND_DIV);

	unsigned char message[3] = {};

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

	unsigned char message[3] = {};

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

void making_comparing_with_stack(Code_state *state)
{
	making_stack_register_command(state, CODE_POP_REG, RAX);
	making_stack_register_command(state, CODE_POP_REG, RCX);

	unsigned char message[3] = {};

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

	unsigned char message[3] = {};

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

	// message[0] = 0xcc;
	// copy_phrase(state, message, 1);

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
	while (sizeof(int) + state->code_length >= state->code_capacity)
		translation_resize(state);

	//printf("val %d, %p\n", value, (int*)(state->x86_code + state->code_length));

	//*(int*)(state->x86_code + state->code_length) = value;
	memcpy((int*)(state->x86_code + state->code_length), &value, sizeof(int));
	state->code_length += 4;
}

Mod_reg *get_type(Code_state *state, int mode)
{
	if (mode && mode <= REG_RDX)
    {
        printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");

        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->type = 0;
        return_parameters->mod = REG_REG;
        return_parameters->reg = AASM_REGISTERS_NUMBERS[mode - REG_RAX];
        printf("REG REG %d\n", return_parameters->reg);
        return_parameters->address_existing = false;

    	return return_parameters;
    }
    else if (mode && mode >= ADDRSS_RAX && mode <= ADDRSS_RDX)
    {
        Mod_reg *return_parameters = reg_state_mode_new();
        return_parameters->mod = REG_REGADDR;
        return_parameters->reg = AASM_REGISTERS_NUMBERS[mode - ADDRSS_RAX];
        printf("REGADDR %d\n", return_parameters->reg);
        return_parameters->address_existing = false;

    	return return_parameters;
    }
    else if ((int)mode == ADDRSS)
    {
        printf("[ADDRESS]");

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
	
	long long initial_reg_index = (state->stop_place + 1) % stack_registers;
	//printf("start index = %lld\n", initial_reg_index);
	long long j = initial_reg_index;
	for (long long i = 0; i < push_amount; i++, j++)
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

		state->rip += stack_push_offsets[bufer[state->rip]] - 1;

		state->stop_place = j;
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
		index += stack_pop_offsets[bufer[index + 1]];
	}

	printf("popopopopopopopopopop\n");

	//assert(state->stack_counter >= pop_amount);
	
	size_t registers_in_stack = stack_registers - state->stack_registers_counter;
	
	long long initial_reg_index = ((long long)state->stop_place) % stack_registers;
	for (long long i = 0, j = initial_reg_index; i < pop_amount; i++, j--)
	{
		printf("pop cat\n");
		state->rip++;

		if (bufer[state->rip])
		{
			Mod_reg *command_state = get_type(state, bufer[state->rip]);
			printf("cs %d\n", command_state->reg);

			unsigned char message[3] = {};
			
			if (command_state->mod == REG_REG && !command_state->address_existing)
			{
				printf("POP AREG\n");
				//making_command_mov_from_reg(state, REGISTERS_NUMBERS[j % stack_registers], command_state->reg);
				making_additional_stack_register_command(state, CODE_POP_REG, command_state->reg);
			}
			else if (command_state->mod != REG_REG && command_state->address_existing)
			{
				//printf("mlem\n");
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

		state->rip += stack_pop_offsets[bufer[state->rip]] - 1;

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
	printf("address baaaaad %lld\n", address);
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
		printf("eye\n");
		making_command_jump(state, 0);
	}

	state->rip++;
	int address = extract_long_value(state);

	printf("jumps amount %d\n", state->jump_addresses->array_length);
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

        printf("%d\n", val);

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
    		case COMMAND_IN:
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
    			//printf("state->rip %d\n", state->rip);
				address = extract_long_value(state);
				//printf("addrss %d\n", address);
    			simple_list_insert(state->addresses_storage, address, state->addresses_storage->array_length);
    			printf("val %d, %d, addrss %d\n", val, state->addresses_storage->array_length, address);
    			break;
    		case COMMAND_RET:
    			state->rip++;
    			break;
    		default:
    			break;
    	}
	}

	printf("!!!!!!!!!!!TOTAL SIZE %d!!!!!!!!!!!!!\n", state->addresses_storage->array_length);
}

void translation_parser_code(Code_state *state)
{
	printf("parsing parsing parsing parsing parsing parsing parsing\n");

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
				printf("label %x\n", state->code_length);

				//break;
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
		printf("where %x, what %x(%d) - %x = %x\n", state->jump_addresses->array[i] - 4, state->label_addresses->array[i], state->label_addresses->array[i], state->jump_addresses->array[i], state->label_addresses->array[i] - state->jump_addresses->array[i]);
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

