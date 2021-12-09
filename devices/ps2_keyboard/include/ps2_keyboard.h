#pragma once

#include <driver/driver.h>
#include <input/input.h>
#include <interrupts/interrupt_handler.h>
#include <utils/port.h>

namespace ps2 {
	enum special_key {
			left_shift,
			left_ctrl,
			left_alt,
			left_gui,

			right_shift,
			right_control,
			right_alt,
			right_gui,
			
			caps_lock,
			num_lock,
			scroll_lock,

			up_arrow,
			down_arrow,
			left_arrow,
			right_arrow,
	};

	class ps2_keyboard : public driver::device_driver, public input::input_device, public interrupts::interrupt_handler {
		public:
			ps2_keyboard();

			virtual void activate();
			virtual bool is_presend();
			virtual char* get_name();

			virtual char getchar();

			virtual void special_key_down(special_key key);
			virtual void special_key_up(special_key key);

			virtual void handle();

			char current_char;
		
		private:
			bool l_shift = false;
			bool r_shift = false;
			bool caps_lock = false;
			Port8Bit dataport;
			Port8Bit commandport;
	};
}