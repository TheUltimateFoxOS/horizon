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

	char keymap(char* keymap_id, uint8_t key, bool l_alt, bool r_alt, bool l_ctrl, bool r_ctrl, bool l_shift, bool r_shift, bool caps_lock);

	extern keyboard_input_device* default_keyboard_input_device;
}