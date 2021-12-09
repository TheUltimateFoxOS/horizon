#include <fs/gpt.h>

#include <memory/page_frame_allocator.h>

#include <utils/string.h>

#include <utils/log.h>
#include <driver/disk.h>
#include <driver/virtual_disk.h>

using namespace fs;
using namespace fs::gpt;

bool fs::gpt::read_gpt(driver::disk_device* disk) {
	gpt_header* header = (gpt_header*) memory::global_allocator.request_page();
	disk->read(1, 1, (void*) header);

	if (memcmp(header->signature, "EFI PART", 8) != 0) {
		memory::global_allocator.free_page(header);
		return false;
	} else {
		gpt_partition_entry* entries = (gpt_partition_entry*) memory::global_allocator.request_pages(((header->partition_entries_size * header->partition_entries_count) / 4096) + 1);
		disk->read(header->partition_entries_startting_lba, ((header->partition_entries_size * header->partition_entries_count) / 512 ) + 1, (void*) entries);

		for (int i = 0; i < header->partition_entries_count; i++) {
			if (entries[i].type_guid.data1 == 0) {
				continue;
			}
			
			debugf("Partition guid: %x, index: %d, partition start lba: %d\n", entries[i].type_guid.data3, i, entries[i].first_lba);

			driver::virtual_disk_device* vdisk = new driver::virtual_disk_device(disk, entries[i].first_lba);
			driver::global_disk_manager->add_disk(vdisk);
		}
		

		memory::global_allocator.free_pages(entries, ((header->partition_entries_size * header->partition_entries_count) / 4096) + 1);
		memory::global_allocator.free_page(header);
		return true;
	}
}