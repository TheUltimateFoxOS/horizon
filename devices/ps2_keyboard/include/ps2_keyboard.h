#pragma once

#include <driver/driver.h>

#include <input/keyboard_input.h>

#include <interrupts/interrupt_handler.h>

#include <utils/port.h>

#include <fs/dev_fs.h>

namespace ps2 {

	class ps2_keyboard : public driver::device_driver, public input::keyboard_input_device, public interrupts::interrupt_handler, public fs::dev_fs_file {
		public:
			ps2_keyboard();

			virtual void activate();
			virtual bool is_presend();
			virtual char* get_name();

			virtual char getchar();

			virtual void special_key_down(input::special_key key);
			virtual void special_key_up(input::special_key key);

			virtual void handle();

			char current_char;

			char keyboard_layout[0xff];
			bool keyboard_debug = false;
			bool lock_keyboard_print = false;

			virtual void read(fs::vfs::file_t* file, void* buffer, size_t size, size_t offset);
			virtual void write(fs::vfs::file_t* file, void* buffer, size_t size, size_t offset);
		
		private:
			bool special_next = false;
			uint8_t special_code = 0x00;

			input::special_keys_down_t special_keys_down;

			bool print_char = true;

			Port8Bit dataport;
			Port8Bit commandport;
	};
}