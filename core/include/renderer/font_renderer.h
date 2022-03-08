#pragma once

#include <output/output.h>

#include <renderer/framebuffer.h>
#include <renderer/font.h>
#include <renderer/point.h>

#include <fs/dev_fs.h>

namespace renderer {
	class font_renderer : public output::output_device, public fs::dev_fs_file {
		public:
			virtual void putstring(const char *str);
			virtual void putchar(char c);

			void clear(uint32_t c_color);
			void clear_line();
			void set_color(uint32_t new_color);
			void reset_color();

			font_renderer(framebuffer_t* target_frame_buffer, psf1_font_t* font);

			virtual void write(fs::file_t* file, void* buffer, size_t size, size_t offset);
			virtual char* get_name();

			renderer::point_t cursor_position;
			framebuffer_t* target_frame_buffer;
			psf1_font_t* font;
			uint32_t color;
			uint32_t old_color;
	};

	extern font_renderer* global_font_renderer;
}