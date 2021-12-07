#pragma once

#include <stdint.h>

namespace memory {
	class page_map_indexer {
		public:
			page_map_indexer(uint64_t virtual_address);
			
			uint64_t PDP_i;
			uint64_t PD_i;
			uint64_t PT_i;
			uint64_t P_i;
	};
}