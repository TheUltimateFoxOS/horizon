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

char input::keymap(char* keymap_id, uint8_t key, special_keys_down_t* special_keys_down) {
	if (strcmp(keymap_id, cached_keymap_id) != 0) {
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
	}

	bool shift = special_keys_down->left_shift || special_keys_down->right_shift;
	bool alt = special_keys_down->left_alt || special_keys_down->right_alt;

	if (special_keys_down->caps_lock) {
		if (shift) {
			return cached_keymap.layout_normal[key];
		} else {
			return cached_keymap.layout_shift[key];
		}
	} else if (shift) {
		return cached_keymap.layout_shift[key];
	} else if (alt) {
		return cached_keymap.layout_alt[key];
	} else {
		return cached_keymap.layout_normal[key];
	}
}
