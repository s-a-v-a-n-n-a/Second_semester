section 		.text

global _start
global MY_PRINTF
extern NUMBER_SUM																;predefined entry point name for ld

buffer_capacity	equ 10000

_start:		
				mov rdi, 1901
				mov rsi, 1901
				call NUMBER_SUM

				mov rdi, format_string
				mov rsi, 3802
				call MY_PRINTF
				
				; push 33
				; push 3802
				; push format_string
				; call PRINTF
				; add rsp, 3 * 8

				mov rax, 0x3C ;exit64
				xor rdi, rdi
				syscall


;-------------------------------------------------------------------------------------------|
;a wrap around printf - pushes parametres in registers (according to system V) to stack		|
;all registers that are required to be saved are saved at the beginning of printf 			|
;																							|
;parameters in order from further: according to system V							 		|		 		 						
;-------------------------------------------------------------------------------------------|
MY_PRINTF:
				pop r11
				
				push r9
				push r8
				push rcx
				push rdx
				push rsi
				push rdi
				call PRINTF
				add rsp, 6 * 8

				push r11
				ret

;-------------------------------------------------------------------------------------------------------------------------------|
;prints the string according to the string inning and parameters in stack												      	|
;																																|
;parameters in order from further: parameters to output (unknown amount), format string 								 		|	
;						      						  																			|
;rax, rdi, rcx, rdx, r9, rsi, r10, r12 are spoiled			     																|
;-------------------------------------------------------------------------------------------------------------------------------|
PRINTF:
				push rbp
				mov rbp, rsp

				push r11

				push rbx
				push r12
				push r13
				push r14
				push r15

				mov r10, 2 ;parameters counter
				xor r12, r12 ;symbols counter

				mov rdi, buffer
				mov rsi, [rbp + 16]

				xor rcx, rcx ;amount of elememts in final buffer
				
READ_FORM:
				
				mov al, byte[rsi]

				cmp al, 0
				je BRAK


				cmp al, '%'
				je READ_OTHER
				jmp CASUAL_PRINTING


;prints lonely procent
PRINT_PROCENT:
				pop rdi
				pop rsi
				pop rcx
				pop r10

				dec rsi
				dec r10

CASUAL_PRINTING:
				mov al, [rsi]
				mov [rdi], al
				
				mov rax, 1
				jmp CHECK
								; inc rdi
				; inc rsi

				; mov rbx, 10000 ;checking if buffer is not full
				; dec rbx
				; cmp r12, rbx
				; ja BRAK

				; inc r12

				; inc rcx

				jmp READ_FORM


READ_OTHER:
				inc r10				
				inc rsi
				xor rax, rax
				mov al, byte[rsi]

				push r10
				push rcx
				push rsi
				push rdi

				mov rbx, jmp_table
				jmp [rbx + rax * 8 - ' ' * 8] 

ADD_OFFSET:

				pop rdi
				pop rsi
				pop rcx
				pop r10

CHECK:
				add rcx, rax
				add rdi, rax

				mov rbx, buffer_capacity ;checking if buffer is not full
				sub rbx, rax
				cmp r12, rbx
				ja BRAK

				add r12, rax

				inc rsi

				jmp READ_FORM


;printing decimal number
DECIMAL:		
				mov rax, [rbp + r10 * 8] 
				call CONVERTER_10
				
				jmp ADD_OFFSET


;printing one symbol
SYMBOL:
				mov rax, [rbp + r10 * 8] 
				mov [rdi], rax
				mov rax, 1

				jmp ADD_OFFSET


;printing string
STRING:			
				xor rcx, rcx
				mov rax, [rbp + r10 * 8] 
				call PRINTMESSAGE
				
				jmp ADD_OFFSET


;printing number in hex format
HEX_NUMBER:
				mov rax, [rbp + r10 * 8] 
				mov rsi, 0xF
				mov rcx, 4
				call BIT_CONVERTER
				
				jmp ADD_OFFSET


;printing number in oct format
OCT_NUMBER:		
				mov rax, [rbp + r10 * 8] 
				mov rsi, 0x7
				mov rcx, 3
				call BIT_CONVERTER
				
				jmp ADD_OFFSET


;printing number in binary format
BIN_NUMBER:		
				mov rax, [rbp + r10 * 8] 
				mov rsi, 0x1
				mov rcx, 1
				call BIT_CONVERTER
				
				jmp ADD_OFFSET

;printing the whole buffer
BRAK:			
				mov rax, 0x01 ;write64
				mov rdi, 1    ;stdout
				mov rsi, buffer
				mov rdx, rcx
				syscall

				pop r15
				pop r14
				pop r13
				pop r12
				pop rbx

				pop r11

				pop rbp
				ret

;-------------------------------------------------------------------------------------------------------------------------------|
;converts number in the notation in parameters and puts the decimal number into the buffer by the offset in parameters	      	|
;																																|
;bit_number in rax, conversion mask in rsi, bits to offset in rcx, buffer offset in rdi, amount of used bytes in r12			|
;						      						  																			|
;length of the string in rax, the sense of r12 must be saved		 		 													|	
;						      						  																			|
;rax, rdi, rcx, rdx, r9 are spoiled			     																				|
;-------------------------------------------------------------------------------------------------------------------------------|
BIT_CONVERTER:

				push rbp
				mov rbp, rsp
				
				mov rdx, rax
				mov r10, rax
				xor r9, r9

CONTINUE_COUNTING:
				
				
				mov rax, rdx
				and rax, rsi
				shr rdx, cl

				inc r9

				cmp rdx, 0
				ja CONTINUE_COUNTING

				mov rbx, buffer_capacity ;checking if buffer is not full
				sub rbx, rcx
				cmp r12, rbx
				ja BIT_BREAK


				mov rdx, r10 
				
				add rdi, r9
				dec rdi

CONTINUE_CONVERTING_16:
				
				mov rax, rdx
				and rax, rsi
				
				cmp rax, 9
				ja ADD_LETTER
				add rax, '0'
				jmp GO_AWAY

ADD_LETTER:		add rax, 'A' - 10

GO_AWAY:
				
				mov [rdi], al
				dec rdi
								
				shr rdx, cl

				cmp rdx, 0
				ja CONTINUE_CONVERTING_16

BIT_BREAK:
				mov rax, r9

				pop rbp
				ret



;---------------------------------------------------------------------------|
;puts the decimal number into the buffer by the offset in parameters      	|
;																			|
;number in rax, buffer offset in rdi, amount of used bytes in r12			|
;						      						  						|
;length of the string in rax, the sense of r12 must be saved		 		|	
;						      						  						|
;rax, rdi, rcx, rdx, r9 are spoiled			     							|
;---------------------------------------------------------------------------|
CONVERTER_10:

				push rbp
				mov rbp, rsp

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

				mov rbx, buffer_capacity ;checking if buffer is not full
				sub rbx, r9
				cmp r12, rbx
				ja BREAK_10


				mov rax, r10
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

				pop rbp
				ret




;-----------------------------------------------------------------------------------|
;counts the length of the string 													|
;																					|
;offset of string is in rdi, amount of used bytes in r12							|
;							 							 							|
;length of the string in rcx, the sense of r12 must be saved		 		 		|	
;							 							 							|	
;rax, rsi, rdi, rcx are spoiled				 										|
;-----------------------------------------------------------------------------------|
STRING_LENGTH:		
			cld

			mov rsi, rdi

			xor al, al 
			xor rcx, rcx
			dec rcx
			repne scasb

			sub rdi, rsi
			mov rcx, rdi
			dec rcx

			ret

;-------------------------------------------------------------------------------------------------------------------|
;puts the message into the buffer by offset in parameters   														|
;																													|
;buffer offset in rdi, message length in rcx, message offset in rax, amount of used bytes in r12					|
;							 							 															|
;length of the string in rax, the sense of r12 must be saved		 		 			 							|	
;							 							 															|	
;rax, rbx, rdi, r9 are spoiled				 																		|
;-------------------------------------------------------------------------------------------------------------------|
PRINTMESSAGE:	
			push rbp
			mov rbp, rsp

			mov r9, rcx
			cmp rcx, 0
			ja PUT_IN_BUFFER

			push rdi
			mov rdi, rax
			push rax
			call STRING_LENGTH
			pop rax
			pop rdi

PUT_IN_BUFFER:
			mov rsi, rax
			mov r9, rcx

			mov rbx, buffer_capacity ;checking if buffer is not full
			sub rbx, rcx
			cmp r12, rbx
			ja DOBREAK

PUT:
			lodsb
			cmp al, 0
			je DOBREAK
			stosb

			loop PUT

DOBREAK:			
			mov rax, r9	

			pop rbp
			ret




section 		.data

format_string:	db "Let us output C language: %x", 0x0A, 0

message:		db "Hello, World", 0									
message_length:	equ $ - message


jmp_table:		
				times ('b' - ' ')	dq PRINT_PROCENT 
									dq BIN_NUMBER
							  		dq SYMBOL
							  		dq DECIMAL
				times ('o'-'d' - 1) dq PRINT_PROCENT
									dq OCT_NUMBER
				times ('s'-'o' - 1) dq PRINT_PROCENT
									dq STRING
				times ('x'-'s' - 1) dq PRINT_PROCENT
									dq HEX_NUMBER
				times (256 - 'x')	dq PRINT_PROCENT


section 		.bss

buffer			resb	buffer_capacity