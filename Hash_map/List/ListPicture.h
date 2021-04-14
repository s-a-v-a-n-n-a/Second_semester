#include "AllSettings.h"
#include "MyList.h"

/*!
Calls the dot-program to make the picture by graphviz
@param[in]       name_file                 The name of file to make a picture from
@param[in]       new_name                  The format of the picture
*/
void             list_call                 (const char *name_file, const char *new_name);
/*!
Outputs the information about the adding nodes of the list
@param[in]       picture                   The file to print code for graphviz into
@param[in]       that_list                 The structure of the list
@param[in]       state                     The indicator if the next-array is printed or the previous one
@param[in]       color                     The color of edges to connect the nodes
*/
void             print_sequence            (FILE *picture, List *that_list, const char state, const char *color);
/*!
Outputs the information about the main nodes of the list
@param[in]       picture                   The file to print code for graphviz into
@param[in]       that_list                 The structure of the list
@param[in]       start                     The counter of the array to start with
@param[in]       end                       The length of the array
@param[in]       index                     The index of the first element
*/
void             print_boxes               (FILE *picture, List *that_list, size_t start, size_t end, size_t index, size_t last);
/*!
Makes the form of an element of the list
@param[in]       picture                   The file to print the code for graphviz
@param[in]       that_list                 The structure of the list
@param[in]       index_color               Color of the index
@param[in]       fill_color                Color of the box
@param[in]       index                     The index of element to make form of
*/
void             make_box                  (FILE *picture, List *that_list, const char *index_color, const char *fill_color, size_t index);
/*!
Outputs the information about the current state of the list logically into the file
@param[in]       that_list                 The structure of the list
@param[in]       picture_name              The name of the file to print the code for graphviz

\warning         Please, avoid using this function
*/
void             list_slow_print_logical_state  (List *that_list, const char* picture_name);
/*!
Outputs the information about the current state of the list as it is really into the file
@param[in]       that_list                 The structure of the list
@param[in]       picture_name              The name of the file to print the code for graphviz
*/
void             list_print_physical_state (List *that_list, const char* picture_name);
