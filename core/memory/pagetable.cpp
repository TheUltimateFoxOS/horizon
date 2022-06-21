#include <memory/pagetable.h>

using namespace memory;

void page_directory_entry_t::set_flag(page_table_flags flag, bool enabled){
	uint64_t bit_selector = (uint64_t) 1 << flag;
	value &= ~bit_selector;

	if (enabled) {
		value |= bit_selector;
	}
}

bool page_directory_entry_t::get_flag(page_table_flags flag){
	uint64_t bit_selector = (uint64_t) 1 << flag;
	return value & bit_selector > 0 ? true : false;
}

uint64_t page_directory_entry_t::get_address(){
	return (value & 0x000ffffffffff000) >> 12;
}

void page_directory_entry_t::set_address(uint64_t address){
	address &= 0x000000ffffffffff;
	value &= 0xfff0000000000fff;
	value |= (address << 12);
}