#pragma once

#include <stdint.h>

namespace input {
	typedef struct keymap_t {
		char layout_normal[0xff];
		char layout_shift[0xff];
		char layout_alt[0xff];
	} keymap_t;

	extern char keymap_load_path[256];

	class keyboard_input_device {
		public:
			virtual void getstring(char* buf);
			virtual char getchar();
	};

	enum special_key {
		left_shift,
		left_ctrl,
		left_alt,
		left_gui,

		right_shift,
		right_ctrl,
		right_alt,
		right_gui,
			
		caps_lock,
		num_lock,
		scroll_lock,

		up_arrow,
		down_arrow,
		left_arrow,
		right_arrow,
	};

	struct special_keys_down_t {
		bool left_shift = false;
		bool left_ctrl = false;
		bool left_alt = false;
		bool left_gui = false;

		bool right_shift = false;
		bool right_ctrl = false;
		bool right_alt = false;
		bool right_gui = false;

		bool caps_lock = false;
		bool num_lock = false;
		bool scroll_lock = false;

		bool up_arrow = false;
		bool down_arrow = false;
		bool left_arrow = false;
		bool right_arrow = false;

		special_key triggered_by;
	};

	extern input::special_keys_down_t global_special_keys_down;

	char keymap(char* keymap_id, uint8_t key, special_keys_down_t* special_keys_down);

	void handle_special_keys_up(input::special_keys_down_t* keys);
	void handle_special_keys_down(input::special_keys_down_t* keys);

	extern keyboard_input_device* default_keyboard_input_device;
}
