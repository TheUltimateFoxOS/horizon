#include <memory/bitmap.h>

using namespace memory;

bool bitmap::operator[](uint64_t index) {
	return get(index);
}

bool bitmap::get(uint64_t index) {
	if (index > size * 8) {
		return false;
	}

	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0b10000000 >> bitIndex;

	if ((buffer[byteIndex] & bitIndexer) > 0){
		return true;
	}
	
	return false;
}

bool bitmap::set(uint64_t index, bool value) {
	if (index > size * 8) {
		return false;
	}

	uint64_t byteIndex = index / 8;
	uint8_t bitIndex = index % 8;
	uint8_t bitIndexer = 0b10000000 >> bitIndex;

	buffer[byteIndex] &= ~bitIndexer;
	if (value) {
		buffer[byteIndex] |= bitIndexer;
	}

	return true;
}