#include "hash_map.h"

const float load_factor = 0.7;

const size_t hash_map_size     = 100;
const size_t hash_chain_length = 1000;

const size_t max_word_size       = 1000;
const size_t max_definition_size = 1000;

const size_t words_to_load = 100;


void test_hash_map()
{
	Text *dictionary = text_new("./dict/dict.txt");
	Hash_map *hash_table = hash_new(hash_map_size, hash_chain_length);
	put_in_hash_map(hash_table, dictionary);
	printf("Loaded\n");

	char state = 0;
	char addition = 0;
	char *key = (char*)calloc(max_word_size, sizeof(char));
	char *definition = (char*)calloc(max_definition_size, sizeof(char));
	
	scanf("%c", &state);
	while (state != '\n')
	{
		if (state == 'f')
		{
			scanf("%c%[^\n]%*c", &addition, key);

			long long hash = hash_get(key, hash_table->capacity);
			long long index = hash_find_element(hash_table->table + hash, key);

			if (index != -1)
				printf("%s", hash_table->meaning[hash].data[index]);
			else
				printf("No match\n");
		}
		else if (state == 'a')
		{
			scanf("%c%[^\n]%*c", &addition, key);
			scanf("%[^\n]%*c", definition);

			hash_add_element(hash_table, key, definition);
		}

		scanf("%c", &state);
	}

	hash_delete(hash_table);
	text_delete(dictionary);
}


void put_in_hash_map(Hash_map *hash_table, Text *text_information)
{
	size_t words_amount = text_information->ptrs_buffer_length;
	for (size_t i = 0; i < words_to_load; i++)
	{
		char *word = extract_key(text_information->ptrs_buffer[i]);
		char *meaning = extract_definition(text_information->ptrs_buffer[i]);

		hash_add_element(hash_table, word, meaning);

		free(word);
		free(meaning);
	}
}

char *extract_key(Ptrs_t line)
{
	size_t line_length = line.length;
	char  *line_string = line.ptr;

	size_t key_length = 0;
	for (size_t i = 0; i < line_length; i++)
	{
		if (line_string[i] != ':')
			key_length++;
		else
			break;
	}

	char *key = (char*)calloc(key_length + 1, sizeof(char));
	memcpy(key, line_string, key_length * sizeof(char));

	return key;
}

char *extract_definition(Ptrs_t line)
{
	size_t line_length = line.length;
	char  *line_string = line.ptr;

	size_t definition_length = 0;
	size_t key_length = 0;
	size_t index = 0;
	for (index = 0; index < line_length; index++)
	{
		if (line_string[index] == ':')
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


Hash_map *hash_new(size_t lists_amount, size_t chain_length)
{
	Hash_map *new_table = (Hash_map*)calloc(1, sizeof(Hash_map));
	hash_construct(new_table, lists_amount, chain_length);

	return new_table;
}

void hash_construct(Hash_map *table, size_t lists_amount, size_t chain_length)
{
	table->table = (List*)calloc(lists_amount, sizeof(List));
	for (size_t i = 0; i < lists_amount; i++)
	{
		list_construct(table->table + i, chain_length);
	}

	table->meaning = (List*)calloc(lists_amount, sizeof(List));
	for (size_t i = 0; i < lists_amount; i++)
	{
		list_construct(table->meaning + i, chain_length);
	}

	table->capacity = lists_amount;

	table->chain_capacity = chain_length;

	table->elements_amount = 0;
}

void hash_delete(Hash_map *table)
{
	hash_destruct(table);
	free(table);
}

void hash_destruct(Hash_map *table)
{
	for (size_t i = 0; i < table->capacity; i++)
	{
		list_destruct(table->table + i);
		list_destruct(table->meaning + i);
	}

	free(table->table);
	free(table->meaning);
}

void hash_resize_and_rehash(Hash_map *table)
{
	size_t new_capacity   = table->capacity * 2;
	size_t chain_capacity = table->chain_capacity;

	Hash_map *new_table = hash_new(new_capacity, chain_capacity);
	
	size_t table_capacity = table->capacity;
	for (size_t i = 0; i < table_capacity; i++)
	{
		size_t chain = (table->table + i)->length;
		List *current_list_of_keys = table->table + i;
		List *current_list_of_meanings = table->meaning + i;
		for (size_t j = 1; j <= chain; j++)
		{
			char *word    = current_list_of_keys->data[j];
			char *meaning = current_list_of_meanings->data[j];

			size_t hash = hash_get(word, new_capacity);
			
			hash_add_element(new_table, word, meaning);
		}
	}	

	hash_swap_maps(table, new_table);
	hash_destruct(new_table);
}

void hash_swap_maps(Hash_map *map_to, Hash_map *map_from)
{
	List *tmp_keys = map_from->table;
	List *tmp_definitions = map_from->meaning;

	map_from->table = map_to->table;
	map_from->meaning = map_to->meaning;

	map_to->table = tmp_keys;
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
}

void hash_add_element(Hash_map *table, char *word, char *meaning)
{
	if (hash_load_factor(table) > load_factor)
	{
		hash_resize_and_rehash(table);
	}

	size_t index = hash_get(word, table->capacity);

	if (hash_find_element(table->table + index, word) == -1)
	{
		list_insert_back(table->table + index, word);
		list_insert_back(table->meaning + index, meaning);

		table->elements_amount++;
	}
}

float hash_load_factor(Hash_map *table)
{
	return (float)table->elements_amount/(float)table->capacity;
}

long long hash_find_element(List *same_hash, char *word)
{
    for (long long i = 1; i <= same_hash->length; i++)
    {
        if (!strcmp(word, same_hash->data[i]))
        {
            return i;
        }
    }

    return -1;
}
 
size_t hash_get(char *string, size_t field_size)
{
    size_t length = strlen(string);
 
    size_t the_hash = 0;
    size_t delta = 11;
    for (size_t i = 0; i < length; i++)
    {
        the_hash = the_hash + (string[i] - 'a' + 1) * delta;
        delta += 100;
    }
 
    return (the_hash) % field_size;
}