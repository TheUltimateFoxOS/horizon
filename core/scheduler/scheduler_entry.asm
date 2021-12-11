[bits 64]
[extern _ZN9scheduler9kill_selfEv]
[global task_entry]

; NOTE: the addres to the target task needs to be in rax
; we do this to prevent jumping to random code if the task returns

task_entry:
	sti
	mov rbp, rsp ; mark bottom of stack trace
	call rax

.exit:
	call _ZN9scheduler9kill_selfEv ; if the task ever returns just exit
	
	jmp $