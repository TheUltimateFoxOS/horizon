#pragma once

#include <output/output.h>
#include <stivale2.h>

namespace output {
	class stivale2_terminal : public output::output_device {
		public:
			stivale2_terminal();

			virtual void putstring(const char *str);
			virtual void putchar(char c);

		private:
			stivale2_struct_tag_terminal* terminal_tag;
	};

	extern stivale2_terminal* global_terminal;
}
