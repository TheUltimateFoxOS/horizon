#pragma once

#include <stdint.h>

namespace smbios {
	struct smbios_entry_t {
		char entry_point_signature[4];
		uint8_t checksum;
		uint8_t length;
		uint8_t major_version;
		uint8_t minor_version;
		uint16_t max_structure_size;
		uint8_t entry_point_revision;
		char formatted_area[5];
		char entry_point_string[5];
		uint8_t checksum_2;
		uint16_t table_length;
		uint32_t table_address;
		uint16_t number_of_structures;
		uint8_t bcd_revision;
	} __attribute__((packed));

	struct smbios_structure_header_t {
		uint8_t type;
		uint8_t length;
		uint16_t handle;
	} __attribute__((packed));

	typedef uint8_t str_id;

	struct bios_information_t {
		str_id vendor;
		str_id version;
		uint16_t start_segment;
		str_id release_date;
		uint8_t rom_size;
		uint64_t characteristics;
	} __attribute__((packed));

	struct processor_information_t {
		str_id socket_designation;
		uint8_t processor_type;
		uint8_t processor_family;
		str_id manufacturer;
		uint64_t processor_id;
		str_id processor_version;
		uint8_t voltage;
		uint16_t external_clock;
		uint16_t max_speed;
		uint16_t current_speed;
		uint8_t status;
		uint8_t processor_upgrade;
	} __attribute__((packed));

	void print_smbios_entry(smbios_entry_t* smbios);

	smbios_structure_header_t* next(smbios_structure_header_t* structure);
	smbios_structure_header_t* find_smbios_structure(smbios_entry_t* smbios, uint8_t type, int idx);
	char* get_string(smbios_structure_header_t* structure, str_id id);

	char* type_to_str(uint8_t type);
}