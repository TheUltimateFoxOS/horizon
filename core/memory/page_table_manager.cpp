#include <memory/page_table_manager.h>

#include <stdint.h>

#include <memory/page_table_manager.h>
#include <memory/page_map_indexer.h>
#include <memory/page_frame_allocator.h>

#include <utils/string.h>

using namespace memory;

namespace memory {
	page_table_manager global_page_table_manager = NULL;
}

page_table_manager::page_table_manager(page_table_t* PML4_address) {
	this->PML4 = PML4_address;
}

void page_table_manager::map_memory(void* virtual_memory, void* physical_memory) {
	page_map_indexer indexer = page_map_indexer((uint64_t) virtual_memory);
	page_directory_entry_t PDE;

	PDE = PML4->entries[indexer.PDP_i];
	page_table_t* PDP;

	if (!PDE.get_flag(page_table_flags::present)) {
		PDP = (page_table_t*) global_allocator.request_page();
		memset(PDP, 0, 0x1000);

		PDE.set_address((uint64_t) PDP >> 12);
		PDE.set_flag(page_table_flags::present, true);
		PDE.set_flag(page_table_flags::read_write, true);
		PML4->entries[indexer.PDP_i] = PDE;
	} else {
		PDP = (page_table_t*)((uint64_t)PDE.get_address() << 12);
	}

	PDE = PDP->entries[indexer.PD_i];
	page_table_t* PD;
	if (!PDE.get_flag(page_table_flags::present)) {
		PD = (page_table_t*) global_allocator.request_page();
		memset(PD, 0, 0x1000);

		PDE.set_address((uint64_t) PD >> 12);
		PDE.set_flag(page_table_flags::present, true);
		PDE.set_flag(page_table_flags::read_write, true);
		PDP->entries[indexer.PD_i] = PDE;
	} else {
		PD = (page_table_t*)((uint64_t)PDE.get_address() << 12);
	}

	PDE = PD->entries[indexer.PT_i];
	page_table_t* PT;
	if (!PDE.get_flag(page_table_flags::present)) {
		PT = (page_table_t*) global_allocator.request_page();
		memset(PT, 0, 0x1000);

		PDE.set_address((uint64_t)PT >> 12);
		PDE.set_flag(page_table_flags::present, true);
		PDE.set_flag(page_table_flags::read_write, true);
		PD->entries[indexer.PT_i] = PDE;
	} else {
		PT = (page_table_t*) ((uint64_t) PDE.get_address() << 12);
	}

	PDE = PT->entries[indexer.P_i];
	PDE.set_address((uint64_t) physical_memory >> 12);
	PDE.set_flag(page_table_flags::present, true);
	PDE.set_flag(page_table_flags::read_write, true);
	PT->entries[indexer.P_i] = PDE;
}

void page_table_manager::map_range(void* virtual_memory, void* physical_memory, size_t range) {
	for (int i = 0; i < range; i += 0x1000) {
		global_page_table_manager.map_memory((void*) ((uint64_t) virtual_memory + i), (void*) ((uint64_t) physical_memory + i));
	}
}

void* page_table_manager::virt_to_phys(void* virtual_address) {
	uint64_t aligned_virtual_address = ((uint64_t) virtual_address / 0x1000) * 0x1000;

	page_map_indexer indexer = page_map_indexer(aligned_virtual_address);
	page_directory_entry_t PDE;

	PDE = PML4->entries[indexer.PDP_i];
	if (!PDE.get_flag(page_table_flags::present)) {
		return NULL;
	}
	page_table_t* PDP = (page_table_t*) ((uint64_t) PDE.get_address() << 12);

	PDE = PDP->entries[indexer.PD_i];
	if (!PDE.get_flag(page_table_flags::present)) {
		return NULL;
	}
	page_table_t* PD = (page_table_t*) ((uint64_t) PDE.get_address() << 12);

	PDE = PD->entries[indexer.PT_i];
	if (!PDE.get_flag(page_table_flags::present)) {
		return NULL;
	}
	page_table_t* PT = (page_table_t*) ((uint64_t)PDE.get_address() << 12);

	PDE = PT->entries[indexer.P_i];
	if (!PDE.get_flag(page_table_flags::present)) {
		return NULL;
	}

	uint64_t physical_address = PDE.get_address() << 12;
	return (void*) (physical_address + (((uint64_t) virtual_address) % 0x1000));
}