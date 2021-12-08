#include <utils/abort.h>

#include <utils/log.h>
#include <utils/vsprintf.h>
#include <utils/string.h>
#include <renderer/font_renderer.h>
#include <renderer/render2d.h>
#include <elf/unwind.h>
#include <elf/elf_resolver.h>

#include <stdint.h>

extern uint8_t screen_of_death[];

__attribute__((noreturn)) void abortf(const char* fmt, ...) {
	uint64_t rbp;
	__asm__ __volatile__ ("movq %%rbp, %0" : "=r" (rbp));
	renderer::point_t bmp_info = renderer::global_renderer_2d->get_bitmap_info(screen_of_death);

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
	printf("https://github.com/TheUltimateFoxOS/FoxOS");
	renderer::global_font_renderer->reset_color();
	printf(" by creating an issue.\n");
	printf("Feel free to fix this and submit a pull request!\n\n");
	printf("\nStarting stack trace:\n");

	int max_lines = (renderer::global_renderer_2d->target->height - renderer::global_font_renderer->cursor_position.y) / 16;
	max_lines -= 4;

	debugf("Starting stack trace using %d as max lines!\n", max_lines);

	elf::unwind(max_lines, rbp, [](int frame_num, uint64_t rip) {
		if(elf::resolve_symbol(elf::resolve_symbol(rip)) != 0) {
			char str[512] = {0};
			sprintf(str, "%s + %d", elf::resolve_symbol(rip), rip - elf::resolve_symbol(elf::resolve_symbol(rip)));
			printf("%s\n", str);
		} else {
			printf("<unknown function at 0x%x>\n", rip);
		}
		// printf("%d: %p\n", frame_num, rip);
	});

	renderer::global_renderer_2d->load_bitmap(screen_of_death, renderer::global_renderer_2d->target->height - bmp_info.x, 0);

	while (true) {
		__asm__ __volatile__ ("cli; hlt");
	}
}