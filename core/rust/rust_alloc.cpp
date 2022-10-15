#include <stdint.h>
#include <stddef.h>

#include <utils/abort.h>
#include <utils/log.h>
#include <memory/heap.h>
#include <utils/string.h>

extern "C" {
	__attribute__((noreturn))
	void __rust_alloc_error_handler(size_t size, size_t align) {
		abortf("RUST: alloc error (size: %d, align: %d)", size, align);
	}

	uint8_t* __rust_alloc(size_t size, size_t align) {
		return (uint8_t*) memory::malloc(size);
	}

	void __rust_dealloc(uint8_t* ptr, size_t size, size_t align) {
		memory::free(ptr);
	}

	uint8_t* __rust_alloc_zeroed(size_t size, size_t align) {
		void* ptr = memory::malloc(size);
		memset(ptr, 0, size);

		return (uint8_t*) ptr;
	}

	uint8_t* __rust_realloc(uint8_t* ptr, size_t old_size, size_t align, size_t new_size) {
		return (uint8_t*) memory::realloc(ptr, old_size, new_size);
	}
}