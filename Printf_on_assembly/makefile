all:
	nasm -f elf64 -l printf_func.lst printf_func.asm

asm:
	nasm -f elf64 -l printf_func.lst printf_func.asm
	ld -o printf_func printf_func.o 
	./printf_func

run: all
	gcc -no-pie printf_func.o C_code_for_asm_printf.cpp
	./a.out