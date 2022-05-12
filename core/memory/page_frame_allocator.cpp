#include <memory/page_frame_allocator.h>

#include <memory/memory.h>

#include <boot/boot.h>

using namespace memory;

uint64_t free_memory;
uint64_t reserved_memory;
uint64_t used_memory;
bool initialized = false;

namespace memory {
	page_frame_allocator global_allocator;
}

void page_frame_allocator::read_EFI_memory_map() {
	if (initialized) {
		return;
	}

	initialized = true;

	void* largest_free_mem_seg = NULL;
	size_t largest_free_mem_seg_size = 0;

	for (int i = 0; i < boot::boot_info.memmap_entries; i++){
		if (boot::boot_info.memmap[i].type == MMAP_USABLE) {
			if (boot::boot_info.memmap[i].length > largest_free_mem_seg_size) {
				largest_free_mem_seg = (void*) boot::boot_info.memmap[i].base;
				largest_free_mem_seg_size = boot::boot_info.memmap[i].length;
			}
		}
	}
	uint64_t memorysize = get_memory_size();
	free_memory = memorysize;
	uint64_t bitmapsize = memorysize / 4096 / 8 + 1;

	init_bitmap(bitmapsize, largest_free_mem_seg);

	reserve_pages(0, memorysize / 4096 + 1);

	for (int i = 0; i < boot::boot_info.memmap_entries; i++){
		if (boot::boot_info.memmap[i].type == MMAP_USABLE) {
			unreserve_pages((void*) boot::boot_info.memmap[i].base, boot::boot_info.memmap[i].length / 4096 );
		}
	}

	reserve_pages(0, 0x100); // reserve between 0 and Ox10000000
	lock_pages(page_bitmap.buffer, page_bitmap.size / 4096 + 1);
}

void page_frame_allocator::init_bitmap(size_t bitmapsize, void* buffer_address){
	page_bitmap.size = bitmapsize;
	page_bitmap.buffer = (uint8_t*) buffer_address;

	for (int i = 0; i < bitmapsize; i++){
		*(uint8_t*)(page_bitmap.buffer + i) = 0;
	}
}

uint64_t page_bitmap_index = 0;
void* page_frame_allocator::request_page(){
	for (int i = 0; i < page_bitmap_index; i++) {
		if(page_bitmap[i] == true) {
			continue;
		}

		page_bitmap_index = i;
		break;
	}
	

	for (uint64_t x = page_bitmap_index; x < page_bitmap.size * 8; x++){
        if (page_bitmap[x] == true) {
			continue;
		}

        lock_page((void*)(x * 4096));
        return (void*)(x * 4096);
    }

    return NULL; // Page Frame Swap to file
}

void* page_frame_allocator::request_pages(int amount){
	for (int i = 0; i < page_bitmap_index; i++) {
		if(page_bitmap[i] == true) {
			continue;
		}

		page_bitmap_index = i;
		break;
	}

	for (uint64_t x = page_bitmap_index; x < page_bitmap.size * 8; x++){
        if (page_bitmap[x] == true) {
			continue;
		}

		for (int i = 0; i < amount; i++) {
			if(page_bitmap[x + i] == true) goto next;
		}
		
        lock_pages((void*)(x * 4096), amount);
        return (void*)(x * 4096);

	next:
		x += amount;
		continue;
    }

    return NULL; // Page Frame Swap to file
}


void page_frame_allocator::free_page(void* address){
	uint64_t index = (uint64_t)address / 4096;
	if (page_bitmap[index] == false) {
		return;
	}

	if (page_bitmap.set(index, false)) {
		free_memory += 4096;
		used_memory -= 4096;
		if (page_bitmap_index > index) {
			page_bitmap_index = index;
		}
	}
}

void page_frame_allocator::free_pages(void* address, uint64_t page_count){
	for (int t = 0; t < page_count; t++){
		free_page((void*)((uint64_t)address + (t * 4096)));
	}
}

void page_frame_allocator::lock_page(void* address){
	uint64_t index = (uint64_t)address / 4096;
	if (page_bitmap[index] == true) {
		return;
	}

	if (page_bitmap.set(index, true)) {
		free_memory -= 4096;
		used_memory += 4096;
	}
}

void page_frame_allocator::lock_pages(void* address, uint64_t page_count){
	for (int t = 0; t < page_count; t++){
		lock_page((void*)((uint64_t)address + (t * 4096)));
	}
}

void page_frame_allocator::unreserve_page(void* address){
	uint64_t index = (uint64_t)address / 4096;
	if (page_bitmap[index] == false) {
		return;
	}

	if (page_bitmap.set(index, false)) {
		free_memory += 4096;
		reserved_memory -= 4096;
		if (page_bitmap_index > index) {
			page_bitmap_index = index;
		}
	}
}

void page_frame_allocator::unreserve_pages(void* address, uint64_t page_count){
	for (int t = 0; t < page_count; t++){
		unreserve_page((void*)((uint64_t)address + (t * 4096)));
	}
}

void page_frame_allocator::reserve_page(void* address){
	uint64_t index = (uint64_t)address / 4096;
	if (page_bitmap[index] == true) {
		return;
	}

	if (page_bitmap.set(index, true)) {
		free_memory -= 4096;
		reserved_memory += 4096;
	}
}

void page_frame_allocator::reserve_pages(void* address, uint64_t pageCount){
	for (uint64_t t = 0; t < pageCount; t++){
		reserve_page((void*)((uint64_t)address + (t * 4096)));
	}
}

uint64_t page_frame_allocator::get_free_RAM(){
	return free_memory;
}

uint64_t page_frame_allocator::get_used_RAM(){
	return used_memory;
}

uint64_t page_frame_allocator::get_reserved_RAM(){
	return reserved_memory;
}