#pragma once

#include <renderer/framebuffer.h>
#include <renderer/point.h>

namespace renderer {
	class render2d {
		public:
			render2d(framebuffer_t* target_frame_buffer);

			framebuffer_t* target;

			void put_pix(uint32_t x, uint32_t y, uint32_t colour);

			void load_bitmap(uint8_t data[], int y);
			void load_bitmap(uint8_t data[], int x, int y);
			renderer::point_t get_bitmap_info(uint8_t data[]);
			
			void next_line();
	};

	extern render2d* global_renderer_2d;
}