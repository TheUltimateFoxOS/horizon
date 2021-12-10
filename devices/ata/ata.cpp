#include <ata.h>

#include <utils/log.h>
#include <fs/gpt.h>

using namespace driver;

advanced_technology_attachment::advanced_technology_attachment(bool master, uint16_t portBase, char* name): dataPort(portBase), error_port(portBase + 0x1), sector_count_port(portBase + 0x2), lba_low_port(portBase + 0x3), lba_mid_port(portBase + 0x4), lba_hi_port(portBase + 0x5), device_port(portBase + 0x6), command_port(portBase + 0x7), control_port(portBase + 0x206) {
	this->master = master;
	this->name = name;
	this->bytes_per_sector = 512;
}

bool advanced_technology_attachment::is_presend() {
	device_port.Write(master ? 0xA0 : 0xB0);
	control_port.Write(0);
	
	device_port.Write(0xA0);
	uint8_t status = command_port.Read();
	if(status == 0xFF) {
		return false;
	}
	
	
	device_port.Write(master ? 0xA0 : 0xB0);
	sector_count_port.Write(0);
	lba_low_port.Write(0);
	lba_mid_port.Write(0);
	lba_hi_port.Write(0);
	command_port.Write(0xEC); // identify command
	
	
	status = command_port.Read();
	if(status == 0x00) {
		return false;
	}
	
	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
		status = command_port.Read();
	}

	if(status & 0x01) {
		return false;
	}


	for(int i = 0; i < 256; i++) {
		uint16_t data = dataPort.Read();
		char *text = (char*) "  \0";
		text[0] = (data >> 8) & 0xFF;
		text[1] = data & 0xFF;
	}

	return true;
}

void advanced_technology_attachment::activate() {
	if (!fs::gpt::read_gpt(this)) {
		debugf("ATA: Failed to read GPT. Adding disk as raw disk!\n");
		driver::global_disk_manager->add_disk(this);
	}
}

void advanced_technology_attachment::read28(uint32_t sector, uint8_t* data, int count) {
	if(sector & 0xF0000000) {
		return;
	}
	if(count > bytes_per_sector) {
		return;
	}

	device_port.Write((master ? 0xE0 : 0xF0) | ((sector & 0x0F000000) >> 24));
	error_port.Write(0);
	sector_count_port.Write(1);

	lba_low_port.Write(sector & 0x000000FF);
	lba_mid_port.Write((sector & 0x0000FF00) >> 8);
	lba_hi_port.Write((sector & 0x00FF0000) >> 16);
	command_port.Write(0x20);



	uint8_t status = command_port.Read();
	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
		status = command_port.Read();
	}

	if(status & 0x01) {
		return;
	}

	for(uint16_t i = 0; i < count; i += 2) {
		uint16_t wdata = dataPort.Read();
		
		data[i] = wdata & 0x00FF;
		if(i + 1 < count) {
			data[i + 1] = (wdata >> 8) & 0x00FF;
		}
	}

	for(uint16_t i = count + (count % 2); i < bytes_per_sector; i+= 2) {
		dataPort.Read();
	}
}

void advanced_technology_attachment::write28(uint32_t sectorNum, uint8_t* data, uint32_t count) {
	if(sectorNum > 0x0FFFFFFF) {
		return;
	}
	if(count > bytes_per_sector) {
		return;
	}


	device_port.Write((master ? 0xE0 : 0xF0) | ((sectorNum & 0x0F000000) >> 24));
	error_port.Write(0);
	sector_count_port.Write(1);
	lba_low_port.Write(sectorNum & 0x000000FF);
	lba_mid_port.Write((sectorNum & 0x0000FF00) >> 8);
	lba_hi_port.Write((sectorNum & 0x00FF0000) >> 16);
	command_port.Write(0x30);


	for(int i = 0; i < (int) count; i += 2) {
		uint16_t wdata = data[i];
		if(i + 1 < (int) count) {
			wdata |= ((uint16_t)data[i + 1]) << 8;
		}
		dataPort.Write(wdata);
	}
	
	for(int i = count + (count % 2); i < 512; i += 2) {
		dataPort.Write(0x0000);
	}
}

void advanced_technology_attachment::flush() {
	device_port.Write(master ? 0xE0 : 0xF0);
	command_port.Write(0xE7);

	uint8_t status = command_port.Read();
	if(status == 0x00) {
		return;
	}

	while(((status & 0x80) == 0x80) && ((status & 0x01) != 0x01)) {
		status = command_port.Read();
	}
	
	if(status & 0x01) {
		return;
	}
}

void advanced_technology_attachment::read(uint64_t sector, uint32_t sector_count, void* buffer) {
	for (int i = 0; i < sector_count; i++) {
		read28(sector + i, (uint8_t*) buffer + (i * bytes_per_sector), bytes_per_sector);
	}	
}

void advanced_technology_attachment::write(uint64_t sector, uint32_t sector_count, void* buffer) {
	for (int i = 0; i < sector_count; i++) {
		write28(sector + i, (uint8_t*) buffer + (i * bytes_per_sector), bytes_per_sector);
		flush();
	}
}

char* advanced_technology_attachment::get_name() {
	return this->name;
}