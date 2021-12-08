#include <input/input.h>

#include <utils/log.h>

using namespace input;

namespace input {
	input_device* default_input_device = nullptr;
}

void input_device::getstring(char* buf) {
	char c;
	while ((c = this->getchar()) != '\n') {
		*buf++ = c;
	}
}

char input_device::getchar() {
	debugf("input_device::getchar() not implemented");
	return 0;
}