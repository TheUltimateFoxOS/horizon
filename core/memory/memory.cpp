#include <memory/memory.h>

#include <memory/page_frame_allocator.h>
#include <memory/page_table_manager.h>
#include <memory/pagetable.h>
#include <memory/heap.h>

#include <utils/log.h>
#include <utils/string.h>

#include <boot/boot.h>

using namespace memory;

uint64_t memory::get_memory_size() {
	static uint64_t memorySizeBytes = 0;
	
	if (memorySizeBytes > 0) {
		return memorySizeBytes; // cache the value
	}

	for (int i = 0; i < boot::boot_info.memmap_entries; i++){
		memorySizeBytes += boot::boot_info.memmap[i].length;
	}

	return memorySizeBytes;
}

extern uint64_t kernel_start;
extern uint64_t kernel_end;

#define KERNEL_PHYSICAL_ADDRESS(virtual_address) (void*) ((uint64_t) boot::boot_info.physical_base_address + ((uint64_t) virtual_address - (uint64_t) &virtual_base))

void memory::prepare_memory() {	
	uint64_t m_map_entries = boot::boot_info.memmap_entries;

	debugf("Creating page frame allocator...\n");
	global_allocator = page_frame_allocator();
	global_allocator.read_EFI_memory_map();

	uint64_t kernel_size = (uint64_t)&kernel_end - (uint64_t)&kernel_start;
	uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

	debugf("Kernel size: %d bytes (%d pages)\n", kernel_size, kernel_pages);

	debugf("Creating page table...\n");
	page_table_t* pml4 = (page_table_t*) global_allocator.request_page();
	memset(pml4, 0, 0x1000);

	debugf("Creating page table manager...\n");
	global_page_table_manager = page_table_manager(pml4);

	debugf("Starting to map memory...\n");
	for(uint64_t size = 0; size < kernel_size; size += 4096) {
		uint64_t physical_address = (uint64_t) boot::boot_info.physical_base_address + size;
		uint64_t virtual_address = (uint64_t) boot::boot_info.virtual_base_address + size;
		global_page_table_manager.map_memory((void*) virtual_address, (void*) physical_address);
	}

	for (int i = 0; i < boot::boot_info.memmap_entries; i++) {
		int type = boot::boot_info.memmap[i].type;
		uint64_t base = boot::boot_info.memmap[i].base;
		base &= ~0xFFF;

		uint64_t top = base + boot::boot_info.memmap[i].length;
		top = (top + 0xFFF) & ~0xFFF;

		debugf("Mapping memory type %d from %p with size %p\n", type, base, top - base);

		for (uint64_t t = base; t < top; t += 0x1000) {
			global_page_table_manager.map_memory((void*) t, (void*) t);
			global_page_table_manager.map_memory((void*) (t + (uint64_t) boot::boot_info.hhdm_base_address), (void*) t);
		}
	}

	for (uint64_t i = 0; i < get_memory_size(); i += 0x1000) {
		global_page_table_manager.map_memory((void*) i, (void*) i);
	}


	// debugf("Mapping and locking framebuffer...\n");
	// int64_t fbBase = (uint64_t) boot::boot_info.framebuffer.base_address;
	// int64_t fbSize = (boot::boot_info.framebuffer.width * boot::boot_info.framebuffer.height * 4) + 0x1000;
	// for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
	// 	global_page_table_manager.map_memory((void*)t, (void*)t);
	// 	global_page_table_manager.map_memory((void*)t + (uint64_t) boot::boot_info.hhdm_base_address, (void*)t);
	// 	global_allocator.lock_page((void*)t);
	// }

	void* smp_trampoline_target = (void*) 0x8000;
	debugf("Locking smp trampoline target...\n");
	global_allocator.lock_page(smp_trampoline_target);

	debugf("Loading page table...\n");
	__asm__ __volatile__ ("mov %0, %%cr3" : : "r" (pml4));

	debugf("Loading heap...\n");
	memory::initialize_heap((void*) 0x0000100000000000, 0x10);
}