#pragma once

#include <stdint.h>
#include <output/output.h>
#include <utils/port.h>

namespace output {
	#define COM1 0x3f8

	class serial_port : public output::output_device {
		public:
			serial_port(uint16_t port_n);

			virtual void putchar(char c);
			virtual void putstring(const char* str);

		private:
			Port8Bit port;

			int serial_recieved();
			int is_transmit_empty();
	};

	extern serial_port* global_serial_port;
}