#include <interrupts/interrupt_handler.h>
#include <stdint.h>

extern "C" {
	struct rust_interrupt_handler_data_t {
		void (*handle)(rust_interrupt_handler_data_t* d);
		uint64_t handler;
	};

	class rust_interrupt_handler_t : public interrupts::interrupt_handler {
		public:
			rust_interrupt_handler_t(int intr) : interrupt_handler(intr) {
				data = nullptr;
			}

			rust_interrupt_handler_data_t* data;

			void handle() {
				if (!data) {
					return;
				}

				this->data->handle(this->data);
			}
	};

	uint64_t __rust_interrupt_handler_alloc(uint8_t intr) {
		return (uint64_t) new rust_interrupt_handler_t(intr);
	}

	void __rust_interrupt_handler_register(rust_interrupt_handler_data_t* data) {
		((rust_interrupt_handler_t*) data->handler)->data = data;
	}
}