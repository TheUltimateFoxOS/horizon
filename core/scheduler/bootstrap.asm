[global bootstrap]
[global bootstrap_data]
[extern load_gdt]

bootstrap:
	cli

	xor rax, rax
	mov eax, [bootstrap_data.pagetable]
	mov cr3, rax

	xor rdi, rdi
	mov rdi, [bootstrap_data.gdt]
	call load_gdt

	xor rax, rax
	mov rax, [bootstrap_data.idt]
	lidt [rax]

	sti

	; enable coprocessor (fpu and sse)
	mov rax, cr0
	and ax, 0xFFFB
	or ax, 0x2
	mov cr0, rax

	mov rax, cr4
	or ax, 3 << 9
	mov cr4, rax

	fninit

	mov rbp, rsp ; mark end of stack for stack trace

	mov [bootstrap_data.status], byte 1

	call [bootstrap_data.entry]

	jmp $

bootstrap_data:
	.status: db 0
	.pagetable: dq 0
	.idt: dq 0
	.gdt: dq 0
	.entry: dq 0

times 4096 - ($ - $$) db 0