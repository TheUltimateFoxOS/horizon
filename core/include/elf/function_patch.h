#pragma once

#include <stdint.h>
#include <utils/abort.h>
#include <utils/log.h>
#include <elf/elf_resolver.h>

namespace elf {
	struct patch_t {
		void* old_addr;
		uint8_t old_code[13];
	};

	patch_t* patch(char* name, uint64_t new_func);
	void unpatch(patch_t* patch);

	#define disable_function(name) \
	{ \
		debugf("Disabling function %s\n", name); \
		elf::patch((char*) name, (uint64_t) (void*) (void (*)()) []() {\
			abortf("Function %s was disabled in file %s:%d", name, __FILE__, __LINE__); \
		}); \
	}
	
	#define disable_function_addr(addr) \
	{ \
		debugf("Disabling function at 0x%x\n", addr); \
		elf::patch(elf::resolve_symbol((uint64_t) addr), (uint64_t) (void*) (void (*)()) []() {\
			abortf("Function %p was disabled in file %s:%d", addr, __FILE__, __LINE__); \
		}); \
	}

}