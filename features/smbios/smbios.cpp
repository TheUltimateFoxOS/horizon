#include <smbios.h>

#include <utils/log.h>
#include <memory/memory.h>
#include <utils/string.h>

using namespace smbios;

void smbios::print_smbios_entry(smbios_entry_t* smbios) {
	debugf("SMBIOS entry point found at 0x%p.\n", smbios);

	debugf("> entry_point_signature: %c%c%c%c\n", smbios->entry_point_signature[0], smbios->entry_point_signature[1], smbios->entry_point_signature[2], smbios->entry_point_signature[3]);
	debugf("> checksum: 0x%x\n", smbios->checksum);
	debugf("> length: 0x%x\n", smbios->length);
	debugf("> major_version: 0x%x\n", smbios->major_version);
	debugf("> minor_version: 0x%x\n", smbios->minor_version);
	debugf("> max_structure_size: 0x%x\n", smbios->max_structure_size);
	debugf("> entry_point_revision: 0x%x\n", smbios->entry_point_revision);
	debugf("> entry_point_string: %c%c%c%c%c\n", smbios->entry_point_string[0], smbios->entry_point_string[1], smbios->entry_point_string[2], smbios->entry_point_string[3], smbios->entry_point_string[4]);
	debugf("> checksum_2: 0x%x\n", smbios->checksum_2);
	debugf("> table_length: 0x%x\n", smbios->table_length);
	debugf("> table_address: 0x%x\n", smbios->table_address);
	debugf("> number_of_structures: 0x%x\n", smbios->number_of_structures);
	debugf("> bcd_revision: 0x%x\n", smbios->bcd_revision);

	debugf("> structures:\n");

	smbios_structure_header_t* structure = (smbios_structure_header_t*) memory::map_if_necessary((void*) (uint64_t) smbios->table_address);

	for (uint32_t i = 0; i < smbios->number_of_structures; i++) {
		debugf("> > type: %s\n", type_to_str(structure->type));
		structure = next(structure);
	}
}

smbios_structure_header_t* smbios::next(smbios_structure_header_t* structure) {
	char* x = (char*) structure;
	x += structure->length;

	size_t len = 0;
	
	// If value at ptr is 0, then offset by 2 and return,
	//  else, get the length of the region in memory until
	//  a null byte is encountered, add that to
	if (*x == 0) {
		x += 2; 
	} else do {
		len  = strlen(x);
		x += len + 1;
	} while(len > 0);

	return (smbios_structure_header_t*) x;
}


smbios_structure_header_t* smbios::find_smbios_structure(smbios_entry_t* smbios, uint8_t type, int idx) {
	smbios_structure_header_t* structure = (smbios_structure_header_t*) memory::map_if_necessary((void*) (uint64_t) smbios->table_address);
	for (uint32_t i = 0; i < smbios->number_of_structures; i++) {		
		if (structure->type == type) {
			if (idx == 0) {
				return structure;
			} else {
				idx--;
			}
		}

		structure = next(structure);
	}

	return nullptr;
}

char* smbios::get_string(smbios_structure_header_t* structure, str_id id) {
	char* str = (char*) structure + structure->length;

	while (id - 1 != 0) {
		str += strlen(str) + 1;
		id--;
	}

	return str;
}



char* smbios::type_to_str(uint8_t type) {
	switch (type) {
		case 0: return (char*) "BIOS Information";
		case 1: return (char*) "System Information";
		case 2: return (char*) "Baseboard Information";
		case 3: return (char*) "System Enclosure";
		case 4: return (char*) "Processor Information";
		case 5: return (char*) "Memory Controller Information";
		case 6: return (char*) "Memory Module Information";
		case 7: return (char*) "Cache Information";
		case 9: return (char*) "System Slots";
		case 16: return (char*) "Physical Memory Array";
		case 17: return (char*) "Memory Device Information";
		case 19: return (char*) "Memory Array Mapped Address";
		case 32: return (char*) "System Boot Information";

		case 127: return (char*) "End Of Table";

		default: return (char*) "Unknown";
	}
}
