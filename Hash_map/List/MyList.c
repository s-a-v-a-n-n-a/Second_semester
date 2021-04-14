#include "MyList.h"
#include "ListPicture.c"
#include "AllSettings.h"

#define SLOW_PRINT

#define ASSERTION(code)                                                  \
    fprintf(stderr, "-----------------!WARNING!----------------\n");     \
    fprintf(stderr, "IN FILE %s\nIN LINE %d\n", __FILE__, __LINE__);     \
    list_print_errors(code);


List* list_new(size_t list_size)
{
    List* new_list = (List*)calloc(1, sizeof(List));
    if (!new_list)
    {
        list_print_list_appearance(NULL, LIST_NO_CONSTRUCT, CONSTRUCTION);
        return NULL;
    }

    if (list_construct(new_list, list_size) != LIST_OK)
    {
        list_destruct(new_list);
        return NULL;
    }

    return new_list;
}

list_code list_delete(List *that_list)
{
    list_code code = list_destruct(that_list);

    if (code == LIST_OK)
    {
        free(that_list);
        return LIST_OK;
    }
    else
        return code;
}

list_code list_construct(List *that_list, const size_t list_size)
{
    that_list->capacity   = list_size;
    that_list->sorted     = 1;
    that_list->first_free = 1;

    that_list->data = (list_elem_type*)calloc(list_size + 1, sizeof(list_elem_type));
    that_list->next = (size_t*)calloc(list_size + 1, sizeof(size_t));
    that_list->prev = (size_t*)calloc(list_size + 1, sizeof(size_t));

    if (!that_list->data || !that_list->next || !that_list->prev)
    {
        ASSERTION(LIST_NO_CONSTRUCT);
        list_print_list_appearance(NULL, LIST_NO_CONSTRUCT, CONSTRUCTION);
        return LIST_NO_CONSTRUCT;
    }
    that_list->data[0] = NAN;
    that_list->next[0] = 0;
    that_list->prev[0] = 0;
    for (size_t i = 1; i <= list_size; i++)
    {
        that_list->data[i] = NAN;
        that_list->next[i] = i + 1;
        that_list->prev[i] = i - 1;
    }

    that_list->length = 0;

    return LIST_OK;
}

list_code list_destruct(List *that_list)
{
    if (!that_list)
        return LIST_DELETED;

    if (that_list->data)
        free(that_list->data);
    if (that_list->next)
        free(that_list->next);
    if (that_list->prev)
        free(that_list->prev);

    return LIST_OK;
}

list_code list_resize(List *that_list, const double coefficient)
{
    if (that_list->capacity > ((size_t)-1) / 2)
    {
        ASSERTION(LIST_TOO_BIG);
        list_print_list_appearance(that_list, LIST_TOO_BIG, RESIZING);
        return LIST_TOO_BIG;
    }

    size_t new_capacity = (size_t)(that_list->capacity * coefficient);

    list_elem_type* new_data = (list_elem_type*)realloc(that_list->data, sizeof(list_elem_type) * (new_capacity + 1));
    if (new_data) that_list->data = new_data;

    size_t* new_next = (size_t*)realloc(that_list->next, sizeof(size_t) * (new_capacity + 1));
    if (new_next) that_list->next = new_next;

    size_t* new_prev = (size_t*)realloc(that_list->prev, sizeof(size_t) * (new_capacity + 1));
    if (new_prev) that_list->prev = new_prev;

    if (!new_data || !new_next || !new_prev)
    {
        ASSERTION(LIST_NO_MEMORY);
        list_print_list_appearance(that_list, LIST_NO_MEMORY, RESIZING);
        return LIST_NO_MEMORY;
    }

    if (coefficient > 1)
    {
        for (size_t i = that_list->length + 1; i <= new_capacity; i++)
        {
            that_list->next[i] = i + 1;
            that_list->prev[i] = i - 1;
            that_list->data[i] = NAN;
        }
    }

    that_list->capacity = new_capacity;

    return LIST_OK;
}

list_code list_insert(List *that_list, const size_t before_physical_index, const list_elem_type value)
{
    list_code request = list_request_verifier(that_list, INSERTION, before_physical_index);
    if (request != LIST_OK)
        return request;

    if (that_list->length + 1 >= that_list->capacity)
    {
        list_resize(that_list, 2);
    }
    if (before_physical_index != that_list->length)
        that_list->sorted = 0;

    size_t tmp_free       = that_list->first_free;
    that_list->first_free = that_list->next[that_list->first_free];

    that_list->data[tmp_free] = value;

    that_list->next[tmp_free] = that_list->next[that_list->prev[before_physical_index]];
    that_list->next[that_list->prev[before_physical_index]] = tmp_free;

    that_list->prev[tmp_free] = that_list->prev[before_physical_index];
    that_list->prev[that_list->next[tmp_free]] = tmp_free;

    that_list->length++;

    return LIST_OK;
}

list_code list_insert_front(List *that_list, const list_elem_type value)
{
    return list_insert(that_list, that_list->next[0], value);
}

list_code list_insert_back(List *that_list, const list_elem_type value)
{
    return list_insert(that_list, 0, value);
}

list_code list_remove(List *that_list, const size_t physical_index, list_elem_type* value)
{
    if (physical_index != that_list->length)
        that_list->sorted = 0;

    *value = that_list->data[physical_index];
    that_list->data[physical_index] = NAN;

    that_list->next[that_list->prev[physical_index]] = that_list->next[physical_index];
    that_list->prev[that_list->next[physical_index]] = that_list->prev[physical_index];

    that_list->next[physical_index] = that_list->first_free;
    that_list->prev[that_list->first_free] = physical_index;
    that_list->prev[physical_index] = that_list->prev[0];
    that_list->first_free = physical_index;

    that_list->length--;

    if (that_list->length <= that_list->capacity / 4)
    {
        list_resize(that_list, 0.5);
    }

    return LIST_OK;
}

list_code list_insert_front(List *that_list, list_elem_type *value)
{
    return list_remove(that_list, that_list->next[0], value);
}

list_code list_remove_back(List *that_list, list_elem_type *value)
{
    return list_remove(that_list, that_list->prev[0], value);
}

list_code list_find_index(List *that_list, const size_t logical_index, size_t *physical_index)
{
    size_t tmp_index = 0;
    size_t cur_index = that_list->next[0];

    while (tmp_index <= logical_index + 1)
    {
        tmp_index++;
        cur_index = that_list->next[cur_index];
    }

    *physical_index = cur_index;

    return LIST_OK;
}

list_code list_get_element(List *that_list, const size_t physical_index, list_elem_type *requested_element)
{
    list_code request = list_request_verifier(that_list, GETTING, physical_index);
    if (request != LIST_OK)
        return request;

    if (that_list->sorted)
    {
        *requested_element = that_list->data[physical_index];
        return LIST_OK;
    }

    list_elem_type element = that_list->data[that_list->next[0]];
    size_t index = that_list->next[0];

    for (size_t i = 1; i < physical_index + 1; i++)
    {
        index = that_list->next[index];
        element = that_list->data[index];
    }

    *requested_element = element;

    return LIST_OK;
}

size_t list_get_head_index(List *that_list)
{
    return that_list->next[0];
}

size_t list_get_tail_index(List *that_list)
{
    return that_list->prev[0];
}

list_code list_get_next_index(List *that_list, size_t physical_index, list_elem_type *next_index)
{
    list_code request = list_request_verifier(that_list, GETTING, physical_index);
    if (request != LIST_OK)
        return request;

    *next_index = that_list->next[physical_index];

    return LIST_OK;
}

list_code list_get_prev_index(List *that_list, size_t physical_index, list_elem_type *prev_index)
{
    list_code request = list_request_verifier(that_list, GETTING, physical_index);
    if (request != LIST_OK)
        return request;

    *prev_index = that_list->prev[physical_index];

    return LIST_OK;
}

list_code list_slow_sort(List *that_list)
{
    if (!that_list->sorted)
    {
        size_t index = 0;
        list_elem_type* new_data = (list_elem_type*)calloc(that_list->capacity + 1, sizeof(list_elem_type));
        if (!new_data)
        {
            ASSERTION(LIST_NO_MEMORY);
            list_print_list_appearance(that_list, LIST_NO_MEMORY, SORTING);
            list_destruct(that_list);
        }

        for (size_t i = 0; i <= that_list->length; i++)
        {
            new_data[i] = that_list->data[index];

            index = that_list->next[index];
        }
        free(that_list->data);
        that_list->data = new_data;

        for (size_t i = 0; i <= that_list->length + 1; i++)
        {
            that_list->next[i] = (i + 1) % (that_list->length + 1);
            that_list->prev[i] = (i + that_list->length) % (that_list->length + 1);
        }
        that_list->first_free = that_list->length + 1;
        that_list->sorted     = 1;

        for (size_t i = that_list->length + 1; i <= that_list->capacity; i++)
        {
            that_list->data[i] = NAN;
            that_list->next[i] = i + 1;
            that_list->prev[i] = i - 1;
        }
    }

    return LIST_OK;
}

void list_print_errors(list_code code)
{
    printf("Error: %s\n", list_state_text[code]);
}

void list_print_list_appearance(List *that_list, list_code code, const list_functions function)
{
    static long int doing = 0;

    const char* mode = "";
    if (!doing)
        mode = "wb";
    else
        mode = "ab";
    FILE* log = fopen("list_print_list_appearance.html", mode);

    fprintf(log, "<pre><font size=\"5\"  face=\"Times New Roman\">\n");

    fprintf(log, "<p><span style=\"font-weight: bold\">CURRENT STATE OF LIST</span></p>\n");
    fprintf(log, "THE NEWS FROM %s\n", FUNCTION_IDENTIFIERS[function]);

    fprintf(log, "%s", state_text[code]);
    if (that_list->sorted)
        fprintf(log, "CURRENT STATE OF SORTING IS <span style=\"font-weight: bolder\">TRUE</span>\n");
    else
        fprintf(log, "CURRENT STATE OF SORTING IS <span style=\"font-weight: bolder\">FALSE</span>\n");

    fprintf(log, "CURRENT CAPACITY IS %u\n", that_list->capacity);
    fprintf(log, "CURRENT SIZE IS            %u\n", that_list->length);

    fprintf(log, "</font><tt>\n");

    char picture_name[MAX_PICTURE_NAME]          = "";

    char picture_adding[MAX_PICTURE_NAME]        = "";
    sprintf(picture_adding, "%d.txt", doing);

    char picture_format_name[MAX_PICTURE_NAME]   = "";
    char picture_format_adding[MAX_PICTURE_NAME] = "";
    #ifdef SLOW_PRINT
    sprintf(picture_name, "%s", PICTURE_FILE_CODE);

    strncat(picture_name, picture_adding, MAX_PICTURE_NAME);

    sprintf(picture_format_name, "%s", PICTURE_FILE_CODE);

    strncat(picture_format_name, picture_adding, MAX_PICTURE_NAME);

    sprintf(picture_format_adding, ".%s", PICTURE_FORMAT);

    strncat(picture_format_name, picture_format_adding, MAX_PICTURE_NAME);

    list_slow_print_logical_state(that_list, picture_name);
    fprintf(log, "<img src=%s>\n", picture_format_name);
    #endif

    sprintf(picture_name, "%s", PICTURE_FILE_CODE_REAL);
    strncat(picture_name, picture_adding, MAX_PICTURE_NAME);

    sprintf(picture_format_name, "%s", PICTURE_FILE_CODE_REAL);
    strncat(picture_format_name, picture_adding, MAX_PICTURE_NAME);
    strncat(picture_format_name, picture_format_adding, MAX_PICTURE_NAME);

    list_print_physical_state(that_list, picture_name);
    fprintf(log, "<img src=%s>", picture_format_name);

    fprintf(log, "\n");

    fclose(log);

    doing++;
}

list_code list_is_pointer_valid(List *that_list)
{
    if (!that_list || !that_list->data || !that_list->next || !that_list->prev)
        return LIST_NULL;

    if ((size_t)(that_list) <= 4096 || (size_t)(that_list->data) <= 4096 || (size_t)(that_list->next) <= 4096 || (size_t)(that_list->prev) <= 4096)
        return LIST_SEG_FAULT;

    return LIST_OK;
}

list_code list_request_verifier(List *that_list, const list_functions mode, size_t request)
{
    if (mode == REMOVING)
    {
        if ((size_t)(that_list->length) <= 0)
        {
            ASSERTION(LIST_UNDERFLOW);
            list_print_list_appearance(that_list, LIST_UNDERFLOW, mode);
            return LIST_UNDERFLOW;
        }
    }
    if (request > (that_list->length))
    {
        ASSERTION(LIST_OVERFLOW);
        list_print_list_appearance(that_list, LIST_OVERFLOW, mode);
        return LIST_OVERFLOW;
    }

    return LIST_OK;
}

list_code list_verifier(List *that_list, const list_functions function)
{
    list_code indicator = list_is_pointer_valid(that_list);
    if (indicator != LIST_OK)
    {
        ASSERTION(indicator);
        list_print_list_appearance(that_list, LIST_OK, function);
        return indicator;
    }

    size_t index = that_list->next[0];

    for (size_t i = 0; i < that_list->length; i++)
    {
        if (index != that_list->prev[that_list->next[index]] && i < that_list->length - 1)
        {
            ASSERTION(LIST_CONNECT_ERROR);
            list_print_list_appearance(that_list, LIST_CONNECT_ERROR, function);
            return LIST_CONNECT_ERROR;
        }

        index = that_list->next[index];
    }

    long long connections_number = that_list->capacity - that_list->length;
    long long *free_indexes = (long long*)calloc(that_list->capacity, sizeof(long long));

    index = that_list->first_free;
    for (int i = 0; i < connections_number - 1; i++)
    {
        if (free_indexes[index])
        {
            ASSERTION(LIST_CONNECT_ERROR);
            list_print_list_appearance(that_list, LIST_CONNECT_ERROR, function);
            return LIST_CONNECT_ERROR;
        }
        free_indexes[index]++;

        index = that_list->next[index];
    }
    free(free_indexes);

    list_print_list_appearance(that_list, LIST_OK, function);

    return LIST_OK;
}
