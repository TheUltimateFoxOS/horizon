#include <syscalls.h>
#include <smbios32.h>
#include <boot/boot.h>
#include <utils/log.h>

using namespace syscall;
using namespace smbios32;

void syscall::sys_sm32_get_cpu_info(interrupts::s_registers* regs) {
	debugf("sys_get_cpu_info()\n");
	cpu_info_t* cpu_info = (cpu_info_t*) regs->rbx;

	int i = 0;
	smbios32::smbios_structure_header_t* processor_info = nullptr;
	while ((processor_info = smbios32::find_smbios_structure((smbios32::smbios_entry_t*) boot::boot_info.smbios_entry_32, 4, i++)) != nullptr) {
		smbios32::processor_information_t* processor = (smbios32::processor_information_t*) ((uint64_t) processor_info + sizeof(smbios32::smbios_structure_header_t));

		memset(&cpu_info[i - 1], 0, sizeof(cpu_info_t));
		cpu_info[i - 1].id = i;
		cpu_info[i - 1].speed = processor->current_speed;
		strcpy(cpu_info[i - 1].manufacture, smbios32::get_string(processor_info, processor->manufacturer));
		strcpy(cpu_info[i - 1].version, smbios32::get_string(processor_info, processor->processor_version));
	}

	regs->rcx = i - 1;
}
