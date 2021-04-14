#include "reading_data.h"


Text *text_new(const char *filename)
{
    Text *new_text = (Text*)calloc(1, sizeof(Text));
    text_construct(new_text, filename);

    return new_text;
}

void text_construct(Text *thus, const char *filename)
{
    FILE *input = fopen(filename, "rb");

    thus->text_buffer_length = get_size_of_file(input);
    thus->text_buffer = get_the_text(input, &thus->text_buffer_length);

    thus->ptrs_buffer_length = get_number_of_lines(thus->text_buffer, thus->text_buffer_length);

    thus->ptrs_buffer = (Ptrs_t*)calloc(thus->ptrs_buffer_length + 2, sizeof(Ptrs_t));
    //printf("\n\n %p, %lu \n\n", thus->ptrs_buffer, thus->ptrs_buffer_length);

    //assert (get_lines(buffer, ptr_buf, len) == n_lines);
    get_lines(thus->text_buffer, thus->ptrs_buffer, thus->text_buffer_length);
    counting_sizes(thus->ptrs_buffer, thus->ptrs_buffer_length);

    fclose(input);
}

void text_delete(Text *thus)
{
    text_destruct(thus);
    free(thus);
}

void text_destruct(Text *thus)
{
    free(thus->text_buffer);
    free(thus->ptrs_buffer);
}


size_t get_size_of_file(FILE* file)
{
    assert(file != NULL);

    long int len = 0;
    if (!fseek(file, 0, SEEK_END))
        len = ftell(file);
    len++;
    fseek(file, 0, SEEK_SET);

    return len;
}

char *get_the_text(FILE* file, size_t* length)
{
    assert(file != NULL && *length != 0);

    char* buffer = (char*)calloc(*length + 2, sizeof(char));
    long int obj = fread(buffer, sizeof(char), *length, file);

    assert((size_t)obj == *length - 1);

    if (buffer[obj] != '\n')
    {
        (*length) += 2;
        buffer[*length - 2] = '\n';
    }

    buffer[*length - 1] = '\0';

    return buffer;
}

size_t get_number_of_lines(char* buf, size_t length)
{
    assert(buf != NULL);

    size_t n_lines = 0;
    char* ptr        = NULL;
    size_t len_tmp   = 0;

    do
    {
        ptr = strchr(buf, '\n');

        if (ptr != NULL)
        {
            len_tmp += ptr - buf + 1;

            if (len_tmp < length)
                n_lines++;

            buf = ptr + 1;
        }

    }while (ptr != NULL && len_tmp < length);

    return n_lines;
}

size_t get_lines(char* buf, Ptrs_t* ptr_mas, size_t length)
{
    assert(buf != NULL && ptr_mas != NULL);

    size_t n_lines = 0;
    char* ptr      = NULL;
    size_t len_tmp = 0;

    (*ptr_mas).ptr = buf;

    do
    {
        ptr = strchr(buf, '\n');

        if (ptr != NULL)
        {
            len_tmp += ptr - buf;

            if (len_tmp < length)
            {
                n_lines++;
                ptr_mas++;
                buf = ptr + 1;
                (*ptr_mas).ptr = buf;
            }
        }

    }while (ptr != NULL && len_tmp < length);

    return n_lines;
}

void counting_sizes(Ptrs_t* ptr_mas, size_t n_lines)
{
    assert(ptr_mas != NULL && n_lines != 0);

    char *pt = NULL;
    for (size_t i = 0; i <= n_lines; i++)
    {
        pt = (ptr_mas[i]).ptr;
        while(*(pt++) != '\n' && ptr_mas[i].length < MAXSYMB)
        {
            (ptr_mas[i]).length++;
        }
    }
}

void put_line(Ptrs_t* put_ptr, const size_t n_lines, FILE* out)
{
    assert(put_ptr != NULL);
    assert(out     != NULL);

    char* pt = NULL;
    size_t l = 0;

    for (size_t i = 0; i <= n_lines; i++)
    {
        pt = put_ptr[i].ptr;
        l  = put_ptr[i].length + 1;

        fwrite(pt, sizeof(char), l, out);
    }
    fputc('\n', out);
}

void put_the_primary_line(char* line, size_t sz, FILE* out)
{
    assert(line != NULL);

    fwrite(line, sizeof(char), sz, out);
}
