#include <memory/memory.h>

#include <memory/page_frame_allocator.h>
#include <memory/page_table_manager.h>
#include <memory/pagetable.h>
#include <memory/heap.h>

#include <utils/log.h>

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

void memory::prepare_memory(stivale2_struct* bootinfo) {
	stivale2_struct_tag_memmap* memmap = stivale2_tag_find<stivale2_struct_tag_memmap>(bootinfo, STIVALE2_STRUCT_TAG_MEMMAP_ID);
	
	uint64_t m_map_entries = memmap->entries;

	debugf("Creating page frame allocator...\n");
	global_allocator = page_frame_allocator();
	global_allocator.read_EFI_memory_map(bootinfo);

	uint64_t kernel_size = (uint64_t)&kernel_end - (uint64_t)&kernel_start;
	uint64_t kernel_pages = (uint64_t)kernel_size / 4096 + 1;

	debugf("Kernel size: %d bytes (%d pages)\n", kernel_size, kernel_pages);
	debugf("Locking kernel pages...\n");
	global_allocator.lock_pages(&kernel_start, kernel_pages);

	debugf("Creating page table...\n");
	page_table_t* pml4 = (page_table_t*) global_allocator.request_page();
	memset(pml4, 0, 0x1000);

	debugf("Creating page table manager...\n");
	global_page_table_manager = page_table_manager(pml4);

	debugf("Identity mapping memory...\n");
	for (uint64_t t = 0; t < get_memory_size(bootinfo); t+= 0x1000){
		global_page_table_manager.map_memory((void*)t, (void*)t);
	}

	debugf("Mapping and locking framebuffer...\n");
	stivale2_struct_tag_framebuffer* framebuffer = stivale2_tag_find<stivale2_struct_tag_framebuffer>(bootinfo, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
	uint64_t fbBase = (uint64_t)framebuffer->framebuffer_addr;
	uint64_t fbSize = (framebuffer->framebuffer_width * framebuffer->framebuffer_height * framebuffer->framebuffer_bpp) + 0x1000;
	for (uint64_t t = fbBase; t < fbBase + fbSize; t += 4096){
		global_page_table_manager.map_memory((void*)t, (void*)t);
	}

	debugf("Loading page table...\n");
	__asm__ __volatile__ ("mov %0, %%cr3" : : "r" (pml4));

	debugf("Loading heap...\n");
	memory::initialize_heap((void*) 0x0000100000000000, 0x10);
}