extern "C" void MY_PRINTF(const char *format_string, ...);

extern "C" int NUMBER_SUM(int a, int b)
{
	return a + b;
}

int main()
{
	MY_PRINTF("I study on the %d in MIPT which was founded in %x (%s)\n and I %s %x %d%%%c%b", 1, 1951, "in hex :)", "LOVE", 3802, 100, 33, 255);
	
	return 0;
}