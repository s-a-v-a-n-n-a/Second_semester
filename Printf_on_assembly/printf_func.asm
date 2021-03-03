section 		.text

global _start																;predefined entry point name for ld

_start:		
				; mov rax, 0x01												;write64
				; mov rdi, 1													;stdout
				; mov rsi, message
				; mov rdx, message_length
				; syscall

				; push 0xEDA
				; call CONVERTER_10
				; pop rax


				push message
				push 3802
				push format_string
				call PRINTF
				add rsp, 3 * 8

				
				; push 0
				; push message
				; call PRINTMESSAGE
				; pop rax
				; pop rax

				mov rax, 0x3C												;exit64
				xor rdi, rdi
				syscall

PRINTF:

				push rbp
				mov rbp, rsp

				mov r10, 2

				mov rdi, buffer
				mov rsi, [rbp + 16]

				xor rcx, rcx
				
READ_FORM:
				
				mov al, byte[rsi]


				; push rax
				; push rdx
				; push rsi
				; push rdi

				; mov rax, 0x01												;write64
				; mov rdi, 1													;stdout
				; mov rsi, message
				; mov rdx, message_length
				; syscall

				; pop rdi
				; pop rsi
				; pop rdx
				; pop rax


				cmp al, 0
				je BRAK


				cmp al, '%'
				je READ_OTHER

				mov al, [rsi]
				mov [rdi], al
				
				inc rdi
				inc rsi

				inc rcx

				jmp READ_FORM


READ_OTHER:
				inc r10				;parameters counter
				inc rsi
				mov al, byte[rsi]

				; push r10
				; push rcx
				; push rsi
				; push rdi

				cmp al, 'd'
				je DECIMAL

				cmp al, 'c'
				je SYMBOL

				cmp al, 's'
				je STRING

				cmp al, 'x'
				je HEX_NUMBER

				cmp al, 'o'
				je OCT_NUMBER

				cmp al, 'b'
				je BIN_NUMBER

				; pop rdi
				; pop rsi
				; pop rcx
				; pop r10

				; add rcx, rax
				; add rdi, rax

				; inc rsi

DECIMAL:		
				; push rax
				; push rbx
				; push rcx
				; push rdx

				; mov rax, 0x01												;write64
				; mov rdi, 1													;stdout
				; mov rsi, message
				; mov rdx, message_length
				; syscall

				; pop rdx
				; pop rcx
				; pop rbx
				; pop rax

				push rcx
				push rsi
				push rdi

				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				call CONVERTER_10
				add rsp, 2 * 8

				pop rdi
				pop rsi
				pop rcx

				add rcx, rax
				add rdi, rax

				inc rsi

				jmp READ_FORM

SYMBOL:
				push rcx
				push rsi
				push rdi

				push rdi
				push 1
				mov rax, [rbp + r10 * 8] 
				push rax
				call PRINTMESSAGE
				add rsp, 3 * 8

				pop rdi
				pop rsi
				pop rcx

				add rcx, rax
				add rdi, rax

				inc rsi

				jmp READ_FORM

STRING:			
				push rcx
				push rsi
				push rdi

				push rdi
				push 0
				mov rax, [rbp + r10 * 8] 
				push rax
				call PRINTMESSAGE
				add rsp, 3 * 8

				pop rdi
				pop rsi
				pop rcx

				add rcx, rax
				add rdi, rax

				inc rsi

				jmp READ_FORM


HEX_NUMBER:
				push rcx
				push rsi
				push rdi

				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				push 0xF
				push 4
				call BIT_CONVERTER
				add rsp, 4 * 8

				pop rdi
				pop rsi
				pop rcx

				add rcx, rax
				add rdi, rax

				inc rsi

				jmp READ_FORM

OCT_NUMBER:		
				push rcx
				push rsi
				push rdi

				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				push 0x7
				push 3
				call BIT_CONVERTER
				add rsp, 4 * 8

				pop rdi
				pop rsi
				pop rcx

				add rcx, rax
				add rdi, rax

				inc rsi

				jmp READ_FORM

BIN_NUMBER:		
				push rcx
				push rsi
				push rdi

				push rdi
				mov rax, [rbp + r10 * 8] 
				push rax
				push 0x1
				push 1
				call BIT_CONVERTER
				add rsp, 4 * 8

				pop rdi
				pop rsi
				pop rcx

				add rcx, rax
				add rdi, rax

				inc rsi

				jmp READ_FORM
				;короче берём смещение первого аргумента (rbp + 6)
				;гоняем до него и выводим, пока не %
				;берём следующий аргумент и выводим его в зависимости от того, что после %
				;и так, пока строчка не кончится
				;в rcx будем хранить текущую длину буфера

BRAK:			
				mov rax, 0x01												;write64
				mov rdi, 1													;stdout
				mov rsi, buffer
				mov rdx, rcx
				syscall

				pop rbp
				ret

;-----------------------------------------------------|
;converts number in the notation in parameters	      |
;						      						  |
;x coordinate in ax, y coordinate in cx   	          |
;						      						  |
;ax, di, cx, dx are spoiled			     			  |
;-----------------------------------------------------|
BIT_CONVERTER:	;system_to_convert, convertion_mask, bit_number

				push rbp
				mov rbp, rsp
				push rcx
				
				mov rdx, [rbp + 32];bit_number
				;mov rdx, 10
				xor r9, r9

				;std
CONTINUE_COUNTING:
				
				mov rax, rdx
				and rax, [rbp + 24];convertion_mask

				mov cl, [rbp + 16];byte ptr system_to_convert
				shr rdx, cl

				inc r9

				cmp rdx, 0
				ja CONTINUE_COUNTING


				mov rdx, [rbp + 32];bit_number

				mov rdi, [rbp + 40]
				add rdi, r9
				dec rdi

CONTINUE_CONVERTING_16:
				
				mov rax, rdx
				and rax, [rbp + 24];convertion_mask

				cmp rax, 9
				ja ADD_LETTER
				add rax, '0'
				jmp GO_AWAY

ADD_LETTER:		add rax, 'A' - 10

GO_AWAY:
				
				mov [rdi], al
				dec rdi
								
				mov cl, [rbp + 16];byte ptr system_to_convert
				shr rdx, cl

				cmp rdx, 0
				ja CONTINUE_CONVERTING_16


				; mov rax, 0x01												;write64
				; mov rdi, 1													;stdout
				; mov rsi, number;[rbp + 24]
				; mov rdx, r9
				; syscall


				mov rax, r9

				pop rcx
				pop rbp
				
				ret




CONVERTER_10:;	proc x_coordinate, y_coordinate, the_number

				push rbp
				mov rbp, rsp

				mov rax, [rbp + 16];the_number
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


				mov rax, [rbp + 16];the_number
				mov rdi, [rbp + 24]
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


				; mov rax, 0x01												;write64
				; mov rdi, 1													;stdout
				; mov rsi, number
				; mov rdx, r9
				; syscall

				mov rax, r9 												;current string length


				pop rbp
				
				ret




STRLEN:		
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

;--------------------------------------------------------|
;prints the whole message by addressing to videomemory   |
;							 							 |
;x coordinate in ax, y coordinate in cx			 		 |	
;							 							 |	
;ax, bx, di, si are spoiled				 				 |
;--------------------------------------------------------|
PRINTMESSAGE:	;proc Message_offset, MesLen, format_offset
			push rbp
			mov rbp, rsp

			; mov ah, 0dh
			; mov si, Message_offset
			; mov di, VIDEOSEG
			; mov es, di
			mov rax, 1
			mov r9, [rbp + 24]
			cmp r9, 0
			ja OTHER_PRINT

			mov rdi, [rbp + 16]
			call STRLEN

			mov rbx, [rbp + 32]
			mov rdi, [rbp + 16]
			mov r9, rcx

PUT_IN_BUFFER:
			mov al, byte[rdi]
			mov [rbx], al
			inc rbx
			inc rdi

			loop PUT_IN_BUFFER
			; mov rax, 0x01												;write64
			; mov rdi, 1													;stdout
			; mov rsi, [rbp + 16]
			; dec rcx
			; mov rdx, rcx
			; syscall
			mov rax, r9

			jmp EXIT_PRINTING

OTHER_PRINT:

			mov rbx, [rbp + 32]
			mov rdi, [rbp + 16]
			; mov rax, 0x01												;write64
			; mov rdi, 1													;stdout
			; mov rsi, [rbp + 16]
			; mov rdx, r9
			; syscall

EXIT_PRINTING:	

			pop rbp
			ret




section 		.data

buffer:			times 10000 db 'b'

format_string:	db "first(%o) : %s", 0

message:		db "Hello, World", 0									;who is 0x0a
message_length:	equ $ - message