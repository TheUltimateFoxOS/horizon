#pragma once

#include <stdint.h>
#include <output/output.h>

#include <renderer/framebuffer.h>
#include <interrupts/interrupt_handler.h>
#include <renderer/point.h>

namespace renderer {
	extern "C" uint8_t boot_animation_frame_0[];
	extern "C" uint8_t boot_animation_frame_1[];
	extern "C" uint8_t boot_animation_frame_2[];
	extern "C" uint8_t boot_animation_frame_3[];
	extern "C" uint8_t boot_animation_frame_4[];
	extern "C" uint8_t boot_animation_frame_5[];
	extern "C" uint8_t boot_animation_frame_6[];
	extern "C" uint8_t boot_animation_frame_7[];

	extern uint8_t* boot_animation_frames[];

	class boot_screen_renderer : public output::output_device, public interrupts::interrupt_handler {
		public:
			virtual void putchar(char c);
			virtual void putstring(const char *str);

			boot_screen_renderer(framebuffer_t* target_frame_buffer);
			~boot_screen_renderer();

			void set_progress(uint8_t progress);

			int current_frame;
			int frame_x;
			int frame_y;
			int text_y;
			int divider;
			point_t frame_size;

			bool no_render;

			framebuffer_t* target_frame_buffer;

			virtual void handle();
	};
}