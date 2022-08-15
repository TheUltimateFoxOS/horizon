#include <utils/abort.h>

#include <utils/log.h>
#include <utils/vsprintf.h>
#include <utils/string.h>
#include <renderer/font_renderer.h>
#include <renderer/render2d.h>
#include <elf/unwind.h>
#include <elf/elf_resolver.h>

#include <apic/apic.h>
#include <acpi/madt.h>

#include <scheduler/scheduler.h>

#include <stdint.h>

extern uint8_t screen_of_death[];

uint64_t program_start;
uint64_t program_end;

__attribute__((noreturn))
void abortf(const char* fmt, ...) {
	__asm__ volatile("cli");

	LAPIC_ID(core_id);
	for (int i = 0; i < acpi::madt::lapic_count; i++) {
		if (apic::cpu_started[i] && i != core_id) {
			debugf("Sending halt to CPU %d\n", i);
			apic::lapic_ipi(i, 0xff);
		}
	}

	uint64_t rbp = ([]() {
		uint64_t rbp;
		__asm__ __volatile__("movq %%rbp, %0" : "=r"(rbp));
		return rbp;
	})();

	log::stdout_device = renderer::global_font_renderer;

	va_list args;
	char buf[1024] = {0};

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

	renderer::global_font_renderer->clear(0xffe36d2d);
	renderer::global_font_renderer->cursor_position = { 0, 0 };

	printf("(/ o_o)/ Oh no! Something terrible has happened and your system has been halted...\n");
	printf("There isn't much you can do apart from restart the computer. More information below.\n\n");

	printf("Kernel PANIC -> ");
	printf(buf);
	printf("\n\n");

	printf("Please report this issue at ");
	renderer::global_font_renderer->set_color(0xff0000ff);
	printf("https://github.com/TheUltimateFoxOS/horizon");
	renderer::global_font_renderer->reset_color();
	printf(" by creating an issue.\n");
	printf("Feel free to fix this and submit a pull request!\n\n");

	if (scheduler::task_queue[core_id]->list[0]->argv[0]) {
		printf("Current task: %s\n", scheduler::task_queue[core_id]->list[0]->argv[0]);
	}

	program_start = (uint64_t) scheduler::task_queue[core_id]->list[0]->offset;
	program_end = (uint64_t) scheduler::task_queue[core_id]->list[0]->offset + scheduler::task_queue[core_id]->list[0]->page_count * 4096;

	printf("\nStarting stack trace:\n");

	int max_lines = (renderer::global_renderer_2d->target->height - renderer::global_font_renderer->cursor_position.y) / 16;
	max_lines -= 4;

	debugf("Starting stack trace using %d as max lines!\n", max_lines);

	//#elf::unwind-discard
	elf::unwind(max_lines, rbp, [](int frame_num, uint64_t rip) {
		if(elf::resolve_symbol(elf::resolve_symbol(rip)) != 0) {
			char str[512] = {0};
			sprintf(str, "%s + %d", elf::resolve_symbol(rip), rip - elf::resolve_symbol(elf::resolve_symbol(rip)));
			printf("%s\n", str);
		} else {
			if (rip >= program_start && rip < program_end) {
				printf("<unknown function at 0x%x (0x%x)>\n", rip, rip - program_start);
			} else {
				printf("<unknown function at 0x%x>\n", rip);
			}
		}
	});

	renderer::point_t bmp_info = renderer::global_renderer_2d->get_bitmap_info(screen_of_death);
	renderer::global_renderer_2d->load_bitmap(screen_of_death, renderer::global_renderer_2d->target->height - bmp_info.x - 3, 0);

	while (true) {
		__asm__ __volatile__ ("cli; hlt");
	}
}