#include <renderer/render2d.h>

#include <renderer/font_renderer.h>

#include <utils/string.h>

using namespace renderer;

namespace renderer {
	render2d* global_renderer_2d;
}

render2d::render2d(framebuffer_t* target_framebuffer) {
	target = target_framebuffer;
}

void render2d::put_pix(uint32_t x, uint32_t y, uint32_t colour) {
	*(uint32_t*)((uint64_t)target->base_address + (x * 4) + (y * 4 * target->width)) = colour;
}

void render2d::load_bitmap(uint8_t data[], int y) {
	uint8_t info[54];
	int _i = 54;
	int _y = 0;
	uint8_t* read_buff = data;
	
	while(_i > 0) {
		uint8_t g = *read_buff;
		info[_y] = g;
		read_buff++;
		_y++;
		_i--;
	}

	int data_offset = *(int*)&info[10]; 
	int src_width = *(int*)&info[18];
	int src_height = *(int*)&info[22];
	int width = src_width * src_height / src_height;
	int bit_count = (*(short*)&info[28]) / 8;

	int lx = (target->width - src_width) / 2;
	int ly = y;

	int location = (lx + ly * target->width) * 4;

	uint8_t* logo_data = data;
	logo_data += data_offset;

	for (int i = src_height; 0 < i; i--) {
		for(int j = 0; j < width; j++) {
			int where = (j + (i * target->width)) * 4 + location;     
			for (int c = 2; 0 <= c; c--) {
				uint8_t g = logo_data[((j * src_width) / width + (((src_height - i) * src_height) / src_height) * src_width) * bit_count + c];
				uint8_t* screen = (uint8_t*) target->base_address;
				screen[where + c] = g;
			}
		}
	}

	renderer::global_font_renderer->cursor_position = { 0, src_height + 16 };
}

void render2d::load_bitmap(uint8_t data[], int y, int x) {
	uint8_t info[54];
	int _i = 54;
	int _y = 0;
	uint8_t* read_buff = data;

	while(_i > 0) {
		uint8_t g = *read_buff;
		info[_y] = g;
		read_buff++;
		_y++;
		_i--;
	}

	int data_offset = *(int*)&info[10]; 
	int src_width = *(int*)&info[18];
	int src_height = *(int*)&info[22];
	int width = src_width * src_height / src_height;
	int bit_count = (*(short*)&info[28]) / 8;

	int lx = x;
	int ly = y;

	int location = (lx + ly * target->width) * 4;

	uint8_t* logo_data = data;
	logo_data += data_offset;

	for (int i = src_height; 0 < i; i--) {
		for(int j = 0; j < width; j++) {
			int where = (j + (i * target->width)) * 4 + location;     
			for (int c = 2; 0 <= c; c--) {
				uint8_t g = logo_data[((j * src_width) / width + (((src_height - i) * src_height) / src_height) * src_width) * bit_count + c];
				uint8_t* screen = (uint8_t*) target->base_address;
				screen[where + c] = g;
			}
		}
	}

	renderer::global_font_renderer->cursor_position = { 0, src_height + 16 };
}

renderer::point_t render2d::get_bitmap_info(uint8_t data[]) {
	uint8_t info[54];
	int _i = 54;
	int _y = 0;
	uint8_t* read_buff = data;
	
	while(_i > 0) {
		uint8_t g = *read_buff;
		info[_y] = g;
		read_buff++;
		_y++;
		_i--;
	}
	
	int src_width = *(int*)&info[18];
	int src_height = *(int*)&info[22];

	return { src_height, src_width };
}

void render2d::next_line() {
	memcpy((void*) (uint64_t)this->target->base_address, (void*)((uint64_t)this->target->base_address + (16 * this->target->width * 4)), (this->target->width * 4 * (this->target->height - 16)));
	memset((void*) ((uint64_t)this->target->base_address + ((this->target->width * 4) * (this->target->height - 16))), 0, (this->target->width * 4 * 16));
	memset((void*) ((uint64_t)this->target->base_address + ((this->target->width * 4) * (this->target->height - 16))), 0, (this->target->width * 4 * 16));
}