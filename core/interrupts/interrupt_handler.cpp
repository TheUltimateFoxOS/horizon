#include <interrupts/interrupt_handler.h>
#include <stddef.h>

using namespace interrupts;
interrupt_handler* interrupts::handlers[256];
intr_handler_ptr interrupts::static_handlers[256];

//#interrupt_handler::interrupt_handler-doc: Registers the interrupt handler for the given interrupt number. This is called by is the constructor.
interrupt_handler::interrupt_handler(uint8_t int_num) {
	this->int_num = int_num;
	handlers[this->int_num] = this;
}

//#interrupt_handler::~interrupt_handler-doc: Unregisters the interrupt handler for the given interrupt number. This is a destructor no need to call it manually.
interrupt_handler::~interrupt_handler() {
	if(handlers[this->int_num] == this) {
		handlers[this->int_num] = NULL;
	}
}

//#interrupt_handler::handle-doc: Default handler for interrupts. Gets called if the default handler doesn't get overwritten.
void interrupt_handler::handle() {

}

//#interrupts::register_interrupt_handler-doc: Register an interrupt handler for the given interrupt number. Uses the static_handlers array to store the handler.
void interrupts::register_interrupt_handler(uint8_t intr, intr_handler_ptr handler) {
	static_handlers[intr] = handler;
}