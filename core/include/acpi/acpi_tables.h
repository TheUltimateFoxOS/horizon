#pragma once

#include <stdint.h>

namespace acpi {
	
	struct rsdp2_t {
		uint8_t signature[8];
		uint8_t checksum;
		uint8_t oem_id[6];
		uint8_t revision;
		uint32_t rsdt_address;
		uint32_t length;
		uint64_t xsdt_address;
		uint8_t extended_checksum;
		uint8_t reserved[3];
	} __attribute__((packed));

	struct sdt_header_t {
		uint8_t signature[4];
		uint32_t length;
		uint8_t revision;
		uint8_t checksum;
		uint8_t oem_id[6];
		uint8_t oem_table_id[8];
		uint32_t oem_revision;
		uint32_t creator_id;
		uint32_t creator_revision;
	} __attribute__((packed));

	struct mcfg_header_t {
		sdt_header_t header;
		uint64_t reserved;
	} __attribute__((packed));

	struct hpet_table_t {
		sdt_header_t header;
		uint8_t hardware_rev_id;
		uint8_t info;
		uint16_t pci_id;
		uint8_t address_space_id;
		uint8_t register_width;
		uint8_t register_offset;
		uint8_t reserved;
		uint64_t address;
		uint8_t hpet_num;
		uint16_t minim_ticks;
		uint8_t page_protection;
	} __attribute__((packed));

		struct mcfg_device_config_t {
			uint64_t base_address;
			uint16_t pci_seg_group;
			uint8_t start_bus;
			uint8_t end_bus;
			uint32_t reserved;
		} __attribute__((packed));
}