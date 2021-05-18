section 		.text

global _start
global MY_SCANF															;predefined entry point name for ld

buffer_capacity	equ 10000

MY_SQRT:
				push rbx
				push rcx
				push rdx
				push rdi
				push rsi
				call SQRT
				pop rsi
				pop rdi
				pop rdx
				pop rcx
				pop rbx

				ret

SQRT:
		
				;Пусть всё сразу будет в rax
				;mov rax, rdi
				
				mov rsi, rax ;rsi - tmp

				xor rbx, rbx ;нижняя граница
				mov rdx, rax ;верхняя граница
				xor rax, rax

COUNT_SQRT:
				cmp rbx, rdx
				jae SQRT_BRAK

				push rbx
				push rdx

				mov r11, rbx
				add r11, rdx ; a + b

				shr r11, 1


				mov rax, r11; (a + b)/2
				mul rax ;square of result

				pop rdx
				pop rbx

				cmp rax, rsi
				je SQRT_BRAK
				ja RIGHT_BOUND

				mov rbx, r11
				inc rbx
				mov rax, rbx


				jmp COUNT_SQRT

RIGHT_BOUND:
				mov rdx, r11
				mov rax, rdx

				jmp COUNT_SQRT


SQRT_BRAK:
				mov rax, r11

				mul rax
				cmp rax, rsi
				jbe OTHER_BRAK
				dec r11

OTHER_BRAK:
				mov rax, r11
				ret
