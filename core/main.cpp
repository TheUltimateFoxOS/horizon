#include <stivale2.h>
#include <gdt.h>

#include <driver/driver.h>
#include <driver/disk.h>
#include <driver/nic.h>

#include <interrupts/interrupts.h>

#include <output/stivale2_terminal.h>
#include <output/serial_port.h>

#include <renderer/renderer.h>
#include <renderer/render2d.h>
#include <renderer/font_renderer.h>
#include <renderer/boot_screen_renderer.h>
#include <renderer/window_renderer.h>

#include <utils/abort.h>
#include <utils/string.h>
#include <utils/argparse.h>
#include <utils/log.h>

#include <elf/elf_resolver.h>
#include <elf/elf_loader.h>
#include <elf/kernel_module.h>
#include <fs/vfs.h>
#include <fs/fd.h>
#include <fs/dev_fs.h>
#include <fs/stivale_modules.h>

#include <timer/timer.h>

#include <pci/pci.h>

#include <acpi/madt.h>
#include <apic/apic.h>

#include <scheduler/scheduler.h>

#include <syscall/syscall.h>

#include <memory/memory.h>

#include <config.h>

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

#ifndef NICE_BOOT_ANIMATION
	renderer::global_font_renderer->clear(0);
	renderer::global_renderer_2d->load_bitmap(logo, 0);
#endif

	timer::setup();

#ifdef NICE_BOOT_ANIMATION
	renderer::boot_screen_renderer* boot_screen_renderer = new renderer::boot_screen_renderer(&renderer::default_framebuffer);
#endif 

	fs::vfs::setup();

	debugf("Mounting stivale modules vfs mount...\n");
	fs::stivale_mount* stivale_mount = new fs::stivale_mount(global_bootinfo);
	fs::global_vfs->register_mount((char*) "stivale", stivale_mount);

	debugf("Creating devfs mount...\n");
	fs::dev_fs* dev_fs_mount = new fs::dev_fs();
	fs::global_devfs = dev_fs_mount;
	fs::global_vfs->register_mount((char*) "dev", dev_fs_mount);

	fs::global_fd_manager = new fs::file_descriptor_manager();

	driver::global_driver_manager = new driver::driver_manager();
	driver::global_disk_manager = new driver::disk_driver_manager();
	driver::global_nic_manager = new driver::nic_driver_manager();

	acpi::madt::parse_madt(global_bootinfo);
	apic::setup();
	// apic::smp_spinup(global_bootinfo);

	setup_global_argparser(global_bootinfo);

	if (global_argparser->is_arg("--serial_to_screen_redirect")) {
		debugf("Redirecting serial port to screen\n");
		log::debug_device = renderer::global_font_renderer;
	}

	if (!global_argparser->is_arg("--no_smp")) {
		apic::smp_spinup(global_bootinfo);
    }

	scheduler::setup();
	syscall::setup();

	printf("Loading kernel modules...\n");
	char* kernel_module_path = nullptr;
	while ((kernel_module_path = global_argparser->get_arg("--load_module"))) {
		debugf("Loading module: %s\n", kernel_module_path);
		elf::load_kernel_module(kernel_module_path, true);
	}

	pci::enumerate_pci();

	// fs::vfs::file_t* test = fs::global_vfs->open("stivale:limine.cfg");

	// char buffer[512] = {0};
	// fs::global_vfs->read(test, buffer, test->size > 512 ? 512 : test->size, 0);

	// printf("%s", buffer);

	// elf::load_kernel_module("stivale:ps2_keyboard.o");

	elf::device_init_all();
	// driver::global_driver_manager->add_driver(new driver::device_driver());
	//init drivers here
	printf("\nLoading drivers...\n");
	driver::global_driver_manager->activate_all(false);

	printf("\nLoading network stack...\n");
	driver::load_network_stack();

	elf::fs_init_all();


	// scheduler::create_task((void*) (void (*)()) []() {
	// 	int i = 1000;
	// 	while (i--) {
	// 		debugf("A");
	// 	}
	// });

	// scheduler::create_task((void*) (void (*)()) []() {
	// 	int i = 1000;
	// 	while (i--) {
	// 		debugf("B");
	// 	}
	// });

	// scheduler::create_task((void*) (void (*)()) []() {
	// 	int i = 1000;
	// 	while (i--) {
	// 		debugf("C");
	// 	}
	// });


	// scheduler::create_task((void*) (void (*)()) []() {
	// 	int i = 1000;
	// 	while (i--) {
	// 		debugf("D");
	// 	}
	// });

	// scheduler::create_task((void*) (void (*)()) []() {
	// 	int i = 1000;
	// 	while (i--) {
	// 		debugf("E");
	// 	}
	// });

	// scheduler::create_task((void*) (void (*)()) []() {
	// 	int i = 1000;
	// 	while (i--) {
	// 		debugf("F");
	// 	}
	// });

	// scheduler::create_task((void*) (void (*)()) []() {
	// 	int i = 1000;
	// 	while (i--) {
	// 		debugf("G");
	// 	}
	// });

	if (global_argparser->is_arg("--autoexec")) {
		char* autoexec_path = global_argparser->get_arg("--autoexec");
		
		const char* autoexec_args[] = {
			autoexec_path,
			nullptr
		};

		const char* autoexec_envp[] = {
			nullptr
		};

		scheduler::task_t* task = elf::load_elf(autoexec_path, autoexec_args, autoexec_envp);
		if (task == nullptr) {
			abortf("Failed to load autoexec: %s\n", autoexec_path);
		}
	}

	// fs::vfs::file_t* ps2_device = fs::global_vfs->open("dev:ps2_keyboard");
	// char buffer[] = {0x1, 0x0};
	// fs::global_vfs->write(ps2_device, buffer, 2, 0);
	// fs::global_vfs->close(ps2_device);

	// printf("Rot dir:\n");
	// fs::vfs::dir_t root_dir = fs::global_vfs->dir_at(0, "fat32_0:/");
	// while(!root_dir.is_none) {
	// 	printf("Root dir file %d: %s\n", root_dir.idx, root_dir.name);
	// 	root_dir = fs::global_vfs->dir_at(root_dir.idx + 1, "fat32_0:/");
	// }

	// fs::global_vfs->delete_(fs::vfs::global_vfs->open("fat32_0:/limine.cfg"));

	// printf("Rot dir (deleted limine.cfg):\n");
	// fs::vfs::dir_t root_dir2 = fs::global_vfs->dir_at(0, "fat32_0:/");
	// while(!root_dir2.is_none) {
	// 	printf("Root dir file %d: %s\n", root_dir2.idx, root_dir2.name);
	// 	root_dir2 = fs::global_vfs->dir_at(root_dir2.idx + 1, "fat32_0:/");
	// }

	// fs::global_vfs->mkdir("fat32_0:/test");

	// printf("Rot dir (created test dir):\n");
	// fs::vfs::dir_t root_dir3 = fs::global_vfs->dir_at(0, "fat32_0:/");
	// while(!root_dir3.is_none) {
	// 	printf("Root dir file %d: %s\n", root_dir3.idx, root_dir3.name);
	// 	root_dir3 = fs::global_vfs->dir_at(root_dir3.idx + 1, "fat32_0:/");
	// }

	// while (true) {}
#ifdef NICE_BOOT_ANIMATION
	delete boot_screen_renderer;
#endif

	printf("\nWelcome to FoxOS Horizon!\n\n");

	scheduler::start();

	while(1) {
		__asm__ __volatile__("hlt");
	}
}
