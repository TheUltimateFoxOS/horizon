[bits 64]

[global fxsave_if_supported]
fxsave_if_supported:
	mov rax, 1
	cpuid
	and edx, 1 <<24 ; check for FXSR
	test edx, edx
	je .exit

	fxsave [rdi]

.exit:
	ret


[global fxrstor_if_supported]
fxrstor_if_supported:
	mov rax, 1
	cpuid
	and edx, 1 << 24 ; check for FXSR
	test edx, edx
	je .exit

	fxrstor [rdi]

.exit:
	ret
