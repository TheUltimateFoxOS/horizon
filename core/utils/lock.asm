[bits 64]
%macro CF_RESULT 0
	mov rcx, 1
	mov rax, 0
	cmovnc rax, rcx
%endmacro

;#atomic_lock-signature: extern "C" bool atomic_lock(uint64_t* mutex, uint64_t bit);
[global atomic_lock]
atomic_lock:
	lock bts qword [rdi], rsi
	CF_RESULT
	ret

;#atomic_unlock-signature: extern "C" bool atomic_unlock(uint64_t* mutex, uint64_t bit);
[global atomic_unlock]
atomic_unlock:
	lock btr qword [rdi], rsi
	CF_RESULT
	ret

;#atomic_spinlock-signature: extern "C" void atomic_spinlock(uint64_t* mutex, uint64_t bit);
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