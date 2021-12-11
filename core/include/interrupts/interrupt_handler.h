#pragma once

#include <stdint.h>

#include <interrupts/interrupts.h>

typedef void (*intr_handler_ptr)(interrupts::s_registers*);

namespace interrupts {
	class interrupt_handler {
		protected:
			uint8_t int_num;
			interrupt_handler(uint8_t int_num);
			~interrupt_handler();
		public:
			virtual void handle();
	};

	extern interrupts::interrupt_handler* handlers[256];

	extern intr_handler_ptr static_handlers[256];

	void register_interrupt_handler(uint8_t intr, intr_handler_ptr handler);
}