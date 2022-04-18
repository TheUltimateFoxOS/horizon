#pragma once

#include <stdint.h>

namespace input {
	class mouse_input_device {
		public:
			int last_x = 0;
			int last_y = 0;

			bool button_left;
			bool button_right;
			bool button_middle;

			void update(int x, int y, bool left, bool right, bool middle);
			void reset();
	};

	extern mouse_input_device* default_mouse_input_device;
}