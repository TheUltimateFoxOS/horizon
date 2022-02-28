#include <driver/disk.h>

#include <utils/log.h>

using namespace driver;

disk_driver_manager* driver::global_disk_manager;


disk_device::disk_device() {

}

disk_device::~disk_device() {

}

void disk_device::read(uint64_t sector, uint32_t sector_count, void* buffer) {

}

void disk_device::write(uint64_t sector, uint32_t sector_count, void* buffer) {

}

bool disk_device::get_disk_label(char* out, fs::vfs::vfs_mount* mount) {
	fs::vfs::file_t* file = mount->open((char*) "/FOXCFG//dn.fox");

	if (file == nullptr) {
		debugf("Failed to open /dn.fox\n");
		return false;
	}

	mount->read(file, out, file->size, 0);
	out[file->size] = 0;

	mount->close(file);
	debugf("Disk label: %s\n", out);

	return true;
}


disk_driver_manager::disk_driver_manager() {
	this->num_disks = 0;
}

int disk_driver_manager::add_disk(disk_device* disk) {
	this->disks[this->num_disks] = disk;
	debugf("Adding new disk at idx %d!\n", this->num_disks);
	this->num_disks++;

	return this->num_disks - 1;
}

void disk_driver_manager::read(int disk_num, uint64_t sector, uint32_t sector_count, void* buffer) {
	this->disks[disk_num]->read(sector, sector_count, buffer);
}

void disk_driver_manager::write(int disk_num, uint64_t sector, uint32_t sector_count, void* buffer) {
	this->disks[disk_num]->write(sector, sector_count, buffer);
}

raw_disk_dev_fs::raw_disk_dev_fs(int disk_num) {
	memset(this->name, 0, 32);
	sprintf(this->name, "disk_%d", disk_num);

	this->disk_num = disk_num;
}

char* raw_disk_dev_fs::get_name() {
	return this->name;
}

void raw_disk_dev_fs::write(fs::file_t* file, void* buffer, size_t size, size_t offset) {
	raw_disk_dev_fs_command* cmd = (raw_disk_dev_fs_command*) buffer;

	switch (cmd->command) {
		case 0: // opcode read
			{
				driver::global_disk_manager->read(this->disk_num, cmd->sector, cmd->sector_count, (void*) cmd->buffer);
			}
			break;
		
		case 1: // opcode write
			{
				driver::global_disk_manager->write(this->disk_num, cmd->sector, cmd->sector_count, (void*) cmd->buffer);
			}
			break;
		
		default:
			{
				debugf("Unknown disk command %d\n", cmd->command);
			}
			break;
	}
}