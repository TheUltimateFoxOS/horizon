#pragma once

#include <memory/pagetable.h>
#include <stddef.h>

namespace memory {
	class page_table_manager {
		public:
		page_table_manager(page_table_t* PML4_address);

		page_table_t* PML4;

		void map_memory(void* virtual_memory, void* physical_memory);
		void map_range(void* virtual_memory, void* physical_memory, size_t range);
		void* virt_to_phys(void* virtual_address);
	};

	extern page_table_manager global_page_table_manager;
}
