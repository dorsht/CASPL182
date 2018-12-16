section .bss
    SIC_code: resq 1
    code_length: resq 1
    temp_scanf_read: resq 1

section .data                    
    print_form: db "%ld ", 0
    input_form: db "%ld" , 32, 0
    new_line: db 10

section .text
    extern printf, scanf, calloc, free, exit
    global main

    main:
        enter 0,0
        xor r11,r11
        .get_input:

            xor rax, rax
            mov rdi, input_form
            mov rsi, temp_scanf_read
            push r11
            call scanf
            cmp rax, 1
            jne .create_sic_code_array
            pop r11
            mov rsi, qword[temp_scanf_read]
            push rsi
            inc r11
            jmp .get_input

        .create_sic_code_array:
            pop r11
            ;dec r11
            mov rdi, r11
            inc rdi
            mov rsi, 8
            push r11
            call calloc
            cmp rcx, 0
            jz .failed_to_calloc
            mov qword[SIC_code], rax
            pop r11
            mov qword[code_length], r11
            dec r11
        .fill_sic_code_array:

            lea r9, [rax+r11*8]
            pop r10
            mov qword[r9], r10
            dec r11
            cmp r11, 0
            jge .fill_sic_code_array

            mov rax, qword[SIC_code]
            xor rbx,rbx

        .for:
            push rbx
            push rax
            lea rax, [rax+rbx*8]
            nop
            mov rbx, qword[rax]
            mov rcx, qword[rax+8]
            mov rdx, qword[rax+16]
            mov rdi, rbx
            add rdi, rcx
            add rdi, rdx
            cmp rdi, 0
            jz .finish_loop

            ;;; rbx = M[A] ;;;
            pop rax
            push rax
            lea rax, [rax+rbx*8]
            mov rbx, qword[rax]

            ;;; rsi = *A ;;;
            mov rsi, rax

            ;;; rcx = M[B] ;;;
            pop rax
            push rax
            lea rax, [rax+rcx*8]
            mov rcx, qword[rax]

            ;;; M[A] = M[A] - M[B] ;;;
            pop rax
            sub rbx, rcx
            mov qword[rsi], rbx

            ;; if M[A] < 0 go to C ;;
            cmp rbx, 0
            pop rbx
            jl .go_to_C

            ;; else next line
            .C_is_i_plus_3:
                add rbx, 3
                jmp .for

            .go_to_C:
                mov rbx, rdx
                jmp .for

        .finish_loop:
            mov rbx, qword[SIC_code]
            xor rcx, rcx
            .print_loop:
            push rcx
            push rbx
            mov rax, 0
            mov rdi, print_form
            lea r8, [rbx+rcx*8]
            mov rsi, qword[r8]
            call printf
            pop rbx
            pop rcx
            inc rcx
            mov r14, qword[code_length]
            cmp rcx, r14
            jle .print_loop
            mov rdi, new_line
            xor rax,rax
            call printf

            xor rax,rax
            mov rdi, qword[SIC_code]
            call free
            leave
            ret

        .failed_to_calloc:
            leave
            ret