#include "hash_map.h"

#define ASM_FUNC
#define CRC_HASH

#define LENGTH_WARNING                                                   \
    fprintf(stderr, "-----------------!WARNING!----------------\n");     \
    fprintf(stderr, "IN FILE %s\nIN LINE %d\n", __FILE__, __LINE__);     \
    fprintf(stderr, "WORD IS SHORTENED")

const char	DIVIDER				 = ':';

const float LOAD_FACTOR 	     = 0.75;
const int   LETTERS_AMOUNT 	     = 26;
const int   POLYNOM 		     = 0x04C11DB7;

const size_t HASH_MAP_SIZE       = 100;
const size_t HASH_CHAIN_LENGTH   = 1000;

const size_t MAX_WORD_SIZE       = 1000;
const size_t MAX_DEFINITION_SIZE = 1000;

const size_t AMOUNT_OF_TESTS     = 10000;

const size_t WORDS_TO_LOAD       = 300;


Hash_map *hash_new(size_t lists_amount, size_t chain_length)
{
	Hash_map *new_map = (Hash_map*)calloc(1, sizeof(Hash_map));
	if (hash_construct(new_map, lists_amount, chain_length) != HASHMAP_OK)
		return NULL;

	return new_map;
}

hashmap_code hash_construct(Hash_map *map, size_t lists_amount, size_t chain_length)
{
	list_code list_report = LIST_OK;

	map->map = (List*)calloc(lists_amount, sizeof(List));
	map->meaning = (List*)calloc(lists_amount, sizeof(List));

	if (!map->map || !map->meaning)
		return HASHMAP_NO_MEMORY;
	for (size_t i = 0; i < lists_amount; i++)
	{
		list_report = list_construct(map->map + i, chain_length);
		if (list_report != LIST_OK)
			return HASHMAP_NO_MEMORY;

		list_report = list_construct(map->meaning + i, chain_length);
		if (list_report != LIST_OK)
			return HASHMAP_NO_MEMORY;
	}

	map->capacity 		= lists_amount;
	map->chain_capacity = chain_length;

	map->elements_amount = 0;

	return HASHMAP_OK;
}

hashmap_code hash_delete(Hash_map *map)
{
	if (hash_destruct(map) != HASHMAP_OK)
		return HASHMAP_NULL;

	free(map);

	return HASHMAP_OK;
}

hashmap_code hash_destruct(Hash_map *map)
{
	list_code list_report = LIST_OK;

	if (!map || !map->map || !map->meaning)
		return HASHMAP_NULL;

	for (size_t i = 0; i < map->capacity; i++)
	{
		if (list_destruct(map->map + i) != LIST_OK)
			return HASHMAP_NULL;
		if (list_destruct(map->meaning + i) != LIST_OK)
			return HASHMAP_NULL;
	}

	free(map->map);
	free(map->meaning);

	return HASHMAP_OK;
}

hashmap_code hash_resize_and_rehash(Hash_map *map)
{
	hashmap_code hash_report = HASHMAP_OK;

	size_t new_capacity   = map->capacity * 2;
	size_t chain_capacity = map->chain_capacity;

	Hash_map *new_table = hash_new(new_capacity, chain_capacity);
	if (!new_table)
		return HASHMAP_NO_MEMORY;
	
	size_t table_capacity = map->capacity;
	for (size_t i = 0; i < table_capacity; i++)
	{
		size_t chain = (map->map + i)->length;
		List *current_list_of_keys = map->map + i;
		List *current_list_of_meanings = map->meaning + i;
		for (size_t j = 1; j <= chain; j++)
		{
			char *word    = current_list_of_keys->data[j];
			char *meaning = current_list_of_meanings->data[j];

			size_t hash = 0;
			#ifndef CRC_HASH
			hash = hash_get(word, map->capacity);
			#else
			hash = hash_crc_intrinsic(word, map->capacity);
			#endif
			
			hash_report = hash_add_element(new_table, word, meaning);
			if (hash_report != HASHMAP_OK)
				return hash_report; 
		}
	}	

	hash_report = hash_swap_maps(map, new_table);
	if (hash_report != HASHMAP_OK)
		return hash_report;
	hash_report = hash_swap_maps(map, new_table);
	if (hash_report != HASHMAP_OK)
		return hash_report;

	return HASHMAP_OK;
}

hashmap_code hash_swap_maps(Hash_map *map_to, Hash_map *map_from)
{
	if (!map_to || !map_from)
		return HASHMAP_NULL;

	List *tmp_keys = map_from->map;
	List *tmp_definitions = map_from->meaning;

	map_from->map = map_to->map;
	map_from->meaning = map_to->meaning;

	map_to->map = tmp_keys;
	map_to->meaning = tmp_definitions;

	size_t tmp = map_from->capacity;
	map_from->capacity = map_to->capacity;
	map_to->capacity = tmp;

	tmp = map_from->chain_capacity;
	map_from->chain_capacity = map_to->chain_capacity;
	map_to->chain_capacity = tmp;

	tmp = map_from->elements_amount;
	map_from->elements_amount = map_to->elements_amount;
	map_to->elements_amount = tmp;

	return HASHMAP_OK;
}

hashmap_code hash_add_element(Hash_map *map, char *word, char *meaning)
{
	if (!map || !word || !meaning)
		return HASHMAP_NULL;

	if (hash_load_factor(map) > LOAD_FACTOR)
	{
		if(hash_resize_and_rehash(map) != HASHMAP_OK)
			return HASHMAP_CANNOT_INSERT;
	}

	size_t index = 0;
	#ifndef CRC_HASH
	index = hash_get(word, map->capacity);
	#else
	index = hash_crc_intrinsic(word, map->capacity);
	#endif

	if (hash_find_element(map->map + index, word) == -1)
	{
		if (list_insert_back(map->map + index, word) != LIST_OK)
			return HASHMAP_CANNOT_INSERT;
		if (list_insert_back(map->meaning + index, meaning) != LIST_OK)
			return HASHMAP_CANNOT_INSERT;

		map->elements_amount++;
	}

	return HASHMAP_OK;
}

static float hash_load_factor(Hash_map *map)
{
	return (float)map->elements_amount/((float)map->capacity * (float)map->capacity);
}

long long hash_find_element(List *same_hash, char *word)
{
    long long index = -1;

    if (!same_hash || !word)
    	return index;

    #ifndef ASM_FUNC
    size_t length = same_hash->length;
    char **current = same_hash->data;
    for (long long i = 1; i <= length; i++)
    {
        if (!strcmp(word, current[i]))
        {
            return i;
        }
    }
    #else
    asm (
    "mov r12, 0x1\n\t"
    "mov r13, %[length]\n\t"
    "mov r14, %[data]\n\t"
    "mov r15, %[word]\n\t"
    "cmp r12, r13\n\t"
    "ja hash_find_element_not_found\n\t"
 
    "finding_cycle:\n\t"
    "mov rdi, r15\n\t"
	"mov rsi, [r14 + r12 * 8]\n\t"
	
	"call STRING_COMPARE\n\t"
	"test rax, rax\n\t"

	"jz hash_find_element_found\n\t"
	"inc r12\n\t"
	"cmp r12, r13\n\t"
	"jbe finding_cycle\n\t"

	"jmp hash_find_element_not_found\n\t"

	"hash_find_element_found:\n\t"
	"mov %[index], r12\n\t"
	"jmp end_of_finding\n\t"

    "hash_find_element_not_found:\n\t"
    "mov %[index], -1\n\t"

    "end_of_finding:\n\t"
    : [index]"=r"(index)
    : [data]"r"(same_hash->data), [length]"r"(same_hash->length), [word]"r"(word)
    : "rax", "r12", "rdi", "rsi", "r13", "r14", "r15", "cc", "memory");
    #endif

	return index;
}


__attribute__((noinline)) size_t hash_get(char *string, size_t field_size)
{
    assert(string);

    size_t length = strlen(string);
 
 	size_t the_hash = 0;
    size_t delta = 11;
    for (size_t i = 0; i < length; i++)
    {
        the_hash = the_hash + (string[i] - 'a' + 1) * delta;
        delta *= 11;
    }
 
    return (the_hash) % field_size;
}

size_t hash_crc_intrinsic(const char *string, size_t field_size)
{
	assert(string);

	size_t hash = 0;
	size_t iterations_amount = MAX_STRING_LENGTH/4;
	for (int i = 0; i < iterations_amount; i++)
	{
		unsigned index = i * 4;
		unsigned number = (((((string[index] << iterations_amount) + string[index + 1]) << iterations_amount) + string[index + 2]) << iterations_amount) + string[index + 3];
		hash += _mm_crc32_u32(POLYNOM, number);
	}

	return hash % field_size;
}

void test_hash_map()
{
	Text *dictionary = text_new("./dict/dict.txt");
	Hash_map *hash_table = hash_new(HASH_MAP_SIZE, HASH_CHAIN_LENGTH);
	if (put_in_hash_map(hash_table, dictionary))
		find_test(hash_table);

	hash_delete(hash_table);
	text_delete(dictionary);
}

void find_test(Hash_map *map)
{
	for (size_t i = 0; i < AMOUNT_OF_TESTS; i++)
	{
		size_t small_length = rand() % MAX_STRING_LENGTH;
		char *small_word = (char*)calloc(MAX_STRING_LENGTH, sizeof(char));

		for (size_t j = 0; j < small_length; j++)
		{
			small_word[j] = 'a' + rand() % LETTERS_AMOUNT;
		}

		long long hash = 0;
		#ifndef CRC_HASH
		hash = hash_get(small_word, map->capacity);
		#else
		hash = hash_crc_intrinsic(small_word, map->capacity);
		#endif
		long long index = hash_find_element(map->map + hash, small_word);
		free(small_word);
	}	
}


bool put_in_hash_map(Hash_map *hash_table, Text *text_information)
{
	size_t words_amount = text_information->ptrs_buffer_length;
	for (size_t i = 0; i < words_amount; i++)
	{
		char *word 	  = extract_key(text_information->ptrs_buffer[i]);
		char *meaning = extract_definition(text_information->ptrs_buffer[i]);

		if (hash_add_element(hash_table, word, meaning) != HASHMAP_OK)
			return false;

		free(word);
		free(meaning);
	}

	return true;
}

char *extract_key(Ptrs_t line)
{
	size_t line_length = line.length;
	char  *line_string = line.ptr;

	size_t key_length = 0;
	size_t i = 0;
	for (; i < line_length; i++)
	{
		if (line_string[i] != ':')
			key_length++;
		else
			break;
	}

	if (key_length >= MAX_STRING_LENGTH)
	{
		key_length = MAX_STRING_LENGTH - 1;
		LENGTH_WARNING;
	}

	char *key = (char*)calloc(MAX_STRING_LENGTH, sizeof(char));
	memcpy(key, line_string, key_length * sizeof(char));

	return key;
}

char *extract_definition(Ptrs_t line)
{
	size_t line_length = line.length;
	char  *line_string = line.ptr;

	size_t definition_length = 0;
	size_t key_length        = 0;
	size_t index             = 0;
	for (index = 0; index < line_length; index++)
	{
		if (line_string[index] == DIVIDER)
			break;
		else
			key_length++;
	}

	while(line_string[index] != '\n')
	{
		definition_length++;
		index++;
	}

	char *definition = (char*)calloc(definition_length + 1, sizeof(char));
	memcpy(definition, line_string + key_length + 1, definition_length * sizeof(char));

	return definition;
}
