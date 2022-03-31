#include <pc_speaker.h>

#include <elf/kernel_module.h>

void init() {
	driver::global_driver_manager->add_driver(new sound::pc_speaker_driver());
}

define_module("pc_speaker", init, null_ptr_func, null_ptr_func);