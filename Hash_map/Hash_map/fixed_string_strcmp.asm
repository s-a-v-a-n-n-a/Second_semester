section 		.text

global STRING_COMPARE

_start:	

;-------------------------------------------------------------------------------------------
;Compares two 32byte strings and returns 0 if they are equal, and non-zero if not
;Parameters[in] are in rdi and rsi, according to System V calling convention			
;																							
;Result[out] in rax, according to System V calling convention 
;rax, ymm0, ymm1 are spoiled							 				 		 						
;-------------------------------------------------------------------------------------------
STRING_COMPARE:
				xor rax, rax				;movemask works with 32-bits register

				vmovdqu ymm0, [rdi]			;Load 256-bits of integer data from memory
		        vmovdqu ymm1, [rsi]

		        vpcmpeqq ymm0, ymm0, ymm1	;Compare packed 64-bit integers in a and b for equality, and store the results in dst
		        vpmovmskb eax, ymm0			;Create mask from the most significant bit of each 8-bit element in a, and store the result in dst
		        not eax						
				ret
