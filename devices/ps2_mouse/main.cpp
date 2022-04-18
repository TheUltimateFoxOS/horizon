#include <elf/kernel_module.h>

#include <ps2_mouse.h>
#include <driver/driver.h>

void device_init() {
	ps2::ps2_mouse* mouse = new ps2::ps2_mouse();
	driver::global_driver_manager->add_driver(mouse);
}

define_module("ps2 mouse", null_ptr_func, device_init, null_ptr_func);