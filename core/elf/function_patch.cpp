#include <elf/function_patch.h>

#include <memory/heap.h>
#include <utils/string.h>

using namespace elf;

patch_t* elf::patch(char* name, uint64_t new_func) {
	uint64_t old_func = resolve_symbol(name);
	patch_t* patch = (patch_t*) memory::malloc(sizeof(patch_t));

	if(old_func != (uint64_t) NULL) {
		memcpy(patch->old_code, (void*) old_func, 13);
		patch->old_addr = (void*) old_func;

		uint8_t* func_mem = (uint8_t*) old_func;
		// write mov r15, some_addr
		*func_mem = 0x49;
		func_mem++;
		*func_mem = 0xbf;
		func_mem++;

		// write some_addr
		uint64_t* func_mem2 = (uint64_t*) func_mem;
		*func_mem2 = new_func;

		// write jmp r15
		uint8_t* jmp = (uint8_t*) (old_func + 10);
		*jmp = 0x41;
		jmp++;
		*jmp = 0xff;
		jmp++;
		*jmp = 0xe7;
	}
	return patch;
}

void unpatch(patch_t* patch) {
	memcpy(patch->old_addr, patch->old_code, 13);
	memory::free(patch);
}