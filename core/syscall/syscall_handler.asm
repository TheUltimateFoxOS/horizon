[bits 64]

%macro pusha 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popa 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

[global syscall_interrupt_stub]
syscall_interrupt_stub:
	push 0
	push 0x30 ; interrupt number

	cli
	pusha
	mov r15, cr0
	push r15
	mov r15, cr2
	push r15
	mov r15, cr3
	push r15
	mov r15, cr4
	push r15

	mov rdi, rsp

	cmp rax, [syscall_table_size]
	ja .skip

	lea r15, [syscall_table + rax * 8]
	call [r15]

.skip:

	pop rax
	pop rax
	pop rax
	pop rax
	popa

	add rsp, 16

	sti

	iretq

max_syscall equ 255

[global syscall_table]
[global syscall_table_size]

syscall_table:
	times  max_syscall * 8 db 0
syscall_table_end:

syscall_table_size: dq 0