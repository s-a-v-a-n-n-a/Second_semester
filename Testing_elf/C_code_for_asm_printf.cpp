#include <cstdio>
#include <cstdlib>

extern "C" int MY_SCANF();

int main()
{
	int a = MY_SCANF();
	printf("%d\n", a);

	return 0;
}