#include <renderer/font_renderer.h>

#include <renderer/render2d.h>

#include <utils/string.h>

using namespace renderer;

namespace renderer {
	font_renderer* global_font_renderer;
}

font_renderer::font_renderer(framebuffer_t* target_frame_buffer, psf1_font_t* font) {
	this->target_frame_buffer = target_frame_buffer;
	this->font = font;

	color = 0xffffffff;
	old_color = 0xffffffff;
	cursor_position = {0, 0};
}

void font_renderer::putchar(char c) {

	if(c == 0) {
		return;
	}

	if(c == '\b') {
		uint32_t* pix_ptr = (uint32_t*) target_frame_buffer->base_address;
		for (unsigned long y = cursor_position.y; y < cursor_position.y + 16; y++){
			for (unsigned long x = cursor_position.x; x < cursor_position.x + 8; x++){
				*(unsigned int*)(pix_ptr + x + (y * target_frame_buffer->width)) = 0;
			}
		}

		if (cursor_position.x - 16 < 0) {
			cursor_position.x = target_frame_buffer->width - 8;
			if (cursor_position.y - 16 < 0) {
				cursor_position.y = 0;
			} else {
				cursor_position.y -= 16;
			}
		} else {
			cursor_position.x -= 8;
		}
		return;
	}

	if(cursor_position.x + 16 > target_frame_buffer->width || c == '\n') {
		cursor_position.x = 0;
		cursor_position.y += 16;
	} else {
		cursor_position.x += 8;
	}

	if (cursor_position.y + 16 > target_frame_buffer->height) {
		global_renderer_2d->next_line();
		cursor_position.y -= 16;
	}

	if(c == '\n') {
		return;
	}

	if (c >= 20 && c <= 126) {

		uint32_t* pix_ptr = (uint32_t*) target_frame_buffer->base_address;
		char* font_ptr = (char*) font->glyph_buffer + (c * font->psf1_Header->charsize);

		for (unsigned long y = cursor_position.y; y < cursor_position.y + 16; y++){
			for (unsigned long x = cursor_position.x; x < cursor_position.x + 8; x++){
				if ((*font_ptr & (0b10000000 >> (x - cursor_position.x))) > 0){
					*(unsigned int*)(pix_ptr + x + (y * target_frame_buffer->width)) = color;
				}
			}
			font_ptr++;
		}
	}
}

void font_renderer::putstring(const char *str) {
	while (*str) {
		putchar(*str++);
	}
}

void font_renderer::clear() {
	uint64_t base = (uint64_t) target_frame_buffer->base_address;
	uint64_t bytes_per_scanline = target_frame_buffer->width * 4;
	uint64_t fb_height = target_frame_buffer->height;
	uint64_t fb_size = target_frame_buffer->buffer_size;

	for (int vertical_scanline = 0; vertical_scanline < fb_height; vertical_scanline ++){
		uint64_t pix_ptr_base = base + (bytes_per_scanline * vertical_scanline);
		for (uint32_t* pixPtr = (uint32_t*)pix_ptr_base; pixPtr < (uint32_t*)(pix_ptr_base + bytes_per_scanline); pixPtr ++){
			*pixPtr = color;
		}
	}
}

void font_renderer::clear_line() {
	memset((void*) ((uint64_t) target_frame_buffer->base_address + cursor_position.y * target_frame_buffer->width * 4), 0, target_frame_buffer->width * 16 * 4);
	cursor_position.x = 0;
}

void font_renderer::set_color(uint32_t new_color) {
	old_color = color;
	color = new_color;
}

void font_renderer::reset_color() {
	color = old_color;
}