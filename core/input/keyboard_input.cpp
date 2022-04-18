#include <input/keyboard_input.h>

#include <utils/log.h>
#include <utils/abort.h>
#include <utils/string.h>
#include <fs/vfs.h>

using namespace input;

namespace input {
	keyboard_input_device* default_keyboard_input_device = nullptr;
}

void keyboard_input_device::getstring(char* buf) {
	char c;
	while ((c = this->getchar()) != '\n') {
		*buf++ = c;
	}
}

char keyboard_input_device::getchar() {
	debugf("keyboard_input_device::getchar() not implemented");
	return 0;
}

keymap_t cached_keymap;
char cached_keymap_id[8] = { 0 };

namespace input {
	char keymap_load_path[256] = { 0 };
};

char input::keymap(char* keymap_id, uint8_t key, bool l_alt, bool r_alt, bool l_ctrl, bool r_ctrl, bool l_shift, bool r_shift, bool caps_lock) {
	if (strcmp(keymap_id, cached_keymap_id) == 0) {
		if (l_shift) {
			return cached_keymap.layout_shift[key];
		} else if (r_shift) {
			return cached_keymap.layout_shift[key];
		} else if (l_alt) {
			return cached_keymap.layout_alt[key];
		} else if (r_alt) {
			return cached_keymap.layout_alt[key];
		} else {
			return cached_keymap.layout_normal[key];
		}
	} else {
		char path[256] = { 0 };
		strcat(path, keymap_load_path);
		strcat(path, keymap_id);
		strcat(path, ".fmp");

		fs::vfs::file_t* file = fs::vfs::global_vfs->open(path);
		if (file == nullptr) {
			debugf("input::keymap: keymap %s not found\n", keymap_id);
			return 0;
		}

		keymap_t keymap_;
		
		fs::vfs::global_vfs->read(file, &keymap_, sizeof(keymap_t), 0);
		fs::vfs::global_vfs->close(file);

		cached_keymap = keymap_;
		strcpy(cached_keymap_id, keymap_id);

		if (l_shift) {
			return keymap_.layout_shift[key];
		} else if (r_shift) {
			return keymap_.layout_shift[key];
		} else if (l_alt) {
			return keymap_.layout_alt[key];
		} else if (r_alt) {
			return keymap_.layout_alt[key];
		} else {
			return keymap_.layout_normal[key];
		}
	}
}
