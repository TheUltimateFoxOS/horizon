[global stivale2_bootstrap]
[global stivale2_data]
[extern start_apic_timer]
[extern load_gdt]

;# stivale2_bootstrap-signature: void stivale2_bootstrap(stivale2_struct* bootinfo);
;# stivale2_bootstrap-doc: The main entry point for aplication processors booted using the stivale2 boot protocol smp tag.

stivale2_bootstrap:
	cli

	xor rax, rax
	mov eax, [stivale2_data.pagetable]
	mov cr3, rax

	xor rdi, rdi
	mov rdi, [stivale2_data.gdt]
	call load_gdt

	xor rax, rax
	mov rax, [stivale2_data.idt]
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

	mov [stivale2_data.status], byte 1

	call [stivale2_data.entry]

	jmp $

;# stivale2_data-discard

stivale2_data:
	.status: db 0
	.pagetable: dq 0
	.idt: dq 0
	.gdt: dq 0
	.stack_ptr: dq 0
	.entry: dq 0

times 4096 - ($ - $$) db 0