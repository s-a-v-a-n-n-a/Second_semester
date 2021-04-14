#ifndef HASH_MAP_FILE
#define HASH_MAP_FILE

#include <stdio.h>
#include <stdlib.h>

#include "../List/MyList.h"
#include "reading_data.h"

typedef struct The_hash_map
{
    List *table;
    List *meaning;

    size_t capacity;

    size_t chain_capacity;

    size_t elements_amount;

} Hash_map;


Hash_map *hash_new(size_t lists_amount, size_t chain_length);
void hash_construct(Hash_map *table, size_t lists_amount, size_t chain_length);
void hash_delete(Hash_map *table);
void hash_destruct(Hash_map *table);
void hash_resize_and_rehash(Hash_map *table);
void hash_swap_maps(Hash_map *map_to, Hash_map *map_from);

void hash_add_element(Hash_map *table, char *word, char *meaning);
float hash_load_factor(Hash_map *table);
long long hash_find_element(List *same_hash, char *word);
size_t hash_get(char *string, size_t field_size);

void put_in_hash_map(Hash_map *hash_table, Text *text_information);
char *extract_key(Ptrs_t line);
char *extract_definition(Ptrs_t line);

void test_hash_map();

#endif