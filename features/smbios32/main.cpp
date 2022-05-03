#include <elf/kernel_module.h>

#include <boot/boot.h>
#include <utils/log.h>

#include <smbios32.h>
#include <syscalls.h>

void init() {
	if (boot::boot_info.smbios_entry_32 == nullptr) {
		debugf("No SMBIOS32 entry point found.\n");
	} else {
		smbios32::print_smbios_entry((smbios32::smbios_entry_t*) boot::boot_info.smbios_entry_32);

		smbios32::smbios_structure_header_t* bios_info = smbios32::find_smbios_structure((smbios32::smbios_entry_t*) boot::boot_info.smbios_entry_32, 0, 0);
		smbios32::bios_information_t* bios = (smbios32::bios_information_t*) ((uint64_t) bios_info + sizeof(smbios32::smbios_structure_header_t));
		debugf("BIOS information:\n");
		debugf("  Vendor: %s\n", smbios32::get_string(bios_info, bios->vendor));
		debugf("  Version: %s\n", smbios32::get_string(bios_info, bios->version));
		debugf("  Release date: %s\n", smbios32::get_string(bios_info, bios->release_date));


		int i = 0;
		smbios32::smbios_structure_header_t* processor_info = nullptr;
		while ((processor_info = smbios32::find_smbios_structure((smbios32::smbios_entry_t*) boot::boot_info.smbios_entry_32, 4, i++)) != nullptr) {
			smbios32::processor_information_t* processor = (smbios32::processor_information_t*) ((uint64_t) processor_info + sizeof(smbios32::smbios_structure_header_t));

			debugf("Processor information:\n");
			debugf("  Socket designation: %s\n", smbios32::get_string(processor_info, processor->socket_designation));
			debugf("  Processor manufacturer: %s\n", smbios32::get_string(processor_info, processor->manufacturer));
			debugf("  Processor version: %s\n", smbios32::get_string(processor_info, processor->processor_version));
		}

		syscall::register_syscall(syscall::sys_sm32_get_cpu_info, "sys_sm32_get_cpu_info");
	}
}

define_module("smbios", init, null_ptr_func, null_ptr_func);