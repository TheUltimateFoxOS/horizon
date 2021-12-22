#include <renderer/window_renderer.h>

using namespace renderer;

namespace renderer {
	window_renderer* global_window_renderer;
}

window_t::window_t(int x, int y, int width, int height, framebuffer_t* target_frame_buffer) {
	this->window_x = x;
	this->window_y = y;
	this->window_width = width;
	this->window_height = height;

	this->buffer_x = x + 1;
	this->buffer_y = y + (WINDOW_HEADER_HEIGHT + 1);
	this->buffer_width = width - 2;
	this->buffer_height = height - (WINDOW_HEADER_HEIGHT + 1);

	this->target = target_frame_buffer;
}

int window_t::get_x() {
	return this->window_x;
}
int window_t::get_y() {
	return this->window_y;
}

int window_t::get_height() {
	return this->window_height;
}
int window_t::get_width() {
	return this->window_width;
}

void window_t::set_background_color(uint32_t colour) {
	this->background_color = colour;
}
uint32_t window_t::get_background_color() {
	return this->background_color;
}

void window_t::put_window_pix(int x, int y, uint32_t colour) {
	if (x >= 0 && x + this->window_x <= this->window_x + this->window_width && y >= 0 && y + this->window_y <= this->window_y + this->window_height) {
		global_renderer_2d->put_pix(x + this->window_x, y + this->window_y, colour);
	}
}
void window_t::put_pix(int x, int y, uint32_t colour) {
	if (x >= 0 && x + this->buffer_x <= this->buffer_x + this->buffer_width && y >= 0 && y + this->buffer_y <= this->buffer_y + this->buffer_height) {
		global_renderer_2d->put_pix(x + this->buffer_x, y + this->buffer_y, colour);
	}
}

void window_t::draw() {
	for (int x = 0; x <= this->window_width; x++) {
		for (int y = 0; y < WINDOW_HEADER_HEIGHT; y++) {
			put_window_pix(x, y, 0xFFFFFFFF);
		}
		for (int y = WINDOW_HEADER_HEIGHT; y <= this->window_height; y++) {
			put_window_pix(x, y, this->background_color);
		}
	}

	for (int x = 0; x <= this->window_height; x++) {
		put_window_pix(0, x, WINDOW_BORDER_COLOR);
		put_window_pix(this->window_width, x, WINDOW_BORDER_COLOR);
	}
	for (int y = 0; y <= this->window_width; y++) {
		put_window_pix(y, 0, WINDOW_BORDER_COLOR);
		put_window_pix(y, WINDOW_HEADER_HEIGHT, WINDOW_BORDER_COLOR);
		put_window_pix(y, this->window_height, WINDOW_BORDER_COLOR);
	}
}

void resize(int width, int height) {
	//TODO
}

void move(int x, int y) {
	//TODO
}


window_renderer::window_renderer(framebuffer_t* target_frame_buffer) {
	this->target = target_frame_buffer;
}

window_t window_renderer::create_window(int x, int y, int width, int height) {
	return window_t(x, y, width, height, this->target);
}
