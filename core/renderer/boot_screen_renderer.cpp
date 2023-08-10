#include <renderer/boot_screen_renderer.h>

#include <renderer/render2d.h>
#include <renderer/font_renderer.h>

#include <utils/log.h>
#include <utils/string.h>
#include <timer/pit.h>

using namespace renderer;
namespace renderer {
	uint8_t* boot_animation_frames[] = {
		boot_animation_frame_0,
		boot_animation_frame_1,
		boot_animation_frame_2,
		boot_animation_frame_3,
		boot_animation_frame_4,
		boot_animation_frame_5,
		boot_animation_frame_6,
		boot_animation_frame_7
	};
}

boot_screen_renderer::boot_screen_renderer(framebuffer_t* target_frame_buffer) : interrupt_handler(0x20) {
	no_render = true;
	this->target_frame_buffer = target_frame_buffer;
	this->current_frame = 0;
	this->divider = 0;

	frame_size = global_renderer_2d->get_bitmap_info(boot_animation_frames[0]);
	this->frame_x = target_frame_buffer->width / 2 - frame_size.x / 2;
	this->frame_y = target_frame_buffer->height / 2 - frame_size.y;

	this->text_y = frame_y + frame_size.y;

	debugf("frame_x: %d, frame_y: %d\n", frame_x, frame_y);
	debugf("frame_size.x: %d, frame_size.y: %d\n", frame_size.x, frame_size.y);

	log::stdout_device = this;
	global_font_renderer->clear(0x0);
	no_render = false;
}

extern uint8_t logo[];

boot_screen_renderer::~boot_screen_renderer() {
	no_render = true;
	log::stdout_device = renderer::global_font_renderer;
	global_font_renderer->clear(0x0);
	global_font_renderer->cursor_position = {0, 0};

	renderer::global_renderer_2d->load_bitmap(logo, 0);

	interrupts::handlers[0x20] = timer::global_pit_timer; // Restore timer handler
}

void boot_screen_renderer::putchar(char c) {
/*
	char str[2];
	str[0] = c;
	str[1] = '\0';
	putstring(str);
*/
}

void boot_screen_renderer::putstring(const char *str) {
/*
	char _str[strlen((char*) str) + 1];
	memset(_str, 0, strlen((char*) str) + 1);
	strcpy(_str, str);

	int len = strlen(_str);
	for (int i = 0; i < len; i++) {
		if (_str[i] == '\n') {
			_str[i] = ' ';
		}
	}

	debugf("[BOOT STATUS] %s\n", _str);

	if (this->target_frame_buffer->width / 2 - len * 8 < 0) {
		debugf("[BOOT STATUS] String too long!\n");
		return;
	}

	global_font_renderer->cursor_position = {0, text_y};
	global_font_renderer->clear_line();

	global_font_renderer->cursor_position = { this->target_frame_buffer->width / 2 - len * 8 / 2, text_y };
	global_font_renderer->putstring(_str);
*/
}

void boot_screen_renderer::set_progress(uint8_t progress) {
	if (progress > 100) {
		progress = 100;
	}


	int x_start = this->target_frame_buffer->width / 2 - 100;
	int x_end = this->target_frame_buffer->width / 2 + 100;

	int x_pos = x_start + progress * (x_end - x_start) / 100;
	
	int y_size = 10;

	for (int y = 0; y < y_size; y++) {
		for (int x = x_start; x < x_pos; x++) {
			global_renderer_2d->put_pix(x, y + this->frame_y + frame_size.y, 0xFFFFFF);
		}
	}

	for (int y = 0; y < y_size; y++) {
		for (int x = x_pos; x < x_end; x++) {
			global_renderer_2d->put_pix(x, y + this->frame_y + frame_size.y, 0x0);
		}
	}
}


void boot_screen_renderer::handle() {
	timer::global_pit_timer->handle(); // quick and dirty hack because we override the timer interrupt handler
	if (no_render) {
		return;
	}

	divider++;
	if (divider == timer::global_pit_timer->get_ticks_per_second() / 4) {
		divider = 0;
	} else {
		return;
	}

	this->current_frame++;
	if(this->current_frame >= 8) {
		this->current_frame = 0;
	}

	global_renderer_2d->load_bitmap(boot_animation_frames[this->current_frame], this->frame_y, this->frame_x);
}