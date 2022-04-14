#include <fat32_utils.h>

#include <driver/clock.h>
#include <driver/disk.h>

#include <memory/page_frame_allocator.h>

#include <utils/log.h>

using namespace fs;

bool fs::is_fat32(BPB_t* bpb) {
	if (bpb == nullptr) {
		return false;
	}
	if (bpb == 0) {
		return false;
	}

	if (bpb->root_entry_count != 0) {
		return false;
	}
	if (bpb->bytes_per_sector == 0) {
		return false;
	}
	if (bpb->sectors_per_cluster == 0) {
		return false;
	}

	uint16_t root_dir_sectors = ((bpb->root_entry_count * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;

	uint32_t fat_size;
	if (bpb->total_sectors_16 != 0) {
		fat_size = bpb->total_sectors_16;
	} else {
		fat_size = bpb->total_sectors_32;
	}

	uint32_t total_sectors;
	if (bpb->total_sectors_16 != 0) {
		total_sectors = bpb->total_sectors_16;
	} else {
		total_sectors = bpb->total_sectors_32;
	}

	uint32_t data_sectors = total_sectors - (bpb->reserved_sector_count + (bpb->num_fats * fat_size) + root_dir_sectors);
	uint32_t cluster_count = data_sectors / bpb->sectors_per_cluster;

	if (cluster_count < 4085) {
		return false; //FAT12
	} else if (cluster_count < 65525) {
		return false; //FAT16
	}

	if (bpb->BS_jump_boot[0] == 0xEB) {
		if (bpb->BS_jump_boot[2] != 0x90) {
			return false;
		}
	} else if (bpb->BS_jump_boot[0] != 0xE9) {
		return false;
	}

	if (bpb->bytes_per_sector != 512 && bpb->bytes_per_sector != 1024 && bpb->bytes_per_sector != 2048 && bpb->bytes_per_sector != 4096) {
		return false;
	}

	if (bpb->sectors_per_cluster != 1 && bpb->sectors_per_cluster % 2 != 2) {
		return false;
	}
	if (bpb->sectors_per_cluster * bpb->bytes_per_sector > 32 * 1024) { //Too large
		return false;
	}

	if (bpb->reserved_sector_count == 0) {
		return false;
	}

	if (bpb->num_fats < 2) {
		return false;
	}

	if (bpb->root_entry_count != 0) {
		return false;
	}

	if (bpb->media != 0xF0 && bpb->media < 0xF8) {
		return false;
	}

	if (bpb->total_sectors_16 != 0) {
		return false;
	}
	if (bpb->total_sectors_32 == 0) {
		return false;
	}

	return true;
}

bool fs::is_fat32(int disk_id, uint8_t* buffer) {
	driver::global_disk_manager->disks[disk_id]->read(0, 1, (void*) buffer);
	return is_fat32((BPB_t*) buffer);
}

uint32_t fs::get_fattime() {
	if (driver::default_clock_device == nullptr) {
		debugf("WARNING: No clock device found!\n");
		return 0;
	}
	driver::clock_device::clock_result_t result = driver::default_clock_device->time();

	return ((uint32_t) (result.year - 1980) << 25)
			| ((uint32_t) result.month << 21)
			| ((uint32_t) result.day << 16)
			| ((uint32_t) result.hours << 11)
			| ((uint32_t) result.minutes << 5)
			| ((uint32_t) result.seconds >> 1);
}