#include <elf/kernel_module.h>

#include <utils/log.h>

void init() {
	printf("Hello, world!\n");
}

void device_init() {
	printf("Hello, device!\n");
}

void fs_init() {
	printf("Hello, fs!\n");
}

define_module("test", init, device_init, fs_init);