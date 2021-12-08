#include <stivale2.h>
#include <gdt.h>

#include <driver/driver.h>

#include <interrupts/interrupts.h>

#include <output/stivale2_terminal.h>
#include <output/serial_port.h>

#include <renderer/renderer.h>
#include <renderer/render2d.h>
#include <renderer/font_renderer.h>

#include <utils/abort.h>
#include <utils/string.h>

#include <elf/elf_resolver.h>
#include <elf/kernel_module.h>
#include <fs/vfs.h>
#include <fs/stivale_modules.h>

#include <memory/memory.h>

#include <utils/log.h>

extern uint8_t logo[];

extern "C" void main() {

	output::stivale2_terminal terminal = output::stivale2_terminal();
	output::global_terminal = &terminal;
	log::stdout_device = output::global_terminal;

	output::serial_port serial_port = output::serial_port(COM1);
	output::global_serial_port = &serial_port;
	log::debug_device = output::global_serial_port;


	debugf("Hello, world!\n");
	printf("Hello, world!\n");

	setup_gdt();
	memory::prepare_memory(global_bootinfo);
	interrupts::prepare_interrupts();
	elf::setup(global_bootinfo);
	renderer::setup(global_bootinfo);

	renderer::global_font_renderer->clear(0);
	renderer::global_renderer_2d->load_bitmap(logo, 0);

	fs::vfs::setup();

	debugf("Mounting stivale modules vfs mount...\n");
	fs::stivale_mount* stivale_mount = new fs::stivale_mount(global_bootinfo);
	fs::global_vfs->register_mount((char*) "stivale", stivale_mount);

	// fs::vfs::file_t* test = fs::global_vfs->open("stivale:limine.cfg");

	// char buffer[512] = {0};
	// fs::global_vfs->read(test, buffer, test->size > 512 ? 512 : test->size, 0);

	// printf("%s", buffer);

	// elf::load_kernel_module("stivale:test_mod.o");

	elf::device_init_all();
	// driver::global_driver_manager->add_driver(new driver::device_driver());
	//init drivers here
	driver::global_driver_manager->activate_all(false);

	elf::fs_init_all();

	printf("Welcome to FoxOS Horizon!\n");

	while(1) {
		__asm__ __volatile__("hlt");
	}
}