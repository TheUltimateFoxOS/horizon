#include <renderer/font_renderer.h>

#include <renderer/render2d.h>

#include <utils/string.h>
#include <utils/log.h>

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
		char* font_ptr = (char*) font->glyph_buffer + (c * font->header->charsize);

		for (unsigned long y = cursor_position.y; y < cursor_position.y + 16; y++){
			for (unsigned long x = cursor_position.x; x < cursor_position.x + 8; x++){
				if ((*font_ptr & (0b10000000 >> (x - cursor_position.x))) > 0){
					*(unsigned int*)(pix_ptr + x + (y * (target_frame_buffer->pitch / 4))) = color;
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

void font_renderer::clear(uint32_t c_color) {
	uint64_t base = (uint64_t) target_frame_buffer->base_address;
	uint64_t bytes_per_scanline = target_frame_buffer->width * 4;
	uint64_t fb_height = target_frame_buffer->height;
	uint64_t fb_size = target_frame_buffer->buffer_size;

	for (int vertical_scanline = 0; vertical_scanline < fb_height; vertical_scanline ++){
		uint64_t pix_ptr_base = base + (bytes_per_scanline * vertical_scanline);
		for (uint32_t* pixPtr = (uint32_t*)pix_ptr_base; pixPtr < (uint32_t*)(pix_ptr_base + bytes_per_scanline); pixPtr ++){
			*pixPtr = c_color;
		}
	}
}

void font_renderer::clear_line() {
	// memset((void*) ((uint64_t) target_frame_buffer->base_address + cursor_position.y * target_frame_buffer->width * 4), 0, target_frame_buffer->width * 16 * 4);
	uint32_t* pix_ptr = (uint32_t*) target_frame_buffer->base_address;
	for (unsigned long y = cursor_position.y; y < cursor_position.y + 16; y++){
		for (unsigned long x = cursor_position.x; x < cursor_position.x + target_frame_buffer->width; x++){
			*(unsigned int*)(pix_ptr + x + (y * target_frame_buffer->width)) = color;
		}
	}
	cursor_position.x = 0;
}

void font_renderer::set_color(uint32_t new_color) {
	old_color = color;
	color = new_color;
}

void font_renderer::reset_color() {
	color = old_color;
}

char* font_renderer::get_name() {
	return (char*) "font_renderer";
}

void font_renderer::write(fs::file_t* file, void* buffer, size_t size, size_t offset) {
	char* buf = (char*) buffer;

	switch (buf[0]) {
		case 0: // opcode set color
			{
				uint32_t new_color = *((uint32_t*) &buf[1]);
				set_color(new_color);
			}
			break;
		
		case 1: // opcode get color
			{
				uint32_t* color_ptr = (uint32_t*) &buf[1];
				*color_ptr = color;
			}
			break;
		
		case 2: // opcode set cursor
			{
				uint64_t x = *((uint64_t*) &buf[1]);
				uint64_t y = *((uint64_t*) &buf[9]);
				
				cursor_position.x = x;
				cursor_position.y = y;
			}
			break;

		case 3: // opcode get cursor
			{
				uint64_t x = cursor_position.x;
				uint64_t y = cursor_position.y;
				
				uint64_t* x_ptr = (uint64_t*) &buf[1];
				uint64_t* y_ptr = (uint64_t*) &buf[9];

				*x_ptr = x;
				*y_ptr = y;
			}
			break;
		
		case 4: // opcode get screen size
			{
				uint64_t width = target_frame_buffer->width;
				uint64_t height = target_frame_buffer->height;

				uint64_t* width_ptr = (uint64_t*) &buf[1];
				uint64_t* height_ptr = (uint64_t*) &buf[9];

				*width_ptr = width;
				*height_ptr = height;
			}
			break;

		case 5: // opcode clear screen
			{
				clear(color);
			}
			break;
		
		case 6: // opcode clear line
			{
				clear_line();
			}
			break;
		
		default:
			{
				debugf("unknown opcode %d\n", buf[0]);
			}
			break;
	}
}