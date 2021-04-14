/**
    \file
    File with functions for using the list
    It has construction, destruction, insertion, deletion, resizing, looking for head and tail, next and previous elements.\n
    It can also sort elements int he order and get the index of needed element.\n
    Thank you for using this program!
    \warning The sortion takes the time of O(n), get_element - O(n) at bad cases. Please, try to put elements at the end of the list.\n
             Function list_construct will cause undefined behavior if you don't alloc the construction of the list before\n
             You should install Graphviz on your PC to make pictures of list
    \authors Anna Savchuk
    \note    To change the type, change the typedef about list_elem_type in AllSettings.c\n
             At the end the last information about the list will be added to log_file\n
*/
#ifndef THE_LIST 
#define THE_LIST

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <wchar.h>
#include <locale.h>

#include "AllSettings.h"

typedef struct The_list
{
    list_elem_type  *data;
    size_t          *next;
    size_t          *prev;

    size_t           length;
    size_t           capacity;

    size_t           first_free;

    char             sorted;
} List;

typedef enum list_code_errors
{
    LIST_OK,
    LIST_NULL,
    LIST_SEG_FAULT,
    LIST_DELETED,
    LIST_NO_CONSTRUCT,
    LIST_UNDERFLOW,
    LIST_OVERFLOW,
    LIST_NO_MEMORY,
    LIST_TOO_BIG,
    LIST_CONNECT_ERROR
} list_code;

extern const char *list_state_text[];

typedef enum list_functions_identifiers
{
    CONSTRUCTION,
    DESTRUCTION,
    INSERTION,
    RESIZING,
    REMOVING,
    FINDING,
    SORTING,
    GETTING
} list_functions;

extern const char *FUNCTION_IDENTIFIERS[];

/*!
Creates a new list and deletes it if the construction failed

@param[in]       list_size                 The size of the list

@param[out]      new_list                  The pointer on the list
*/
List            *list_new                  (size_t list_size);
/*!
Destructs the list

@param[in]       that_list                 The construction of the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_CONNECT_ERROR        If the connection between next array and previous array was broken\n
*/
list_code        list_delete               (List *that_list);
/*!
Constructs the elements of new list

@param[in]       that_list                 The construction of the list
@param[in]       list_size                 The size of the list

Returns          LIST_OK                   If everything is ok
                 LIST_NO_CONSTRUCT         If it was impossible to create the list

\warning         Undefined behavior if you don't alloc the construction of the list
*/
list_code        list_construct            (List *that_list, const size_t list_size);
/*!
Destructs the elements of the list
@param[in]       that_list                 The construction of the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_CONNECT_ERROR        If the connection between next array and previous array was broken\n
*/
list_code        list_destruct             (List *that_list);

/*!
Changes the capacity of the list

@param[in]       that_list                 The construction of the list
@param[in]       coefficient               2, if you want to make list twice bigger, 0.5, if you if you want to make list twice smaller

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_CONNECT_ERROR        If the connection between next array and previous array was broken\n
                 LIST_NO_MEMORY            If there is no memory to resize the list\n
                 LIST_TOO_BIG              If the new capacity is bigger than the maximum size of the type\n
*/
list_code        list_resize               (List *that_list, const double coefficient);
/*!
Inserts the value into the list. It can get indexes from 0 to the number of indexes minus 1, where 0 means insertion to the end.

@param[in]       that_list                 The construction of the list
@param[in]       before_physical_index     The index to put value before
@param[in]       value                     The value to put into the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_insert               (List *that_list, const size_t before_physical_index, const list_elem_type value);
/*!
Inserts the value into the front of the list

@param[in]       that_list                 The construction of the list
@param[in]       value                     The value to put into the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_insert_front         (List *that_list, const list_elem_type value);
/*!
Inserts the value into the back of the list

@param[in]       that_list                 The construction of the list
@param[in]       value                     The value to put into the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_insert_back          (List *that_list, const list_elem_type value);
/*!
Deletes the value of the list
@param[in]       that_list                 The construction of the list
@param[in]       where                     The index to delete value off
@param[in]       value                     The value deleting

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_UNDERFLOW            If the list is empty\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_remove               (List *that_list, const size_t physical_index);
/*!
Removes the value from the front of the list

@param[in]       that_list                 The construction of the list
@param[in]       value                     The value to put into the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_remove_front         (List *that_list);
/*!
Removes the value from the back of the list

@param[in]       that_list                 The construction of the list
@param[in]       value                     The value to put into the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_remove_back          (List *that_list);
/*!
Finds the index that matces to the sought one

@param[in]       that_list                 The construction of the list
@param[in]       where                     The sought index
@param[in]       index                     The index that matches the sought one

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n

\warning         Takes the time of O(n). Try to avoid using this function by putting the elements at the end of the list\n
*/
list_code        list_find_index           (List *that_list, const size_t logical_index, size_t *physical_index);
/*!
Finds the element that is located at the given index

@param[in]       that_list                 The construction of the list
@param[in]       physical_index            The index element is located at
@param[in]       requested_element         The value of the list that was requested

Returns          LIST_OK                   If everything is ok\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code       list_get_element           (List *that_list, const size_t physical_index, list_elem_type *requested_element);

/*!
Finds the index of the first element of the list

@param[in]       that_list                 The construction of the list

@param[out]                                The index of the first element of the list
*/
size_t           list_get_head_index       (List *that_list);
/*!
Finds the index of the last element of the list

@param[in]       that_list                 The construction of the list

@param[out]                                The index of the last element of the list
*/
size_t           list_get_tail_index       (List *that_list);
/*!
Finds the index of the element of the list after the given one

@param[in]       that_list                 The construction of the list
@param[in]       index                     The index of the element is sought for after
@param[in]       next_index                The index of the element is sought for

Returns          LIST_OK                   If everything is ok\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_get_next_index       (List *that_list, size_t physical_index, list_elem_type *next_index);
/*!
Finds the index of the element of the list before the given one

@param[in]       that_list                 The construction of the list
@param[in]       index                     The index the element is sought for before
@param[in]       prev_index                The index of the element is sought for

Returns          LIST_OK                   If everything is ok\n
                 LIST_OVERFLOW             If the index does not exist or can not be added\n
*/
list_code        list_get_prev_index       (List *that_list, size_t physical_index, list_elem_type *prev_index);

/*!
Sorts the elements of the list at the order of usual array

@param[in]       that_list                 The construction of the list

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_NO_MEMORY            If there is no memory to sort the list\n

\warning         Takes the time of O(n). Try to avoid using this function by putting elements at the end of the list.
*/
list_code        list_slow_sort            (List *that_list);

/*!
Prints the errors in the console

@param[in]       code                      The identifier of the error
*/
void             list_print_errors          (list_code code);

/*!
Outputs the information about the current state of the list into "log_file.html"

@param[in]       that_list                 The structure of the list
@param[in]       code                      The code of the mistake
@param[in]       who                       The code of the function requested for dump
*/
void             list_print_list_appearance (List *that_list, list_code code, const list_functions function);

/*!
Checks if all pointers are valid
@param[in]       that_list                 The structure of the list

Returns          LIST_NULL                 If the pointer does not exists\n
                 LIST_SEG_FAULT            If the pointer points in prohibited for using block of memory\n
                 LIST_OK                   If the pointer is valid\n
*/
list_code        list_is_pointer_valid       (List *that_list);
/*!
Checks if the request is valid

@param[in]       that_list                 The structure of the list
@param[in]       mode                      The code of the function requested for dump
@param[in]       request                   The request to check

Returns          LIST_OK                   If everything is ok\n
                 LIST_UNDERFLOW            If the list is empty\n
                 LIST_OVERFLOW             If the index does not exist or can not be added
*/
list_code        list_request_verifier       (List *that_list, const list_functions mode, size_t request);
/*!
Checks all of the states of the list

@param[in]       that_list                 The structure of the list
@param[in]       who                       The code of the function requested for dump

Returns          LIST_OK                   If everything is ok\n
                 LIST_NULL                 If there were not pointers on units of list\n
                 LIST_SEG_FAULT            If memory access denied\n
                 LIST_DELETED              If some of the units were deleted\n
                 LIST_CONNECT_ERROR        If the connection between next array and previous array was broken\n

\warning         Takes the time of O(n). Try to avoid using this function.
*/
list_code       list_verifier              (List *that_list, const list_functions function);

#endif