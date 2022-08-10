[bits 64]

;#load_idt-signature: void load_idt(void* idt_ptr)
[global load_idt]
load_idt:
	lidt [rdi]
	ret