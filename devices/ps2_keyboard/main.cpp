#include <elf/kernel_module.h>

#include <driver/driver.h>
#include <ps2_keyboard.h>
#include <input/input.h>
#include <utils/log.h>
#include <fs/dev_fs.h>

void device_init() {
	ps2::ps2_keyboard* keyboard = new ps2::ps2_keyboard();
	driver::global_driver_manager->add_driver(keyboard);
	fs::global_devfs->register_file(keyboard);
}

define_module("ps2 keyboard", null_ptr_func, device_init, null_ptr_func);