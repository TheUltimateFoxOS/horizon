#include <syscall/syscall.h>

#include <elf/kernel_module.h>

#include <driver/driver.h>

#include <utils/log.h>
#include <utils/assert.h>

using namespace syscall;

void syscall::sys_insmod(interrupts::s_registers* regs) {
	void* module = (void*) regs->rbx;
	int size = regs->rcx;

	debugf("sys_insmod: %p %d\n", module, size);

	elf::module_t* loaded_module = elf::load_kernel_module(module, size);

	assert(loaded_module->device_init == null_ptr_func);
	assert(loaded_module->fs_init == null_ptr_func);

	driver::global_driver_manager->activate_all(false);
}
