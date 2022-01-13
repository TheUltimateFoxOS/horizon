#pragma once

#include <renderer/framebuffer.h>
#include <renderer/point.h>
#include <renderer/render2d.h>

#define WINDOW_HEADER_HEIGHT 25
#define WINDOW_BORDER_COLOR 0xFFFFFFFF

namespace renderer {
	class window_t {
		public:
			window_t(int x, int y, int width, int height, framebuffer_t* target_frame_buffer);

			int get_x();
			int get_y();
			int get_height();
			int get_width();

			void set_background_color(uint32_t colour);
			uint32_t get_background_color();

			void draw();
			void put_pix(int x, int y, uint32_t colour);

			void resize(int width, int height);
			void move(int x, int y);

		private:
			framebuffer_t* target;

			int window_x;
			int window_y;
			int window_width;
			int window_height;

			int buffer_x;
			int buffer_y;
			int buffer_width;
			int buffer_height;

			uint32_t background_color = 0xFFFFFFFF;

			void put_window_pix(int x, int y, uint32_t colour);
	};

	class window_renderer {
		public:
			window_renderer(framebuffer_t* target_frame_buffer);

			window_t create_window(int x, int y, int width, int height);

		private:
			framebuffer_t* target;
	};

	extern window_renderer* global_window_renderer;
}