#include <driver/virtual_disk.h>

using namespace driver;

virtual_disk_device::virtual_disk_device(disk_device* disk, uint64_t lba_offset) {
	this->disk = disk;
	this->lba_offset = lba_offset;
}

virtual_disk_device::~virtual_disk_device() {
}

void virtual_disk_device::read(uint64_t sector, uint32_t sector_count, void* buffer) {
	uint64_t lba = sector + lba_offset;
	disk->read(lba, sector_count, buffer);
}

void virtual_disk_device::write(uint64_t sector, uint32_t sector_count, void* buffer) {
	uint64_t lba = sector + lba_offset;
	disk->write(lba, sector_count, buffer);
}