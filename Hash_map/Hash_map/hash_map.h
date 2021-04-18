#ifndef HASH_MAP_FILE
#define HASH_MAP_FILE

#include <cstdio>
#include <cstdlib>
#include <cassert>

#include <clocale>

#include <nmmintrin.h>

#include "../List/MyList.h"
#include "reading_data.h"

typedef struct The_hash_map
{
    List *map;
    List *meaning;

    size_t capacity;

    size_t chain_capacity;

    size_t elements_amount;

} Hash_map;

typedef enum hashmap_code_errors
{
    HASHMAP_OK,
    HASHMAP_NULL,
    HASHMAP_SEG_FAULT,
    HASHMAP_DELETED,
    HASHMAP_NO_CONSTRUCT,
    HASHMAP_NO_MEMORY,
    HASHMAP_CANNOT_INSERT
} hashmap_code;

extern "C" int STRING_COMPARE		(char *string1, char *string2);

Hash_map        *hash_new					(size_t lists_amount, size_t chain_length);
hashmap_code 	 hash_construct				(Hash_map *map, size_t lists_amount, size_t chain_length);
hashmap_code	 hash_delete 				(Hash_map *map);
hashmap_code	 hash_destruct 				(Hash_map *map);
hashmap_code 	 hash_resize_and_rehash 	(Hash_map *map);
hashmap_code 	 hash_swap_maps				(Hash_map *map_to, Hash_map *map_from);

hashmap_code 	 hash_add_element			(Hash_map *map, char *word, char *meaning);
static float 	 hash_load_factor			(Hash_map *map);
long long 		 hash_find_element			(List *same_hash, char *word);

size_t 	  hash_get					(const char *string, size_t field_size);
size_t 	  hash_crc_intrinsic		(const char *string, size_t field_size);

//FUNCTIONS FOR TESTING
//---------------------------------------------------------------------------------
void 	  test_hash_map				();
void 	  find_test					(Hash_map *map);

bool 	  put_in_hash_map			(Hash_map *hash_table, Text *text_information);
char 	 *extract_key				(Ptrs_t line);
char 	 *extract_definition		(Ptrs_t line);
//---------------------------------------------------------------------------------

#endif