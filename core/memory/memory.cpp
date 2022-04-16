#include <memory/memory.h>

#include <memory/page_frame_allocator.h>
#include <memory/page_table_manager.h>
#include <memory/pagetable.h>
#include <memory/heap.h>

#include <utils/log.h>
#include <utils/string.h>

using namespace memory;

uint64_t memory::get_memory_size(stivale2_struct* bootinfo) {
	static uint64_t memorySizeBytes = 0;
	
	if (memorySizeBytes > 0) {
		return memorySizeBytes; // cache the value
	}

	stivale2_struct_tag_memmap* memmap = stivale2_tag_find<stivale2_struct_tag_memmap>(bootinfo, STIVALE2_STRUCT_TAG_MEMMAP_ID);

	for (int i = 0; i < memmap->entries; i++){
		memorySizeBytes += memmap->memmap[i].length;
	}

	return memorySizeBytes;
}

extern uint64_t kernel_start;
extern uint64_t kernel_end;
extern uint64_t virtual_base;

#define KERNEL_PHYSICAL_ADDRESS(virtual_address) ((void*)((unsigned long long)(virtual_address) - ((unsigned long long) &virtual_base)))

void memory::prepare_memory(stivale2_struct* bootinfo) {
	stivale2_struct_tag_memmap* memmap = stivale2_tag_find<stivale2_struct_tag_memmap>(bootinfo, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	
	uint64_t m_map_entries = memmap->entries;

	debugf("Creating page frame allocator...\n");
	global_allocator = page_frame_allocator();
	global_allocator.read_EFI_memory_map(bootinfo);

	uint64_t kernel_size = (uint64_t)&kernel_end - (uint64_t)&kernel_start;
	uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

	debugf("Kernel size: %d bytes (%d pages)\n", kernel_size, kernel_pages);

	debugf("Creating page table...\n");
	page_table_t* pml4 = (page_table_t*) global_allocator.request_page();
	memset(pml4, 0, 0x1000);

	debugf("Creating page table manager...\n");
	global_page_table_manager = page_table_manager(pml4);

	void* kernel_physical_start = KERNEL_PHYSICAL_ADDRESS(&kernel_start);
	void* kernel_physical_end = KERNEL_PHYSICAL_ADDRESS(&kernel_end);
	uint64_t kernel_virtual_base = (uint64_t) &virtual_base;

	debugf("Kernel physical start: %p\n", kernel_physical_start);
	debugf("Kernel physical end: %p\n", kernel_physical_end);
	debugf("Kernel virtual base: %p\n", kernel_virtual_base);

	debugf("Starting to map memory...\n");
	for(void* ptr = kernel_physical_start; (uint64_t)ptr < (uint64_t)kernel_physical_end; ptr = ptr + 0x1000) {
		void* virtual_address = (void*)((uint64_t)ptr + kernel_virtual_base);
		global_page_table_manager.map_memory(virtual_address, ptr);
	}
	for (int i = 0; i < get_memory_size(bootinfo); i += 0x1000) {
		global_page_table_manager.map_memory((void*) i, (void*) i);
	}


	debugf("Mapping and locking framebuffer...\n");
	stivale2_struct_tag_framebuffer* framebuffer = stivale2_tag_find<stivale2_struct_tag_framebuffer>(bootinfo, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	int64_t fbBase = (uint64_t)framebuffer->framebuffer_addr;
	int64_t fbSize = (framebuffer->framebuffer_width * framebuffer->framebuffer_height * framebuffer->framebuffer_bpp) + 0x1000;
	for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
		global_page_table_manager.map_memory((void*)t, (void*)t);
		global_allocator.lock_page((void*)t);
	}

	void* smp_trampoline_target = (void*) 0x8000;
	debugf("Locking smp trampoline target...\n");
	global_allocator.lock_page(smp_trampoline_target);

	debugf("Loading page table...\n");
	__asm__ __volatile__ ("mov %0, %%cr3" : : "r" (pml4));

	debugf("Loading heap...\n");
	memory::initialize_heap((void*) 0x0000100000000000, 0x10);
}