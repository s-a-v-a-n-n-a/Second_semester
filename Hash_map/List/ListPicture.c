#include "ListPicture.h"

void list_call(const char *name_file, const char *new_name)
{
    char temp[512];
    sprintf(temp, "dot %s -T%s -O", name_file, new_name);
    system((char*)temp);
}

void print_sequence(FILE *picture, List *that_list, const char state, const char *color)
{
    size_t index = 0;

    fprintf(picture, "  ");

    size_t *printed_array = NULL;
    if (state == 'n')
    {
        printed_array = that_list->next;
    }
    else
    {
        printed_array = that_list->prev;
    }

    for (size_t i = 0; i < that_list->length + 1; i++)
    {
        fprintf(picture, "nod%lu:%c%lu->", index, state, index);

        index = printed_array[index];
    }
    fprintf(picture, "nod%lu:%c%lu", index, state, index);
    fprintf(picture, "[color=\"%s\", arrowsize = \"1\"]", color);
    fprintf(picture, ";\n");
}

void print_boxes(FILE *picture, List *that_list, size_t start, size_t end, size_t index, size_t last)
{
    for (size_t i = start; i < end; i++)
    {
        fprintf(picture, "nod%lu->", index);

        index = that_list->next[index];
    }
    fprintf(picture, "nod%lu", last);
    fprintf(picture, "[color = \"Black\"];\n");
}

void make_box(FILE *picture, List *that_list, const char *index_color, const char *fill_color, size_t index)
{
    fprintf(picture, "  nod%lu[shape=\"none\" ", index);
    if (index == 0)
    {
        fprintf(picture, "      label = <<table border = \"0\" cellborder = \"1\" cellspacing = \"0\">\n   <tr>\n      <td bgcolor = \"%s\" rowspan = \"3\">%lu</td>\n", COLOR_FICTIVE_INDEX, index);
        fprintf(picture, "      <td bgcolor =  \"%s\" port = \"n%lu\">%lu</td>\n", COLOR_FICTIVE_FILL, index, that_list->next[index]);
        fprintf(picture, "      </tr>\n      <tr>\n");
        fprintf(picture, "      <td bgcolor =  \"%s\">"list_elem_specifier"</td>\n", COLOR_FICTIVE_FILL, that_list->data[index]);
        fprintf(picture, "      </tr>\n      <tr>\n");
        fprintf(picture, "      <td bgcolor =  \"%s\" port = \"p%lu\">%lu</td>\n", COLOR_FICTIVE_FILL, index, that_list->prev[index]);
    }
    else
    {
        fprintf(picture, "      label = <<table border = \"0\" cellborder = \"1\" cellspacing = \"0\">\n   <tr>\n      <td bgcolor = \"%s\" rowspan = \"3\">%lu</td>\n", index_color, index);
        fprintf(picture, "      <td bgcolor =  \"%s\" port = \"n%lu\">%lu</td>\n", fill_color, index, that_list->next[index]);
        fprintf(picture, "      </tr>\n      <tr>\n");
        fprintf(picture, "      <td bgcolor =  \"%s\">"list_elem_specifier"</td>\n", fill_color, that_list->data[index]);
        fprintf(picture, "      </tr>\n      <tr>\n");
        fprintf(picture, "      <td bgcolor =  \"%s\" port = \"p%lu\">%lu</td>\n", fill_color, index, that_list->prev[index]);
    }
    fprintf(picture, "      </tr>\n      </table>>];\n");
}

void list_slow_print_logical_state(List *that_list, const char* picture_name)
{
    FILE* picture = fopen(picture_name, "wb");

    fprintf(picture, "digraph list\n{\n  rankdir = LR;\n");

    size_t index = 0;

    char next_bad = 0;
    char now_bad  = 0;
    for (size_t i = 0; i < that_list->length + 1; i++)
    {
        if (next_bad)
        {
            next_bad = 0;
            now_bad  = 1;
        }
        else
            now_bad = 0;

        if (index != that_list->prev[that_list->next[index]] || (that_list->next[index] == 0 && i < that_list->length))
        {
            next_bad = 1;
        }

        if (index != that_list->next[that_list->prev[index]] || index != that_list->prev[that_list->next[index]])
            make_box(picture, that_list, COLOR_INDEX_BAD, COLOR_FILL_BAD, index);
        else
            make_box(picture, that_list, COLOR_INDEX, COLOR_FILL, index);

        index = that_list->next[index];
    }

    fprintf(picture, "  ");

    print_boxes(picture, that_list, 0, that_list->length + 1, index, 0);

    print_sequence(picture, that_list, 'n', COLOR_EDGE_OF_NEXT);
    print_sequence(picture, that_list, 'p', COLOR_EDGE_OF_PREV);

    index = that_list->first_free;
    for (size_t i = that_list->length + 1; i <= that_list->capacity; i++)
    {
        make_box(picture, that_list, COLOR_INDEX_ADD, COLOR_FILL_ADD, index);

        index = that_list->next[index];
    }
    index = that_list->first_free;
    fprintf(picture, "  nod%lu->", that_list->prev[0]);

    print_boxes(picture, that_list, that_list->length + 1, that_list->capacity + 1, index, that_list->capacity);

    fprintf(picture, "}");
    fclose(picture);

    list_call(picture_name, PICTURE_FORMAT);
}

void list_print_physical_state(List *that_list, const char* picture_name)
{
    FILE* picture = fopen(picture_name, "wb");

    fprintf(picture, "digraph list\n{\n  rankdir = LR;\n");

    for (size_t i = 0; i <= that_list->capacity; i++)
    {
        //isnan(that_list->data[i])
        if (that_list->data[i] == neutral)
        {
            make_box(picture, that_list, COLOR_INDEX_ADD, COLOR_FILL_ADD, i);
        }
        else if (i != that_list->next[that_list->prev[i]] || i != that_list->prev[that_list->next[i]])
        {
            make_box(picture, that_list, COLOR_INDEX_BAD, COLOR_FILL_BAD, i);
        }
        else
        {
            make_box(picture, that_list, COLOR_INDEX, COLOR_FILL, i);
        }
    }
    for (size_t i = 0; i <= that_list->capacity; i++)
    {
        fprintf(picture, "nod%lu->", i);
    }
    fprintf(picture, "nod%lu", that_list->capacity);
    fprintf(picture, "[color = \"Black\"];\n");

    print_sequence(picture, that_list, 'n', COLOR_EDGE_OF_NEXT);
    print_sequence(picture, that_list, 'p', COLOR_EDGE_OF_PREV);

    fprintf(picture, "}");
    fclose(picture);

    list_call(picture_name, PICTURE_FORMAT);
}
