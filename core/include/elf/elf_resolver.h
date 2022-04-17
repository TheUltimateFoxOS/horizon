#pragma once

#include <elf/elf.h>

namespace elf {
	class elf_symbol_resolver {
		public:
			elf_symbol_resolver(void* raw_elf_file);
			~elf_symbol_resolver();

			void* resolve(char* symbol_name);
			char* resolve(void* symbol_addr);
		private:
			void* raw_elf_file;
			symbol_table_information sym_info;
	};

	extern elf_symbol_resolver* kernel_resolver;
	extern elf_symbol_resolver* other_resolver[512];
	extern int other_resolver_count;

	uint64_t resolve_symbol(char* symbol_name);
	char* resolve_symbol(uint64_t symbol_addr);

	void setup();
}