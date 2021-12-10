#include <elf/kernel_module.h>

#include <driver/driver.h>
#include <ps2_keyboard.h>
#include <input/input.h>
#include <utils/log.h>

void device_init() {
	driver::global_driver_manager->add_driver(new ps2::ps2_keyboard());
}

define_module("ps2 keyboard", null_ptr_func, device_init, null_ptr_func);