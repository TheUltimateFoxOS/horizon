#include <memory/page_frame_allocator.h>

#include <memory/memory.h>

#include <boot/boot.h>

#include <utils/log.h>

using namespace memory;

uint64_t free_memory;
uint64_t reserved_memory;
uint64_t used_memory;
bool initialized = false;

uint64_t page_bitmap_index;

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

	for (int i = 0; i < boot::boot_info.memmap_entries; i++) {
		if (boot::boot_info.memmap[i].type == MMAP_USABLE) {
			if (boot::boot_info.memmap[i].length > largest_free_mem_seg_size) {
				largest_free_mem_seg = (void*) boot::boot_info.memmap[i].base;
				largest_free_mem_seg_size = boot::boot_info.memmap[i].length;
			}
		}
	}

	uint64_t memorysize = get_memory_size();
	free_memory = memorysize;
	used_memory = 0;
	reserved_memory = 0;

	page_bitmap_index = 0;

	uint64_t bitmapsize = memorysize / 4096 / 8 + 1;
	init_bitmap(bitmapsize, largest_free_mem_seg);

	reserve_pages(0, memorysize / 4096 + 1);

	for (int i = 0; i < boot::boot_info.memmap_entries; i++) {
		if (boot::boot_info.memmap[i].type == MMAP_USABLE) {
			unreserve_pages((void*) boot::boot_info.memmap[i].base, boot::boot_info.memmap[i].length / 4096);
		}
	}

	reserve_pages(0, 0x100); // reserve between 0 and Ox10000000
	lock_pages(this->page_bitmap.buffer, this->page_bitmap.size / 4096 + 1);
}

void page_frame_allocator::init_bitmap(size_t bitmapsize, void* buffer_address){
	this->page_bitmap.size = bitmapsize;
	this->page_bitmap.buffer = (uint8_t*) buffer_address;

	memset(this->page_bitmap.buffer, 0, bitmapsize);
}

void* page_frame_allocator::request_page_internal(uint64_t start, uint64_t end) {
	for (uint64_t x = start; x < end; x++) {
		if (x == 0) {
			continue;
		}
        if (this->page_bitmap[x] == true) {
			continue;
		}

        lock_page((void*) (x * 4096));
		page_bitmap_index = x + 1;
        return (void*) (x * 4096);
    }

	return NULL;
}

void* page_frame_allocator::request_page() {
	void* address = this->request_page_internal(page_bitmap_index, this->page_bitmap.size * 8);
	if (address) {
		return address;
	}

	address = this->request_page_internal(0, page_bitmap_index); //Make sure we don't miss a page
	if (address) {
		return address;
	}

	debugf("Unable to allocate page. Free: %d; Reserved: %d; Used: %d.\n", free_memory, reserved_memory, used_memory);
    return NULL; //Maybe create a swap file
}

void* page_frame_allocator::request_pages_internal(uint64_t start, uint64_t end, int amount) {
	for (uint64_t x = start; x < end; x++) {
        if (this->page_bitmap[x] == true) {
			continue;
		}

		int i = 1;
		for (; i < amount; i++) {
			if (this->page_bitmap[x + i] == true) {
				goto next;
			}
		}

        lock_pages((void*) (x * 4096), amount);
		page_bitmap_index = x + amount;
        return (void*) (x * 4096);

	next:
		x += i;
		continue;
    }

	return NULL;
}

void* page_frame_allocator::request_pages(int amount) {
	if (amount == 1) {
		return this->request_page();
	} else if (amount == 0) {
		return NULL;
	}

	void* address = this->request_pages_internal(page_bitmap_index, this->page_bitmap.size * 8, amount);
	if (address) {
		return address;
	}

	address = this->request_pages_internal(0, page_bitmap_index, amount); //Make sure we don't miss a page
	if (address) {
		return address;
	}

	debugf("Unable to alocate %d pages. Free: %d; Reserved: %d; Used: %d.\n", amount, free_memory, reserved_memory, used_memory);
    return NULL; //Maybe create a swap file
}

void page_frame_allocator::free_page(void* address) {
	uint64_t index = (uint64_t) address / 4096;
	if (this->page_bitmap[index] == false) {
		return;
	}

	if (this->page_bitmap.set(index, false)) {
		free_memory += 4096;
		used_memory -= 4096;

		if (page_bitmap_index > index) {
			page_bitmap_index = index;
		}
	}
}

void page_frame_allocator::free_pages(void* address, uint64_t page_count) {
	for (uint64_t t = 0; t < page_count; t++) {
		free_page((void*) ((uint64_t) address + (t * 4096)));
	}
}

void page_frame_allocator::lock_page(void* address) {
	uint64_t index = (uint64_t) address / 4096;
	if (this->page_bitmap[index] == true) {
		return;
	}

	if (this->page_bitmap.set(index, true)) {
		free_memory -= 4096;
		used_memory += 4096;
	}
}

void page_frame_allocator::lock_pages(void* address, uint64_t page_count) {
	for (int t = 0; t < page_count; t++) {
		lock_page((void*) ((uint64_t) address + (t * 4096)));
	}
}

void page_frame_allocator::unreserve_page(void* address) {
	uint64_t index = (uint64_t) address / 4096;
	if (this->page_bitmap[index] == false) {
		return;
	}

	if (this->page_bitmap.set(index, false)) {
		free_memory += 4096;
		reserved_memory -= 4096;

		if (page_bitmap_index > index) {
			page_bitmap_index = index;
		}
	}
}

void page_frame_allocator::unreserve_pages(void* address, uint64_t page_count) {
	for (int t = 0; t < page_count; t++) {
		unreserve_page((void*) ((uint64_t) address + (t * 4096)));
	}
}

void page_frame_allocator::reserve_page(void* address) {
	uint64_t index = (uint64_t) address / 4096;
	if (this->page_bitmap[index] == true) {
		return;
	}

	if (this->page_bitmap.set(index, true)) {
		free_memory -= 4096;
		reserved_memory += 4096;
	}
}

void page_frame_allocator::reserve_pages(void* address, uint64_t pageCount) {
	for (uint64_t t = 0; t < pageCount; t++) {
		reserve_page((void*) ((uint64_t) address + (t * 4096)));
	}
}

uint64_t page_frame_allocator::get_free_RAM() {
	return free_memory;
}

uint64_t page_frame_allocator::get_used_RAM() {
	return used_memory;
}

uint64_t page_frame_allocator::get_reserved_RAM() {
	return reserved_memory;
}