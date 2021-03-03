section 		.text

global _start																;predefined entry point name for ld

_start:		
				push 33
				push 3802
				push format_string
				call PRINTF
				add rsp, 3 * 8

				mov rax, 0x3C ;exit64
				xor rdi, rdi
				syscall



;-------------------------------------------------------------------------------------------------------------------------------|
;prints the string according to the string inning and parameters in stack												      	|
;																																|
;parameters in order from further: parameters to output (unknown amount), format string 								 		|		 		 																					|	
;						      						  																			|
;rax, rdi, rcx, rdx, r9, rsi, r10 are spoiled			     																	|
;-------------------------------------------------------------------------------------------------------------------------------|
PRINTF:
				push rbp
				mov rbp, rsp

				mov r10, 2 ;parameters counter

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
				
				inc rdi
				inc rsi

				inc rcx

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
				shl rax, 3
				add rbx, rax
				jmp [rbx]

ADD_OFFSET:

				pop rdi
				pop rsi
				pop rcx
				pop r10

				add rcx, rax
				add rdi, rax

				inc rsi

				jmp READ_FORM

DECIMAL:		
				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				call CONVERTER_10
				add rsp, 2 * 8

				jmp ADD_OFFSET

SYMBOL:
				mov rax, [rbp + r10 * 8] 
				mov [rdi], rax
				mov rax, 1

				jmp ADD_OFFSET

STRING:			
				push rdi
				push 0
				mov rax, [rbp + r10 * 8] 
				push rax
				call PRINTMESSAGE
				add rsp, 3 * 8

				jmp ADD_OFFSET


HEX_NUMBER:
				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				push 0xF
				push 4
				call BIT_CONVERTER
				add rsp, 4 * 8

				jmp ADD_OFFSET

OCT_NUMBER:		
				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				push 0x7
				push 3
				call BIT_CONVERTER
				add rsp, 4 * 8

				jmp ADD_OFFSET

BIN_NUMBER:		
				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				push 0x1
				push 1
				call BIT_CONVERTER
				add rsp, 4 * 8

				jmp ADD_OFFSET

BRAK:			
				mov rax, 0x01 ;write64
				mov rdi, 1    ;stdout
				mov rsi, buffer
				mov rdx, rcx
				syscall

				pop rbp
				ret

;-------------------------------------------------------------------------------------------------------------------------------|
;converts number in the notation in parameters and puts the decimal number into the buffer by the offset in parameters	      	|
;																																|
;parameters in order from further: buffer offset, number to convert, convertion mask, power of binary system to convert 		|
;						      						  																			|
;length of the string in rax		 		 																					|	
;						      						  																			|
;rax, rdi, rcx, rdx, r9 are spoiled			     																				|
;-------------------------------------------------------------------------------------------------------------------------------|
BIT_CONVERTER:

				push rbp
				mov rbp, rsp
				
				mov rdx, [rbp + 32] ;bit_number
				xor r9, r9

CONTINUE_COUNTING:
				
				mov rax, rdx
				and rax, [rbp + 24] ;convertion_mask

				mov cl, [rbp + 16] ;byte ptr system_to_convert
				shr rdx, cl

				inc r9

				cmp rdx, 0
				ja CONTINUE_COUNTING


				mov rdx, [rbp + 32] ;bit_number

				mov rdi, [rbp + 40] ;buffer offset
				add rdi, r9
				dec rdi

CONTINUE_CONVERTING_16:
				
				mov rax, rdx
				and rax, [rbp + 24] ;convertion_mask

				cmp rax, 9
				ja ADD_LETTER
				add rax, '0'
				jmp GO_AWAY

ADD_LETTER:		add rax, 'A' - 10

GO_AWAY:
				
				mov [rdi], al
				dec rdi
								
				mov cl, [rbp + 16] ;byte ptr system_to_convert
				shr rdx, cl

				cmp rdx, 0
				ja CONTINUE_CONVERTING_16

				mov rax, r9

				pop rbp
				ret



;---------------------------------------------------------------------------|
;puts the decimal number into the buffer by the offset in parameters      	|
;																			|
;parameters in order from further: buffer offset, number to put 			|
;						      						  						|
;length of the string in rax		 		 								|	
;						      						  						|
;rax, rdi, rcx, rdx, r9 are spoiled			     							|
;---------------------------------------------------------------------------|
CONVERTER_10:

				push rbp
				mov rbp, rsp

				mov rax, [rbp + 16]	;the_number
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


				mov rax, [rbp + 16]	;the_number
				mov rdi, [rbp + 24] ;buffer offset
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

				mov rax, r9 ;current string length

				pop rbp
				ret




;-----------------------------------------------------------|
;counts the length of the string 							|
;															|
;offset of string is in rdi									|
;							 							 	|
;length of the string in rcx		 		 			 	|	
;							 							 	|	
;rax, rsi, rdi, rcx are spoiled				 				|
;-----------------------------------------------------------|
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

;-----------------------------------------------------------------------------------|
;puts the message into the buffer by offset in parameters   						|
;																					|
;parameters in order from further: buffer offset, message length, message offset	|
;							 							 							|
;length of the string in rax		 		 			 							|	
;							 							 							|	
;rax, rbx, rdi, r9 are spoiled				 										|
;-----------------------------------------------------------------------------------|
PRINTMESSAGE:	
			push rbp
			mov rbp, rsp

			mov r9, [rbp + 24] ;message length
			mov rcx, [rbp + 24]
			cmp rcx, 0
			ja PUT_IN_BUFFER

			mov rdi, [rbp + 16] ;message offset
			call STRING_LENGTH

PUT_IN_BUFFER:
			mov rbx, [rbp + 32] ;buffer offset
			mov rdi, [rbp + 16]
			mov r9, rcx

PUT:			
			mov al, byte[rdi]
			mov [rbx], al
			inc rbx
			inc rdi

			loop PUT
			
			mov rax, r9	

			pop rbp
			ret




section 		.data

format_string:	db "first(%%d) : %c aaaaa", 0x0A, 0

message:		db "Hello, World", 0									
message_length:	equ $ - message


jmp_table:		
				times ('b')			dq PRINT_PROCENT 
									dq BIN_NUMBER
							  		dq SYMBOL
							  		dq DECIMAL
				times ('o'-'d') 	dq PRINT_PROCENT
									dq OCT_NUMBER
				times ('s'-'o') 	dq PRINT_PROCENT
									dq STRING
				times ('x'-'s') 	dq PRINT_PROCENT
									dq HEX_NUMBER
				times (256 - 'x')	dq PRINT_PROCENT


section 		.bss

buffer			resb	10000