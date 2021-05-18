section 		.text

global _start
global MY_PRINTF															;predefined entry point name for ld

buffer_capacity	equ 10000

MY_SCANF:
				push rbx
				push rcx
				push rdx
				push rdi
				push rsi
				push r10
				call SCANF
				pop r10
				pop rsi
				pop rdi
				pop rdx
				pop rcx
				pop rbx
				
				ret

SCANF:
				call CONVERTER_INTO_10
				
				ret



CONVERTER_INTO_10:
				xor rax, rax

CONTINUE_COUNTING_DIJITS_IN_STRING:
				
				mov r10, rax 

				xor rax, rax
				mov rdi, 0	
				mov rsi, buffer
				mov rdx, 1
				syscall

				mov rax, r10

				mov rbx, [rsi]
				mov rsi, rbx
				cmp rbx, 10
				jna BRAK_BRAK

				;xor rax, rax
				;mov rax, rcx
				;mov rax, r10

				mov rbx, 10 
				mul rbx
				sub rsi, '0'
				add rax, rsi


				jmp CONTINUE_COUNTING_DIJITS_IN_STRING

BRAK_BRAK:
				ret

section .bss
bufer	resb	10000
output_buffer	resb	10000

