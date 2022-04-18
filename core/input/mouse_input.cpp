#include <input/mouse_input.h>

#include <utils/log.h>

namespace input {
	mouse_input_device* default_mouse_input_device = nullptr;
}

using namespace input;

void mouse_input_device::update(int x, int y, bool left, bool right, bool middle) {
	last_x = x;
	last_y = y;
	button_left = left;
	button_right = right;
	button_middle = middle;

	// debugf("New mouse input: x: %d, y: %d %s%s%s\n", x, y, left ? "left button" : "", right ? "right button" : "", middle ? "middle button" : "");
}

void mouse_input_device::reset() {
	last_x = 0;
	last_y = 0;
	button_left = false;
	button_right = false;
	button_middle = false;
}