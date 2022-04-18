#pragma once

#include <driver/driver.h>
#include <interrupts/interrupt_handler.h>
#include <utils/port.h>
#include <input/mouse_input.h>

namespace ps2 {
	#define PS2XSign 0b00010000
	#define PS2YSign 0b00100000
	#define PS2XOverflow 0b01000000
	#define PS2YOverflow 0b10000000

	class ps2_mouse : public driver::device_driver, public interrupts::interrupt_handler, public input::mouse_input_device {
		public:
			ps2_mouse();
			virtual void activate();
			virtual bool is_presend();
			virtual char* get_name();

			virtual void handle();
		
		private:
			Port8Bit dataport;
			Port8Bit commandport;

			void mouse_wait();
			void mouse_wait_input();
			void mouse_write(unsigned char a_write);
			unsigned char mouse_read();

			uint8_t mouse_cycle = 0;
			uint8_t mouse_packet[4];
			bool mouse_packet_ready = false;
	};
}