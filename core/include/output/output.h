#pragma once

namespace output {
	class output_device {
		public:
			virtual void putstring(const char *str);
			virtual void putchar(char c);
	};
}