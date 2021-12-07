#pragma once

#include <stdint.h>

namespace memory {
	enum page_table_flags {
		present = 0,
		read_write = 1,
		user_super = 2,
		write_through = 3,
		cache_disabled = 4,
		accessed = 5,
		larger_pages = 7,
		custom0 = 9,
		custom1 = 10,
		custom2 = 11,
		NX = 63 // only if supported
	};

	struct page_directory_entry_t {
		uint64_t value;
		void set_flag(page_table_flags flag, bool enabled);
		bool get_flag(page_table_flags flag);
		void set_address(uint64_t address);
		uint64_t get_address();
	};

	struct page_table_t { 
		page_directory_entry_t entries [512];
	}__attribute__((aligned(0x1000)));
}