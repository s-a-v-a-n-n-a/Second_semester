#include <ctime>

#include "hash_map.h"

int main()
{
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	unsigned start_time = clock();
	test_hash_map();
	unsigned end_time = clock();
	printf("Time gone: %f sec\n", (float)(end_time - start_time));

	return 0;
}