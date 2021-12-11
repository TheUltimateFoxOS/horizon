[bits 64]
%macro CF_RESULT 0
	mov rcx, 1
	mov rax, 0
	cmovnc rax, rcx
%endmacro

[global atomic_lock]
atomic_lock:
	lock bts qword [rdi], rsi
	CF_RESULT
	ret

[global atomic_unlock]
atomic_unlock:
	lock btr qword [rdi], rsi
	CF_RESULT
	ret

[global atomic_spinlock]
atomic_spinlock:
.aquire:
	lock bts qword [rdi], rsi
	jnc .exit
.spin:
	pause
	bt qword [rdi], rsi
	jc .spin
	jmp .aquire
.exit:
	ret