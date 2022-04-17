[global _start_limine]
[extern limine_entry]

_start_limine: 
	mov rbp, rsp
	call cpu_init
	call limine_entry

cpu_init:
	cli
	push rbp
	mov rbp, rsp
	push rbx

	; enable coprocessor (fpu and sse)
	mov rax, cr0
	and ax, 0xFFFB
	or ax, 0x2
	mov cr0, rax

	mov rax, cr4
	or ax, 3 << 9
	mov cr4, rax

	fninit

	pop rbx
	pop rbp
	ret