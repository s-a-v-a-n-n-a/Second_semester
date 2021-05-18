
all:
	g++ -no-pie -g -c -o C_code_for_asm_printf.obj C_code_for_asm_printf.cpp
	nasm -f elf64 -o IN.o IN.asm
	ld IN.o C_code_for_asm_printf.obj -o SCAN -lc
	./SCAN