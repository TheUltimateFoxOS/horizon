#include <ahci.h>

#include <memory/page_frame_allocator.h>
#include <memory/page_table_manager.h>

#include <fs/gpt.h>
#include <fs/dev_fs.h>

#include <utils/log.h>

using namespace ahci;

void ahci_port::configure() {
	stop_command();

	void* new_base = memory::global_allocator.request_page();
	hba_port->command_list_base = (uint32_t)(uint64_t)new_base;
	hba_port->command_list_base_upper = (uint32_t)((uint64_t)new_base >> 32);
	memset((void*)(uint64_t)(hba_port->command_list_base), 0, 1024);

	void* fis_base = memory::global_allocator.request_page();
	hba_port->fis_base_address = (uint32_t)(uint64_t)fis_base;
	hba_port->fis_base_address_upper = (uint32_t)((uint64_t)fis_base >> 32);
	memset(fis_base, 0, 256);

	HBA_command_header* cmd_header = (HBA_command_header*)((uint64_t)hba_port->command_list_base + ((uint64_t)hba_port->command_list_base_upper << 32));

	for (int i = 0; i < 32; i++){
		cmd_header[i].prdt_length = 8;

		void* cmd_table_address = memory::global_allocator.request_page();
		uint64_t address = (uint64_t)cmd_table_address + (i << 8);
		cmd_header[i].command_table_base_address = (uint32_t)(uint64_t)address;
		cmd_header[i].command_table_base_address_upper = (uint32_t)((uint64_t)address >> 32);
		memset(cmd_table_address, 0, 256);
	}

	start_command();

	if (!fs::gpt::read_gpt(this)) {
		debugf("AHCI: Failed to read GPT. Adding disk as raw disk only!\n");
	}

	fs::global_devfs->register_file(new driver::raw_disk_dev_fs(driver::global_disk_manager->add_disk(this)));
}

void ahci_port::stop_command() {
	hba_port->cmd_sts &= ~HBA_PxCMD_ST;
	hba_port->cmd_sts &= ~HBA_PxCMD_FRE;

	while (true) {
		if (hba_port->cmd_sts & HBA_PxCMD_FR) continue;
		if (hba_port->cmd_sts & HBA_PxCMD_CR) continue;

		break;
	}
}

void ahci_port::start_command() {
	while (hba_port->cmd_sts & HBA_PxCMD_CR) {
		__asm__ __volatile__("pause");
	}

	hba_port->cmd_sts |= HBA_PxCMD_FRE;
	hba_port->cmd_sts |= HBA_PxCMD_ST;
}

void ahci_port::read(uint64_t sector, uint32_t sector_count, void* buffer) {
	void* physical_buffer = memory::global_page_table_manager.virt_to_phys(buffer);
	if (physical_buffer == NULL) {
		debugf("AHCI: Failed to get physical address of buffer!");
		return;
	}/* else {
		debugf("AHCI: Buffer is at physical address 0x%x and virtual address 0x%x.\n", physical_buffer, buffer);
	}*/

	uint64_t spin = 0;
	while ((hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
		spin ++;
	}
	if (spin == 1000000) {
		debugf("AHCI: Timeout while waiting for device to be ready!");
		return;
	}

	uint32_t sector_low = (uint32_t) sector;
	uint32_t sector_high = (uint32_t) (sector >> 32);

	hba_port->interrupt_status = (uint32_t)-1; // Clear pending interrupt bits

	HBA_command_header* cmd_header = (HBA_command_header*)(uint64_t)hba_port->command_list_base;
	cmd_header->command_fis_length = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
	cmd_header->write = 0; // Indicate a read
	cmd_header->prdt_length = 1;

	HBA_command_table* commandTable = (HBA_command_table*)(uint64_t)(cmd_header->command_table_base_address);
	memset(commandTable, 0, sizeof(HBA_command_table) + (cmd_header->prdt_length-1)*sizeof(HBA_PRDT_entry));

	commandTable->prdt_entry[0].data_base_address = (uint32_t)(uint64_t) physical_buffer;
	commandTable->prdt_entry[0].data_base_address_upper = (uint32_t)((uint64_t) physical_buffer >> 32);
	commandTable->prdt_entry[0].byte_count = (sector_count << 9) - 1; // 512 bytes per sector
	commandTable->prdt_entry[0].interrupt_on_completion = 1;

	FIS_REG_H2D* cmd_fis = (FIS_REG_H2D*)(&commandTable->command_fis);

	cmd_fis->fis_type = FIS_TYPE_REG_H2D;
	cmd_fis->command_control = 1; // Command
	cmd_fis->command = ATA_CMD_READ_DMA_EX;

	cmd_fis->lba0 = (uint8_t)sector_low;
	cmd_fis->lba1 = (uint8_t)(sector_low >> 8);
	cmd_fis->lba2 = (uint8_t)(sector_low >> 16);
	cmd_fis->lba3 = (uint8_t)sector_high;
	cmd_fis->lba4 = (uint8_t)(sector_high >> 8);
	cmd_fis->lba5 = (uint8_t)(sector_high >> 16);

	cmd_fis->device_register = 1<<6; //LBA mode

	cmd_fis->count_low = sector_count & 0xFF;
	cmd_fis->count_high = (sector_count >> 8) & 0xFF;

	hba_port->command_issue = 1;

	while (true) {
		if((hba_port->command_issue == 0)) break;
		if(hba_port->interrupt_status & HBA_PxIS_TFES) {
			return;
		}
	}

	return;
}

void ahci_port::write(uint64_t sector, uint32_t sector_count, void* buffer) {
	void* physical_buffer = memory::global_page_table_manager.virt_to_phys(buffer);
	if (physical_buffer == NULL) {
		debugf("AHCI: Failed to get physical address of buffer!");
		return;
	}/* else {
		debugf("AHCI: Buffer is at physical address 0x%x and virtual address 0x%x.\n", physical_buffer, buffer);
	}*/

	uint64_t spin = 0;
	while ((hba_port->task_file_data & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
		spin ++;
	}
	if (spin == 1000000) {
		debugf("AHCI: Timeout while waiting for device to be ready!");
		return;
	}

	uint32_t sector_low = (uint32_t) sector;
	uint32_t sector_high = (uint32_t) (sector >> 32);

	hba_port->interrupt_status = (uint32_t)-1; // Clear pending interrupt bits

	HBA_command_header* cmd_header = (HBA_command_header*)(uint64_t)hba_port->command_list_base;
	cmd_header->command_fis_length = sizeof(FIS_REG_H2D)/ sizeof(uint32_t); //command FIS size;
	cmd_header->write = 1; // Indicate a write
	cmd_header->prdt_length = 1;

	HBA_command_table* commandTable = (HBA_command_table*)(uint64_t)(cmd_header->command_table_base_address);
	memset(commandTable, 0, sizeof(HBA_command_table) + (cmd_header->prdt_length-1)*sizeof(HBA_PRDT_entry));

	commandTable->prdt_entry[0].data_base_address = (uint32_t)(uint64_t) physical_buffer;
	commandTable->prdt_entry[0].data_base_address_upper = (uint32_t)((uint64_t) physical_buffer >> 32);
	commandTable->prdt_entry[0].byte_count = (sector_count << 9) - 1; // 512 bytes per sector
	commandTable->prdt_entry[0].interrupt_on_completion = 1;

	FIS_REG_H2D* cmd_fis = (FIS_REG_H2D*)(&commandTable->command_fis);

	cmd_fis->fis_type = FIS_TYPE_REG_H2D;
	cmd_fis->command_control = 1; // Command
	cmd_fis->command = ATA_CMD_WRITE_DMA_EX;

	cmd_fis->lba0 = (uint8_t)sector_low;
	cmd_fis->lba1 = (uint8_t)(sector_low >> 8);
	cmd_fis->lba2 = (uint8_t)(sector_low >> 16);
	cmd_fis->lba3 = (uint8_t)sector_high;
	cmd_fis->lba4 = (uint8_t)(sector_high >> 8);
	cmd_fis->lba5 = (uint8_t)(sector_high >> 16);

	cmd_fis->device_register = 1<<6; //LBA mode

	cmd_fis->count_low = sector_count & 0xFF;
	cmd_fis->count_high = (sector_count >> 8) & 0xFF;

	hba_port->command_issue = 1;

	while (true) {
		if((hba_port->command_issue == 0)) break;
		if(hba_port->interrupt_status & HBA_PxIS_TFES) {
			return;
		}
	}

	return;
}