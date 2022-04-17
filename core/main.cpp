#include <gdt.h>

#include <driver/driver.h>
#include <driver/disk.h>
#include <driver/nic.h>

#include <interrupts/interrupts.h>

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
#include <utils/assert.h>

#include <elf/elf_resolver.h>
#include <elf/elf_loader.h>
#include <elf/kernel_module.h>
#include <fs/vfs.h>
#include <fs/fd.h>
#include <fs/dev_fs.h>
#include <fs/modules_modules.h>
#include <fs/saf.h>
#include <input/input.h>

#include <timer/timer.h>

#include <pci/pci.h>

#include <acpi/madt.h>
#include <apic/apic.h>

#include <scheduler/scheduler.h>

#include <syscall/syscall.h>

#include <memory/memory.h>
#include <memory/page_frame_allocator.h>

#include <config.h>

extern uint8_t logo[];

#ifdef NICE_BOOT_ANIMATION
#define BOOT_PROGRESS(progress) debugf("boot: %d%%\n", progress); boot_screen_renderer->set_progress(progress)
#else
#define BOOT_PROGRESS(progress) debugf("boot: %d%%\n", progress)
#endif

extern "C" void main() {


	output::serial_port serial_port = output::serial_port(COM1);
	output::global_serial_port = &serial_port;
	log::stdout_device = output::global_serial_port;
	log::debug_device = output::global_serial_port;


	debugf("Hello, world!\n");
	debugf("Booting FoxOS using the %s boot protocol...\n", boot::boot_info.boot_protocol_name);

	setup_gdt();
	memory::prepare_memory();
	interrupts::prepare_interrupts();
	elf::setup();
	renderer::setup();

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
	fs::modules_mount* modules_mount = new fs::modules_mount(&boot::boot_info);
	fs::global_vfs->register_mount((char*) "modules", modules_mount);

	BOOT_PROGRESS(10);

	debugf("Creating devfs mount...\n");
	fs::dev_fs* dev_fs_mount = new fs::dev_fs();
	fs::global_devfs = dev_fs_mount;
	fs::global_vfs->register_mount((char*) "dev", dev_fs_mount);
	fs::global_devfs->register_file(renderer::global_font_renderer);

	fs::global_fd_manager = new fs::file_descriptor_manager();

	BOOT_PROGRESS(15);

	driver::global_driver_manager = new driver::driver_manager();
	driver::global_disk_manager = new driver::disk_driver_manager();
	driver::global_nic_manager = new driver::nic_driver_manager();

	BOOT_PROGRESS(20);

	acpi::init();
	acpi::madt::parse_madt();
	apic::setup();

	// apic::smp_spinup(global_bootinfo);

	BOOT_PROGRESS(25);

	setup_global_argparser();

	if (global_argparser->is_arg("--serial_to_screen_redirect")) {
		debugf("Redirecting serial port to screen\n");
		log::debug_device = renderer::global_font_renderer;
	}

	if (global_argparser->is_arg("--keymap_load_path")) {
		strcpy(input::keymap_load_path, global_argparser->get_arg("--keymap_load_path"));
	}

	if (global_argparser->is_arg("--initrd")) {
		debugf("Loading initrd...\n");
		fs::vfs::file_t* initrd = fs::vfs::global_vfs->open(global_argparser->get_arg("--initrd"));
		assert(initrd != nullptr);

		void* data = memory::global_allocator.request_pages(initrd->size / 4096 + 1);
		fs::vfs::global_vfs->read(initrd, data, initrd->size, 0);
		fs::vfs::global_vfs->close(initrd);

		fs::saf_mount* saf_mount = new fs::saf_mount(data);
		fs::global_vfs->register_mount((char*) "initrd", saf_mount);
	}

	if (!global_argparser->is_arg("--no_smp")) {
		apic::smp_spinup();
    }

	BOOT_PROGRESS(30);

	scheduler::setup();
	syscall::setup();

	BOOT_PROGRESS(35);

	printf("Loading kernel modules...\n");
	char* kernel_module_path = nullptr;
	while ((kernel_module_path = global_argparser->get_arg("--load_module"))) {
		debugf("Loading module: %s\n", kernel_module_path);
		elf::load_kernel_module(kernel_module_path, true);
	}

	if (global_argparser->is_arg("--load_modules")) {
		char* modules_folder = global_argparser->get_arg("--load_modules");

		int idx = 0;
		fs::vfs::dir_t dir = fs::vfs::global_vfs->dir_at(idx, modules_folder);
		while (!dir.is_none) {
			char module_path[256] = { 0 };
			strcpy(module_path, modules_folder);
			if (module_path[strlen(module_path) - 1] != '/') {
				strcat(module_path, "/");
			}

			strcat(module_path, dir.name);

			debugf("Loading module: %s\n", module_path);

			elf::load_kernel_module(module_path, true);

			dir = fs::vfs::global_vfs->dir_at(++idx, modules_folder);
		}
	}

	BOOT_PROGRESS(40);

	pci::enumerate_pci();

	BOOT_PROGRESS(45);

	// fs::vfs::file_t* test = fs::global_vfs->open("stivale:limine.cfg");

	// char buffer[512] = {0};
	// fs::global_vfs->read(test, buffer, test->size > 512 ? 512 : test->size, 0);

	// printf("%s", buffer);

	// elf::load_kernel_module("stivale:ps2_keyboard.o");

	elf::device_init_all();

	BOOT_PROGRESS(50);

	// driver::global_driver_manager->add_driver(new driver::device_driver());
	//init drivers here
	printf("\nLoading drivers...\n");
	driver::global_driver_manager->activate_all(false);

	BOOT_PROGRESS(60);

	printf("\nLoading network stack...\n");
	driver::load_network_stack();

	BOOT_PROGRESS(70);

	elf::fs_init_all();

	BOOT_PROGRESS(80);

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

	BOOT_PROGRESS(100);

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
