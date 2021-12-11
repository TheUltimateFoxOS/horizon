#pragma once

#include <fs/vfs.h>
#include <driver/disk.h>

namespace fs {
	using namespace vfs;

	struct BPB_t {
		uint8_t BS_jump_boot[3];
		uint8_t BS_oem_name[8];
		uint16_t bytes_per_sector;
		uint8_t sectors_per_cluster;
		uint16_t reserved_sector_count;
		uint8_t num_fats;
		uint16_t root_entry_count;
		uint16_t total_sectors_16;
		uint8_t media;
		uint16_t fat_size_16;
		uint16_t sectors_per_track;
		uint16_t number_of_heads;
		uint32_t hidden_sectors;
		uint32_t total_sectors_32;
	} __attribute__ ((__packed__));

	bool is_fat32(BPB_t* bpb);
	bool is_fat32(int disk_id);

	uint32_t get_fattime();
}