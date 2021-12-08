#include <elf/kernel_module.h>

#include <driver/driver.h>
#include <ps2_keyboard.h>
#include <input/input.h>
#include <utils/log.h>

void init() {}

void device_init() {
	driver::global_driver_manager->add_driver(new ps2_keyboard());
}

void fs_init() {}

define_module("ps2 keyboard", init, device_init, fs_init);