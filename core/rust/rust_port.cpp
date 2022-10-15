#include <utils/port.h>

extern "C" {
	void __rust_io_wait() {
		io_wait();
	}

	uint8_t __rust_inb(uint16_t _port) {
		return inb(_port);
	}

	void __rust_outb(uint16_t _port, uint8_t _data) {
		outb(_port, _data);
	}

	uint16_t __rust_inw(uint16_t _port) {
		return inw(_port);
	}

	void __rust_outw(uint16_t _port, uint16_t _data) {
		outw(_port, _data);
	}

	uint32_t __rust_inl(uint16_t port) {
		return inl(port);
	}

	void __rust_outl(uint16_t port, uint32_t data) {
		outl(port, data);
	}
}