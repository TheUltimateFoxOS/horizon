[global _start_stivale2]
[extern stivale2_entry]

; This is here to mark the end of the stack trace.
; The boot loader passes the bootinfo struct just dont touch it and call kernel_main

;#_start_stivale2-signature: void _start_stivale2(stivale2_struct* bootinfo)
_start_stivale2: 
	push qword 0
	push qword 0

	mov rbp, rsp
	call cpu_init
	call stivale2_entry

;#cpu_init-signature: void cpu_init()
cpu_init:
	cli
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
	ret