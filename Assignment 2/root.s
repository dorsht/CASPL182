section .bss
    re: resq 1
    im: resq 1
    first_re: resq 1
    sec_re: resq 1
    third_re: resq 1
    first_im: resq 1
    sec_im: resq 1
    third_im: resq 1
    epsilon: resq 1
    order: resq 1
    initial_re: resq 1
    initial_im: resq 1
    coeff_re: resq 1
    coeff_im: resq 1
    coeff_place: resq 1
    before_derivative_re: resq 1
    after_derivative_re: resq 1
    before_derivative_im: resq 1
    after_derivative_im: resq 1
    z_n_re: resq 1
    z_n_im: resq 1
    re_array: resq 1
    im_array: resq 1
    der_re_array: resq 1
    der_im_array: resq 1
    horner_re_array_pointer: resq 1
    horner_im_array_pointer: resq 1
    horner_order: resq 1
section .data                    
    str: db "root = %0.20lf %0.20lf" , 10, 0
    read_epsilon:
        db "epsilon = %lf", 0
    read_order:
        db 10, "order = %lu", 0
    read_coeff:
        db 10, "coeff %lu = %lf %lf", 0
    read_initial:
        db 10, "initial = %lf %lf", 0
    error_message_order:
        db "Error: the minimum order is 1!", 10, 0
    error_message_array:
        db "Error: cant allocate array(s)", 10, 0
    error_message_division:
        db "Error: cant divide by zero", 10, 0
    zero:
        dq 0.0
    horner_re:
        dq 0.0
    horner_im:
        dq 0.0
    float_counter:
        dq 2.0

section .text
    extern printf, scanf, calloc, free
    global main

    main:
    enter 0,0

    .read_epsilon:
    mov rax, 0
    mov rdi, read_epsilon
    mov rsi, epsilon
    call scanf

    .read_order:
    mov rax, 0
    mov rdi, read_order
    mov rsi, order
    call scanf
    mov rax, 0
    
    mov r15, qword[order]
    cmp r15, 0
    jg .cont
    ;; we'll continue if the order > 0, otherwise we'll print error and stop
    mov rdi, error_message_order
    call printf
    leave
    jmp finish
    
    .cont:
    inc r15
    mov qword[order], r15
    .create_polinomial:
    ; allocate re_array
    mov rdi, r15
    mov rsi, 8
    call calloc
    cmp rcx, 0
    je .failed
    mov qword[re_array], rax
    mov rax, 0
    ; allocate im_array
    mov rdi, r15
    mov rsi, 8
    call calloc
    cmp rcx, 0
    je .failed
    mov qword[im_array], rax
    mov rax, 0
    
    dec r15 ;; for the derivative arrays
    ; allocate der_re_array
    mov rdi, r15
    mov rsi, 8
    call calloc
    cmp rcx, 0
    je .failed
    mov qword[der_re_array], rax
    mov rax, 0
    ; allocate der_im_array
    mov rdi, r15
    mov rsi, 8
    call calloc
    cmp rcx, 0
    je .failed
    mov qword[der_im_array], rax
    mov rax, 0
    
    
    mov r13, qword[order]
    jmp .read_coeff_args
    .failed: ;; in case we failed in calloc...
    mov rax, 0
    mov rdi, error_message_array
    call printf
    leave
    jmp finish
    
    .read_coeff_args:
    mov rax,0
    mov rdi, read_coeff
    mov rsi, coeff_place
    mov rdx, coeff_re
    mov rcx, coeff_im
    call scanf
    mov r15, qword[re_array]
    mov r14, qword[im_array]
    fld qword[coeff_re]
    fld qword[coeff_im]
    mov rax, 8
    mov rsi, qword[coeff_place]
    mul rsi
    add r14, rax
    fstp qword[r14]
    add r15, rax
    fstp qword[r15]
    dec r13
    cmp r13, 0
    jge .read_coeff_args
    mov rax, 0
    mov rdi, read_initial
    mov rsi, initial_re
    mov rdx, initial_im
    call scanf
    ;; initial values for z_n
    fld qword[initial_re]
    fstp qword[z_n_re]
    fld qword[initial_im]
    fstp qword[z_n_im]
    
    .derivative:
        mov r11, 2 ; counter
        mov r12, qword[re_array]
        mov r13,qword[im_array]
        mov r14,qword[der_re_array]
        mov r15, qword[der_im_array]
        add r12, 8
        add r13, 8
        fld qword[r12]
        fld qword[r13]
        fstp qword[r15]
        fstp qword[r14]

        mov r8, qword[order]
        ;; check if order = 1
        cmp r8, 2
        jle .finish_dervative
        
        .derivative_loop:
        add r12, 8
        add r13, 8
        add r14, 8
        add r15, 8
        fld qword[r12]
        fld qword[r13]
        fstp qword[first_im]
        fstp qword[first_re]
        fld qword[zero]
        fstp qword[sec_im]
        fld qword[float_counter]
        fstp qword[sec_re]
        call complex_mul
        fld qword[re]
        fld qword[im]
        fstp qword[r15]
        fstp qword[r14]
        ; store in derivative arrays
        ;; inc counter for derivative
        finit
        fld qword[float_counter]
        fld1
        fadd
        fst qword[float_counter]
        inc r11
        cmp r11, r8
        jne .derivative_loop
        
        .finish_dervative:
        ;; return order to it's original value
        mov r15, qword[order]
        dec r15
        mov qword[order], r15
        call close_to_epsilon
        leave
        jmp finish
    


        close_to_epsilon:
        enter 0,0
        ;; we send the arguments to horner method
        mov r13, qword[order]
        mov qword[horner_order], r13
        mov r13, qword[re_array]
        mov qword[horner_re_array_pointer], r13
        mov r13, qword[im_array]
        mov qword[horner_im_array_pointer], r13
        call horner_method
        ; f(z_n)
        fld qword[horner_re]
        fstp qword[re]
        fld qword[horner_im]
        fstp qword[im]
        
        ;; here we'll check the results from f(z_n), then, if it'll be little than epsilon we'll print the root(z_n)
        finit
        fld qword[re]
        fld st0
        fmulp ;; re^2
        fld qword[im]
        fld st0
        fmulp ;; im^2
        faddp ;; re^2 + im^2
        fld qword[epsilon]
        fld st0
        fmulp
        fcomi
        jb .call_calc;if epsilon^2 < (re^2 + im^2), contuinue calculation
        call print_complex_num
        call free_arrays
        leave
        ret
        .call_calc:
        call cont_calculate
        leave
        ret
        cont_calculate:
        enter 0,0
        mov r13, qword[order]
        mov qword[horner_order], r13
        mov r13, qword[re_array]
        mov qword[horner_re_array_pointer], r13
        mov r13, qword[im_array]
        mov qword[horner_im_array_pointer], r13
        call horner_method
        ; f(z_n)
        fld qword[horner_re]
        fstp qword[before_derivative_re]
        fld qword[horner_im]
        fstp qword[before_derivative_im]
        
        mov r13, qword[order]
        dec r13
        mov qword[horner_order], r13
        mov r13, qword[der_re_array]
        mov qword[horner_re_array_pointer], r13
        mov r13, qword[der_im_array]
        mov qword[horner_im_array_pointer], r13
        call horner_method
        ; f'(z_n)
        fld qword[horner_re]
        fstp qword[after_derivative_re]
        fld qword[horner_im]
        fstp qword[after_derivative_im]
        fld qword[horner_re]
        fld st0
        fmulp
        fld qword[horner_im]
        fld st0
        fmulp
        faddp
        fld qword[zero]
        fcomi
        je .division_in_zero
        
        ;; f(z_n)/f'(z_n)
         fld qword[before_derivative_re]
        fstp qword[first_re]
         fld qword[before_derivative_im]
        fstp qword[first_im]
         fld qword[after_derivative_re]
        fstp qword[sec_re]
         fld qword[after_derivative_im]
        fstp qword[sec_im]
        call complex_div
        
        ; result from division (stored re and im)
         fld qword[z_n_re]
        fstp qword[first_re]
         fld qword[z_n_im]
        fstp qword[first_im]
        fld qword[re]
        fstp qword[sec_re]
         fld qword[im]
        fstp qword[sec_im]
        ;; (z_n)-f(z_n)/f'(z_n)
        call complex_sub
        ;; stores the new z_n
          fld qword[re]
        fstp qword[z_n_re]
         fld qword[im]
        fstp qword[z_n_im]
        call close_to_epsilon
        leave
        ret
        
        .division_in_zero:
        mov rax, 0
        mov rdi, error_message_division
        call printf
        call free_arrays
        leave
        ret
    horner_method:
        enter 0,0
        mov rsi, qword[horner_order]
        mov r15, rsi
        mov rax, 8
        mul rsi
        ;; in rax we'll have the offset
        
        mov r13, qword[horner_re_array_pointer]
        add r13, rax
        mov r14, qword[horner_im_array_pointer]
        add r14, rax
        fld qword[r13]
        fstp qword[horner_re]
        fld qword[r14]
        fstp qword[horner_im]
        dec r15
        cmp r15, -1
        jne .inside_loop
        leave
        ret
      .inside_loop:
        sub r13, 8
        sub r14, 8
      ; Set a,b,c
        fld qword[horner_re]
        fstp qword[first_re] ; a_re
        fld qword[horner_im] 
        fstp qword[first_im] ; a;_im
        fld qword[z_n_re]
        fstp qword[sec_re] ; b_re
        fld qword[z_n_im]
        fstp qword[sec_im] ; b_im
        fld qword[r13] 
        fstp qword[third_re] ; c_re
        fld qword[r14]
        fstp qword[third_im] ; c_im
        ; calculate
        call compute_a_mult_b_plus_c
        
        ; next a
        fld qword[re]
        fstp qword[horner_re]
        fld qword[im]
        fstp qword[horner_im]
        dec r15
        cmp r15, 0
        jge .inside_loop

        leave
        ret
    ;; when this method finish, we'll see the values in horner_re and horner_im
    
    print_complex_num:
        enter 0,0
        movsd xmm0, [z_n_re]
        movsd xmm1, [z_n_im]
        mov rax, 2
        mov rdi, str
        call printf
        mov rax, 0
        leave
        ret
    
    complex_add:
    enter 0,0
    finit
    fld qword[first_re]
    fld qword[sec_re]
    fadd
    fst qword [re]
    finit
    fld qword[first_im]
    fld qword[sec_im]
    fadd
    fst qword[im]
    leave
    ret
    

    complex_sub:
    enter 0,0
    finit
    fld qword[first_re]
    fld qword[sec_re]
    fsub
    fst qword [re]
    finit
    fld qword[first_im]
    fld qword[sec_im]
    fsub
    fst qword[im]
    leave
    ret
    
    
    
    complex_mul:
    enter 0,0
    finit
    fld qword[first_re]
    fld qword[sec_re]
    fmul
    fld qword[first_im]
    fld qword[sec_im]
    fmul
    fsub
    fst qword[re]
    finit
    fld qword[first_re]
    fld qword[sec_im]
    fmul
    fld qword[sec_re]
    fld qword[first_im]
    fmul
    fadd
    fst qword[im]
    leave
    ret
    
    
    
    complex_div:
   enter 0,0
    ;; using the formula: (a+bi)/(c+di)= ((ac+bd)/c^2+d^2) + ((bc-ad)/c^2+d^2)i
    finit
    fld qword[sec_re] ; c
    fld st0 ; c
    fmul ; c^2
    fld qword[sec_im] ; d
    fld st0 ; d
    fmul ; d^2
    fadd ; c^2 + d^2
    fld st0 ; copy the last result for later use
    fld qword[first_re] ; a
    fld qword[sec_re] ; c
    fmul ; ac
    fld qword[first_im] ; b
    fld qword[sec_im] ; d
    fmul ; bd
    fadd ; ac+bd
    fxch
    fdiv ; ac+bd / c^2 + d^2
    fst qword[re] ; the real value of the new number
    fld qword[first_im] ; b
    fld qword[sec_re] ; c
    fmulp ; bc
    fld qword[first_re] ; a
    fld qword[sec_im] ; d
    fmul ; ad
    fsub ; bc-ad
    fdiv st2 ; bc-ad / c^2 + d^2
    fst qword[im] ; the imagniry value of the new number
    leave
    ret
    
    compute_a_mult_b_plus_c:
    enter 0,0
    ; assume we have the 3 numbers, and we'll put the result in re and im
    ; a in first, b in sec and c in third
    call complex_mul
    ;; now we have a*b in re and im
    finit
    fld qword[re]
    fst qword[first_re]
    finit
    fld qword[im]
    fst qword[first_im]
    finit
    fld qword[third_re]
    fst qword[sec_re]
    finit
    fld qword[third_im]
    fst qword[sec_im]
    call complex_add
    mov rax, 0
    leave
    ret
    ;; now we have the result in re and im
    
    free_arrays:
        enter 0,0
        mov rdi, qword[re_array]
        call free
        mov rdi, qword[im_array]
        call free
        mov rdi, qword[der_re_array]
        call free
        mov rdi, qword[der_im_array]
        call free
        
        leave
        ret
        
    finish:
        ret