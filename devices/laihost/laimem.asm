[bits 64]

[global memcpy]
[global memcmp]
[global memset]

[extern _Z6memcpyPvPKvi]
[extern _Z6memcmpPKvS0_i]
[extern _Z6memsetPvhm]

memcpy:
	jmp _Z6memcpyPvPKvi

memcmp:
	jmp _Z6memcmpPKvS0_i

memset:
	jmp _Z6memsetPvhm