#include <interrupts/idt.h>

using namespace interrupts;

//#idt_desc_entry_t::set_offset-doc: Sets the offset of a entry in the interrupt descriptor table.
void idt_desc_entry_t::set_offset(uint64_t offset) {
	offset_0 = (uint16_t) (offset & 0x000000000000ffff);
	offset_1 = (uint16_t) ((offset & 0x00000000ffff0000) >> 16);
	offset_2 = (uint32_t) ((offset & 0xffffffff00000000) >> 32);
}

//#idt_desc_entry_t::get_offset-doc: Gets the offset of a entry in the interrupt descriptor table.
uint64_t idt_desc_entry_t::get_offset(){ 
	uint64_t offset = 0;
	offset |= (uint64_t) offset_0;
	offset |= (uint64_t) offset_1 << 16;
	offset |= (uint64_t) offset_2 << 32;
	return offset;
}