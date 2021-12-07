#pragma once

namespace output {
	class output_device {
		virtual void putstring(const char *str);
		virtual void putchar(char c);
	};
}