section .bss
        num1: resb 1
        num2: resq 1
        num3: resq 1
        num4: resq 1

section .data                    	; data section, read-write
        %define first_num rdi
        %define second_num rsi
        %define result_num rdx
        ; RDI RSI RDX RCX R8 R9

section .text                    	; our code is always in the .text section
        global big_add          	    ; makes the function appear in global scope
        global big_sub
        global div_by_two

	big_add:
		enter 0, 0
        mov rax, 0
        mov byte[num1], 0                       ; carry flag
        mov r15, 0                              ; new num size
        mov r10, qword[first_num] ; size of the first_num
        mov r11, qword[second_num] ; of the second_num
        mov r12, qword[result_num] ; of the result_num
        mov rcx, [first_num+8] ; first char first num
        mov r8, [second_num+8] ; first char second num
        mov r9, [result_num+8] ; first char result num
        add rcx, r10
        add r8, r11
        add r9, r12
        mov byte[rcx], 0
        dec rcx
        dec r8
        dec r9
        mov r13, r10
        cmp r13, r11
        jle .start_loop
        xchg r13, r11
        xchg rcx, r8
        .start_loop:
        inc r15
        mov r14b, 0
        cmp r13, 0
        jle .con
        mov r14b, byte[rcx]
        sub r14b, 48
        .con:
        cmp r11, 0
        jle .con2
        add r14b, byte[r8]
        sub r14b, 48
        .con2:
        add r14b, byte[num1]
        cmp r14b, 9
        jg .save_carry
        mov byte[num1], 0
        
        .save_num:
        add r14b, 48
        mov byte[r9], r14b
        cmp r13, 0
        jle .skip_first_dec
        dec r13
        dec rcx
        
        .skip_first_dec:
        cmp r11, 0
        jle .skip_second_dec
        dec r11
        dec r8
        
        .skip_second_dec:
        cmp r12, 0
        jle .cont_loop
        dec r12
        dec r9
        
        .cont_loop:
        cmp r11 ,0
        jle add_end_loop
        jmp .start_loop
        
        .save_carry:
        sub r14b, 10
        mov byte[num1],1
        jmp .save_num
        
        add_end_loop:
        cmp byte[num1], 1
        jl .finish
        mov byte[r9], 49
        dec r9
        jmp .finish2
        .finish:
        mov byte[r9], 48
        dec r9
        .finish2:
        inc r15
        inc r9
        mov qword[result_num], r15
        mov rax, r9
        leave
        ret
        
	big_sub:
		enter 0, 0
        mov rax, 0
        mov byte[num1], 0						; borrow flag
        mov r15, 0									; new num size
        mov r10, qword[first_num]			; size of the first_num
        mov r11, qword[second_num]		; of the second_num
        mov r12, qword[result_num]		; of the result_num
        mov rcx, [first_num+8]				; first char first num
        mov r8, [second_num+8]			; first char second num
        mov r9, [result_num+8]				; first char result num
        add rcx, r10
        add r8, r11
        add r9, r12
        mov byte[rcx], 0
        dec rcx
        dec r8
        dec r9
        mov r13, r10
        .start_loop:
        inc r15
        mov r14b, 0
        cmp r13, 0
        jle .con
        mov r14b, byte[rcx]
        sub r14b, 48
        .con:
        cmp r11, 0
        jle .con2
        sub byte[r8], 48
        sub r14b, byte[r8]
        add byte[r8], 48
        .con2:
        sub r14b, byte[num1]
        cmp r14b, 0
        js .save_borrow
        mov byte[num1], 0
        
        .save_num:
        add r14b, 48
        mov byte[r9], r14b
        cmp r13, 0
        jle .skip_first_dec
        dec r13
        dec rcx
        
        .skip_first_dec:
        cmp r11, 0
        jle .skip_second_dec
        dec r11
        dec r8
        
        .skip_second_dec:
        cmp r12, 0
        jle .cont_loop
        dec r12
        dec r9
        
        .cont_loop:
        cmp r13 ,0
        jle sub_end_loop
        jmp .start_loop
        
        .save_borrow:
        add r14b, 10
        mov byte[num1],1
        jmp .save_num
        
        sub_end_loop:
        cmp byte[num1], 1
        jne .finish
        mov byte[r9], 49
        dec r9
        jmp .finish2
        .finish:
        mov byte[r9], 48
        dec r9
        .finish2:
        inc r15
        inc r9
        mov qword[result_num], r15
        mov rax, r9
		leave
		ret 
		
		div_by_two:
			nop
			enter 0, 0
			mov rcx, qword[first_num]				; length of dividend 
			mov r8, [first_num+8]						; poniter to first digit of dividend
			mov r9, qword[second_num]			; length of answer
			dec r9												; need length - 1
			mov r11, [second_num+8]				; pointer to first slot of answer
			mov ax, 0											; init temp register
			mov bx, 0											; init divisor register
			mov bl, 2											; insert 2 to divisor
			mov r12, 1										; flag = 1
			mov r13, 0										; i
		
			.prepare_first_digit:
				mov al, byte[r8]								; al holds first digit
				sub al, '0'											; al holds value of first digit
				div bl												; div al by 2
				add al, '0'											; al holds first digit of answer
				mov byte[r11], al								; insert first digit to answer
				inc r11												; step to next digit in answer
				mov ax, 0											; init temp register
			
			.while_loop:
				cmp r13, r9										; while i < length - 1
				je .finish
				mov al, byte[r8]								; al holds ith digit
				sub al, '0'											; convert to value
				and al, 01h										; AND with 0000 0001 to check if odd or even
				jnz .odd
				mov r12, 0										; if even { flag = 0 }
			.odd:
				mov al, 0											; init temp
				inc r8												; step to next digit
				mov al, byte[r8]								; al holds (i+1)th digit
				sub al, '0'											; convert to value
				cmp r12, 1										; if flag, add 10
				jne .dont_add_10
				add al, 10
			.dont_add_10:
				div bl												; div al by 2
				add al, '0'											; convert al to digit
				mov byte[r11], al								; save al to answer
				mov ax, 0											; init temp
				mov r12, 1										; flag = 1
				inc r13												; i++
				inc r11												; step to next digit in answer
				jmp .while_loop								; loop
				
			.finish:
				mov rax, 0										; init rax
				mov rax, [second_num+8]				; return pointer to start of answer string
				leave
				ret
				