#pragma once

#include <output/output.h>

#include <config.h>

#include <elf/unwind.h>
#include <elf/elf_resolver.h>

namespace log {
	extern output::output_device* debug_device;
	extern output::output_device* stdout_device;
}

void debugf_intrnl(const char* fmt, ...);

#ifdef DEBUG
#define debugf(fmt, ...) debugf_intrnl("[%s:%d] ", __FILE__, __LINE__); debugf_intrnl(fmt, ##__VA_ARGS__)
#define debugf_raw(fmt, ...) debugf_intrnl(fmt, ##__VA_ARGS__)
#define trace() { \
		debugf("TRACE: \n"); \
		([]() { \
			uint64_t rbp; \
			__asm__ __volatile__ ("movq %%rbp, %0" : "=r" (rbp)); \
			elf::unwind(100, rbp, [](int frame_num, uint64_t rip) { \
				if(elf::resolve_symbol(elf::resolve_symbol(rip)) != 0) { \
					debugf(">   %s + %d\n", elf::resolve_symbol(rip), rip - elf::resolve_symbol(elf::resolve_symbol(rip))); \
				} else { \
					debugf(">   <unknown function at 0x%x>\n", rip); \
				} \
			}); \
		})();\
	}
#else
#define debugf(fmt, ...)
#define debugf_raw(fmt, ...)
#define trace()
#endif

void printf(const char *fmt, ...);