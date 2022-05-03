#include <syscalls.h>
#include <smbios.h>
#include <boot/boot.h>
#include <utils/log.h>

using namespace syscall;
using namespace smbios;

void syscall::sys_get_cpu_info(interrupts::s_registers* regs) {
	debugf("sys_get_cpu_info()\n");
	cpu_info_t* cpu_info = (cpu_info_t*) regs->rbx;

	int i = 0;
	smbios::smbios_structure_header_t* processor_info = nullptr;
	while ((processor_info = smbios::find_smbios_structure((smbios::smbios_entry_t*) boot::boot_info.smbios_entry_32, 4, i++)) != nullptr) {
		smbios::processor_information_t* processor = (smbios::processor_information_t*) ((uint64_t) processor_info + sizeof(smbios::smbios_structure_header_t));

		memset(&cpu_info[i - 1], 0, sizeof(cpu_info_t));
		cpu_info[i - 1].id = i;
		cpu_info[i - 1].speed = processor->current_speed;
		strcpy(cpu_info[i - 1].manufacture, smbios::get_string(processor_info, processor->manufacturer));
		strcpy(cpu_info[i - 1].version, smbios::get_string(processor_info, processor->processor_version));
	}

	regs->rcx = i - 1;
}
