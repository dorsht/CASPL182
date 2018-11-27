section	.text
global	_start
global	system_call
extern	main
_start:
	pop		rdi		; rdi = argc
	mov		rsi,rsp		; rsi = argv
	mov		rdx,rcx		; put the number of arguments into rdx
	shl		rdx,2		; compute the size of argv in bytes
	add		rdx,rsi		; add the size to the address of argv	
	add		rdx,8		; skip NULL at the end of argv. rdx = envp
	
	call	main		; int main(int argc, char *argv[], char *envp[])

	mov		rdi,rax	;; error number
	mov		rax,60 ;; sys_exit 
	syscall
	nop
		
system_call:
	; Save caller state
	push	rbp				
	mov	rbp,rsp
	push	rdi
	push	rsi
	push	rdx
	push 	rcx		; syscall may modify rcx
	push	r11		; syscall may modify r11
	
	; Invoke system API
	mov		rax,rdi	; syscall index
	mov		rdi,rsi	; first argument	
	mov		rsi,rdx	; second argument	
	mov		rdx,rcx	; third argument
	syscall			; Transfer control to operating system. Result stored rax
	
	; Restore caller state
	pop		r11
	pop		rcx
	pop		rdx
	pop		rsi
	pop		rdi
	pop		rbp
	ret				; Back to caller
