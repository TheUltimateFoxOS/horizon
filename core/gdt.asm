[bits 64]

;#load_gdt-signature: void load_gdt(gdt_descriptor_t* gdt_descriptor);
[global load_gdt]
load_gdt:
	lgdt [rdi]
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	pop rdi
	mov rax, 0x08
	push rax
	push rdi

	retfq