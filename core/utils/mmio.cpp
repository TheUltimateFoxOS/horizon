#include <utils/mmio.h>

//#MMIO::read8-doc: Read a uint8 from an MMIO address.
uint8_t MMIO::read8(uint64_t p_address) {
	return *((volatile uint8_t*) p_address);
}

//#MMIO::read16-doc: Read a uint16 from an MMIO address.
uint16_t MMIO::read16(uint64_t p_address) {
	return *((volatile uint16_t*) p_address);
}

//#MMIO::read32-doc: Read a uint32 from an MMIO address.
uint32_t MMIO::read32(uint64_t p_address) {
	return *((volatile uint32_t*) p_address);
}

//#MMIO::read64-doc: Read a uint64 from an MMIO address.
uint64_t MMIO::read64(uint64_t p_address) {
	return *((volatile uint64_t*) p_address);    
}

//#MMIO::write8-doc: Write a uint8 to an MMIO address.
void MMIO::write8(uint64_t p_address,uint8_t p_value) {
	(*((volatile uint8_t*) p_address)) = p_value;
}

//#MMIO::write16-doc: Write a uint16 to an MMIO address.
void MMIO::write16(uint64_t p_address,uint16_t p_value) {
	(*((volatile uint16_t*) p_address)) = p_value;    
}

//#MMIO::write32-doc: Write a uint32 to an MMIO address.
void MMIO::write32(uint64_t p_address,uint32_t p_value) {
	(*((volatile uint32_t*) p_address)) = p_value;
}

//#MMIO::write64-doc: Write a uint64 to an MMIO address.
void MMIO::write64(uint64_t p_address,uint64_t p_value) {
	(*((volatile uint64_t*) p_address)) = p_value;    
}