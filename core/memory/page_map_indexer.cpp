#include <memory/page_map_indexer.h>

using namespace memory;

page_map_indexer::page_map_indexer(uint64_t virtual_address) {
	virtual_address >>= 12;
	P_i = virtual_address & 0x1ff;
	virtual_address >>= 9;
	PT_i = virtual_address & 0x1ff;
	virtual_address >>= 9;
	PD_i = virtual_address & 0x1ff;
	virtual_address >>= 9;
	PDP_i = virtual_address & 0x1ff;
}