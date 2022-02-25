#pragma once

#include <stdint.h>

namespace ps2 {
	enum keymap_layout {
		keymap_de_e,
		keymap_us_e,
		keymap_fr_e
	};

	char keymap_de(uint8_t key, bool l_alt, bool r_alt, bool l_ctrl, bool r_ctrl, bool l_shift, bool r_shift, bool caps_lock);
	char keymap_us(uint8_t key, bool l_alt, bool r_alt, bool l_ctrl, bool r_ctrl, bool l_shift, bool r_shift, bool caps_lock);
	char keymap_fr(uint8_t key, bool l_alt, bool r_alt, bool l_ctrl, bool r_ctrl, bool l_shift, bool r_shift, bool caps_lock);

	char keymap(keymap_layout keymap_id, uint8_t key, bool l_alt, bool r_alt, bool l_ctrl, bool r_ctrl, bool l_shift, bool r_shift, bool caps_lock);

}