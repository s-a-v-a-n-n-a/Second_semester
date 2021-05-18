section 		.text

global _start
global MY_PRINTF															;predefined entry point name for ld

buffer_capacity	equ 10000

_start:
MY_PRINTF:
				push rbx
				push rcx
				push rdx
				push r9
				push r10
				push rdi
				push rsi
				call PRINTF
				pop rsi
				pop rdi
				pop r10
				pop r9
				pop rdx
				pop rcx
				pop rbx
				
				ret

PRINTF:
		
				;mov rax, rdi
				call CONVERTER_10

				mov rcx, rax

				mov rax, 0x01 ;write64
				mov rdi, 1    ;stdout
				mov rsi, output_buffer
				mov rdx, rcx
				syscall

				ret




CONVERTER_10:
				mov r10, rax
				xor rcx, rcx

				xor r9, r9

				std
CONTINUE_COUNTING_DIJITS:
				
				xor rdx, rdx
				mov rbx, 10
				div rbx

				inc r9

				cmp rax, 0
				ja CONTINUE_COUNTING_DIJITS


				mov rax, r10
				mov rdi, output_buffer
				add rdi, r9
				dec rdi

CONTINUE_CONVERTING_10:
				
				xor rdx, rdx
				mov rbx, 10
				div rbx
				
				push rax
				mov rax, rdx

				add al, '0'
				mov byte[rdi], al
				dec rdi
				
				pop rax
				cmp rax, 0
				ja CONTINUE_CONVERTING_10

BREAK_10:
				mov rax, r9 ;current string length

				ret


section 		.bss

output_buffer			resb	buffer_capacity