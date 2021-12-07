#pragma once

#include <stdint.h>
#include <stddef.h>

namespace memory {
	struct heap_segment_header_t {
		size_t length;
		heap_segment_header_t* next;
		heap_segment_header_t* last;
		bool free;
		void combine_forward();
		void combine_backward();
		heap_segment_header_t* split(size_t split_length);
	};

	void initialize_heap(void* heap_address, size_t page_count);

	void* malloc(size_t size);
	void* realloc(void* ptr, size_t oldSize, size_t size);
	void free(void* address);

	void expand_heap(size_t length);
}

// inline void* operator new(size_t size) {return memory::malloc(size);}
// inline void* operator new[](size_t size) {return memory::malloc(size);}

// inline void operator delete(void* p, unsigned long) {memory::free(p);}
// inline void operator delete[](void* p) {memory::free(p);}