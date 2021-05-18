// #include "Instructions_realizations.h"
#include "Translation.h"


int main()
{
	translation_into_x86_64("binary.xex", "output");

	system("chmod +x ./output");

	return 0;
}
