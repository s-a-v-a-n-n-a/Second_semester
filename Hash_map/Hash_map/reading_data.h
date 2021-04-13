#ifndef THE_DATA
#define THE_DATA

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

const int MAXSYMB = 1000;

typedef struct Ptrs
{
    char  *ptr;
    size_t length;
} Ptrs_t;

typedef struct Text_buffer
{
    Ptrs_t *ptrs_buffer;
    char   *text_buffer;

    size_t ptrs_buffer_length;
    size_t text_buffer_length;
} Text;


Text *text_new          (const char *filename);
void text_construct     (Text *thus, const char *filename);
void text_delete        (Text *thus);
void text_destruct      (Text *thus);

size_t get_size_of_file     (FILE* file);
char *get_the_text          (FILE* file, size_t* length);
size_t get_number_of_lines  (char* buf, size_t length);
size_t get_lines            (char* buf, Ptrs_t* ptr_mas, size_t length);
void counting_sizes         (Ptrs_t* ptr_mas, size_t n_lines);
void put_line               (Ptrs_t* put_ptr, const size_t n_lines, FILE* out);
void put_the_primary_line   (char* line, size_t sz, FILE* out);

#endif