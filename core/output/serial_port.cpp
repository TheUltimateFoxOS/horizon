#include <output/serial_port.h>

using namespace output;

serial_port::serial_port(uint16_t port_n) : port(port_n) {
	port.offset = 1;
	port.Write(0x00);
	port.offset = 3;
	port.Write(0x80);
	port.offset = 0;
	port.Write(0x03);
	port.offset = 1;
	port.Write(0x00);
	port.offset = 3;
	port.Write(0x03);
	port.offset = 2;
	port.Write(0xc7);
	port.offset = 4;
	port.Write(0x0b);
}

int serial_port::is_transmit_empty() {
	port.offset = 5;
	return port.Read() & 0x20;
}

void serial_port::putchar(char c) {
	while(is_transmit_empty() == 0) {
		__asm__ __volatile__("pause" ::: "memory");
	}

	port.offset = 0;
	port.Write(c);

	if (c == '\n') {
		putchar('\r');
	}
}

void serial_port::putstring(const char* str) {
	while(*str) {
		putchar(*str++);
	}
}


namespace output {
	serial_port* global_serial_port;
}