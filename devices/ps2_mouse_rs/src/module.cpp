#include <elf/kernel_module.h>

extern "C" void init();
extern "C" void device_init();

define_module("ps2_mouse", init, device_init, null_ptr_func);