#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MyList.cpp"

//#define FIRST_TEST
//#define SECOND_TEST
#define THIRD_TEST
//#define FORTH_TEST
//#define FIFTH_TEST
//#define SIXTH_TEST
//#define SEVENTH_TEST

void unit_tests();

int main()
{
    unit_tests();

    return 0;
}

void unit_tests()
{
    #ifdef FIRST_TEST
    {
        printf("The first unit-test\n");
        List* pain = list_new(10);
        printf("cap %lld\n", pain->capacity);

        list_insert(pain, 0, 10);
        list_insert(pain, 1, 20);
        list_insert(pain, 2, 30);

        list_verifier(pain, INSERTION);

        list_insert(pain, 1, 40);

        list_verifier(pain, INSERTION);

        double a = 0;

        get_element(pain, 1, &a);
        printf("%lg\n", a);

        list_destruct(pain);
    }
    #endif

    #ifdef SECOND_TEST
    {
        printf("The second unit-test\n");
        List* pain = list_new(2);

        double a = 0;

        list_insert(pain, 0, 10);
        get_element(pain, 0, &a);
        printf("%lg\n", a);

        list_insert(pain, 1, 11);
        get_element(pain, 1, &a);
        printf("%lg\n", a);

        list_insert(pain, 2, 12);
        a = get_element(pain, 2);
        printf("%lg\n", a);

        list_insert(pain, 3, 13);

        list_destruct(pain);
    }
    #endif

    #ifdef THIRD_TEST
    {
        printf("The third unit-test\n");
        List* pain = list_new(2);

        list_insert(pain, 0, 10);
        list_insert(pain, 1, 11);
        list_insert(pain, 2, 12);
        list_insert(pain, 3, 13);

        list_verifier(pain, INSERTION);

        list_insert(pain, 1, 14);
        list_insert(pain, 1, 15);
        list_insert(pain, 0, 16);

        pain->prev[2] = 0;

        list_verifier(pain, INSERTION);

        list_slow_sort(pain);

        list_verifier(pain, INSERTION);

        list_destruct(pain);
    }
    #endif

    #ifdef FORTH_TEST
    {
        printf("The forth unit-test\n");
        List* pain = list_new(2);

        double a = 0;

        list_insert(pain, 1, 1.0);
        list_verifier(pain, INSERTION);
        list_insert(pain, 2, 1.1);
        list_verifier(pain, INSERTION);
        list_insert(pain, 3, 1.2);
        list_verifier(pain, INSERTION);
        list_insert(pain, 4, 1.3);

        list_verifier(pain, INSERTION);

        list_insert(pain, 2, 1.4);
        list_insert(pain, 2, 1.5);
        list_insert(pain, 1, 1.6);

        list_verifier(pain, INSERTION);
        printf("hello\n");

        list_remove(pain, 1, &a);

        list_verifier(pain, REMOVING);

        printf("my value %lg\n", a);

        list_slow_sort(pain);

        list_verifier(pain, SORTING);

        list_destruct(pain);
    }
    #endif

    #ifdef FIFTH_TEST
    {
        printf("The fifth unit-test\n");
        List *pain  = list_new(2);
        List *konan = list_new(10);

        list_insert(pain, 0, 1.0);
        list_insert(pain, 1, 1.1);
        list_insert(pain, 2, 1.2);
        list_insert(pain, 3, 1.3);

        list_verifier(pain, INSERTION);

        list_insert(konan, 0, 10.4);
        list_insert(konan, 0, 10.5);
        list_insert(konan, 5, 10.6);

        list_verifier(konan, INSERTION);

        list_delete(pain);
        list_delete(konan);
    }
    #endif

    #ifdef SIXTH_TEST
    {
        printf("The sixth unit-test\n");
        List *pain  = list_new(2);

        list_insert(pain, 1, 100);
        list_insert(pain, 2, 101);
        list_insert(pain, 3, 102);
        list_insert(pain, 4, 103);

        list_verifier(pain, INSERTION);

        list_insert(pain, 5, 104);
        list_insert(pain, 2, 105);
        list_insert(pain, 6, 106);

        list_verifier(pain, INSERTION);

        size_t mid_index = 0;
        list_find_index(pain, 2, &mid_index);

        double value = 0;
        list_get_element(pain, mid_index, &value);

        printf("index %u, this element %lf\n", mid_index, value);

        list_remove(pain, mid_index, &value);

        list_verifier(pain, REMOVING);

        list_delete(pain);
    }
    #endif

    #ifdef SEVENTH_TEST
    {
        printf("The seventh unit-test\n");
        List *pain  = list_new(2);

        list_insert_back(pain, 3.14);
        list_insert_back(pain, 0.0015926);

        list_verifier(pain, INSERTION);

        double value = 0;

        list_remove(pain, 1, &value);

        list_verifier(pain, REMOVING);

        list_delete(pain);
    }
    #endif
}

