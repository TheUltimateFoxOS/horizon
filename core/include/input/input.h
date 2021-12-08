#pragma once

namespace input {
	class input_device {
		public:
			virtual void getstring(char* buf);
			virtual char getchar();
	};

	extern input_device* default_input_device;
}