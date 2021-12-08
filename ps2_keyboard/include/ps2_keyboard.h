#pragma once

#include <driver/driver.h>
#include <input/input.h>
#include <interrupts/interrupt_handler.h>
#include <utils/port.h>

class ps2_keyboard : public driver::device_driver, public input::input_device, public interrupts::interrupt_handler {
	public:
		ps2_keyboard();

		virtual void activate();
		virtual bool is_presend();
		virtual char* get_name();

		virtual char getchar();

		virtual void handle();

		char current_char;
	
	private:
		Port8Bit dataport;
		Port8Bit commandport;
};