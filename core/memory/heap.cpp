#include <utils/string.h>
#include <memory/heap.h>
#include <memory/page_frame_allocator.h>
#include <memory/page_table_manager.h>

using namespace memory;

void* heap_start;
void* heap_end;
heap_segment_header_t* last_hdr;

//#memory::initialize_heap-doc: Initialize the memory heap.
void memory::initialize_heap(void* heap_address, size_t page_count){
	void* pos = heap_address;

	for (size_t i = 0; i < page_count; i++){
		global_page_table_manager.map_memory(pos, global_allocator.request_page());
		pos = (void*)((size_t)pos + 0x1000);
	}

	size_t heap_length = page_count * 0x1000;

	heap_start = heap_address;
	heap_end = (void*)((size_t)heap_start + heap_length);
	heap_segment_header_t* start_seg = (heap_segment_header_t*)heap_address;
	start_seg->length = heap_length - sizeof(heap_segment_header_t);
	start_seg->next = NULL;
	start_seg->last = NULL;
	start_seg->free = true;
	last_hdr = start_seg;
}

//#memory::free-doc: Free some allocated memory at an address.
void memory::free(void* address){
	heap_segment_header_t* segment = (heap_segment_header_t*)address - 1;
	segment->free = true;
	segment->combine_forward();
	segment->combine_backward();
}

//#memory::malloc-doc: Allocate some memory.
void* memory::malloc(size_t size){
	if (size % 0x10 > 0){ // it is not a multiple of 0x10
		size -= (size % 0x10);
		size += 0x10;
	}

	if (size == 0) return NULL;

	heap_segment_header_t* current_seg = (heap_segment_header_t*) heap_start;
	while(true){
		if(current_seg->free){
			if (current_seg->length > size){
				current_seg->split(size);
				current_seg->free = false;
				return (void*)((uint64_t)current_seg + sizeof(heap_segment_header_t));
			}
			if (current_seg->length == size){
				current_seg->free = false;
				return (void*)((uint64_t)current_seg + sizeof(heap_segment_header_t));
			}
		}
		if (current_seg->next == NULL) break;
		current_seg = current_seg->next;
	}
	expand_heap(size);
	return malloc(size);
}

//#memory::realloc-doc: Re-allocate some memory with a different size.
void* memory::realloc(void* ptr, size_t oldSize, size_t size) {
	if (size == 0) {
		free(ptr);
		return NULL;
	} else if (!ptr) {
		return malloc(size);
	} else if (size <= oldSize) {
		return ptr;
	} else {
		void* newPtr = malloc(size);
		if (newPtr) {
			memcpy(newPtr, ptr, oldSize);
			free(ptr);
		}
		return newPtr;
	}
}

//#heap_segment_header_t::split-doc: Split a heap segment.
heap_segment_header_t* heap_segment_header_t::split(size_t split_length){
	if (split_length < 0x10) return NULL;
	int64_t split_seg_length = length - split_length - (sizeof(heap_segment_header_t));
	if (split_seg_length < 0x10) return NULL;

	heap_segment_header_t* new_split_hdr = (heap_segment_header_t*) ((size_t)this + split_length + sizeof(heap_segment_header_t));
	next->last = new_split_hdr; // Set the next segment's last segment to our new segment
	new_split_hdr->next = next; // Set the new segment's next segment to out original next segment
	next = new_split_hdr; // Set our new segment to the new segment
	new_split_hdr->last = this; // Set our new segment's last segment to the current segment
	new_split_hdr->length = split_seg_length; // Set the new header's length to the calculated value
	new_split_hdr->free = free; // make sure the new segment's free is the same as the original
	length = split_length; // set the length of the original segment to its new length

	if (last_hdr == this) last_hdr = new_split_hdr;
	return new_split_hdr;
}

//#memory::expand_heap-doc: Expand the heap size.
void memory::expand_heap(size_t length){
	if (length % 0x1000) {
		length -= length % 0x1000;
		length += 0x1000;
	}

	size_t page_count = length / 0x1000;
	heap_segment_header_t* new_segment = (heap_segment_header_t*)heap_end;

	for (size_t i = 0; i < page_count; i++){
		global_page_table_manager.map_memory(heap_end, global_allocator.request_page());
		heap_end = (void*)((size_t)heap_end + 0x1000);
	}

	new_segment->free = true;
	new_segment->last = last_hdr;
	last_hdr->next = new_segment;
	last_hdr = new_segment;
	new_segment->next = NULL;
	new_segment->length = length - sizeof(heap_segment_header_t);
	new_segment->combine_backward();
}

//#heap_segment_header_t::combine_forward-doc: Combine the next part of the heap with the current one. Used to expand the heap size.
void heap_segment_header_t::combine_forward(){
	if (next == NULL) return;
	if (!next->free) return;

	if (next == last_hdr) last_hdr = this;

	if (next->next != NULL){
		next->next->last = this;
	}

	length = length + next->length + sizeof(heap_segment_header_t);

	next = next->next;
}

//#heap_segment_header_t::combine_backward-doc: Combine the next part of the heap with the current one. Used to expand the heap size.
void heap_segment_header_t::combine_backward(){
	if (last != NULL && last->free) last->combine_forward();
}
