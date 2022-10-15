#include <input/mouse_input.h>

extern "C" {
	void __rust_mid_reset() {
		if (input::default_mouse_input_device == nullptr) {
			input::default_mouse_input_device = new input::mouse_input_device();
		}

		input::default_mouse_input_device->reset();
	}

	void __rust_mid_update(int x, int y, bool left, bool right, bool middle) {
		if (input::default_mouse_input_device == nullptr) {
			input::default_mouse_input_device = new input::mouse_input_device();
		}

		input::default_mouse_input_device->update(x, y, left, right, middle);
	}
}