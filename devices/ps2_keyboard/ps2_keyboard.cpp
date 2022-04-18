#include <ps2_keyboard.h>

#include <utils/log.h>
#include <stdint.h>

using namespace ps2;

ps2_keyboard::ps2_keyboard() : interrupts::interrupt_handler(0x21), dataport(0x60), commandport(0x64) {
	memcpy(keyboard_layout, "us", 3);
	print_char = true;
}

void ps2_keyboard::activate() {
	while(commandport.Read() & 0x1) {
		dataport.Read();
	}
	commandport.Write(0xae);
	commandport.Write(0x20);
	uint8_t status = (dataport.Read() | 1) & ~0x10;
	commandport.Write(0x60);
	dataport.Write(status);
	dataport.Write(0xf4);

	input::default_keyboard_input_device = this;
}

bool ps2_keyboard::is_presend() {
	return true;
}

char* ps2_keyboard::get_name() {
	return (char*) "ps2_keyboard";
}

char ps2_keyboard::getchar() {
	current_char = 0;

	while(!current_char) {
		__asm__ __volatile__("hlt");
	}

	return current_char;
}

void ps2_keyboard::special_key_down(special_key key) {

}

void ps2_keyboard::special_key_up(special_key key) {

}

void ps2_keyboard::handle() {
	uint8_t key = dataport.Read();

	if (this->keyboard_debug) {
		debugf("KEY EVENT: %x\n", key);
	}
	
	if (this->special_next) {
		switch (this->special_code) {
			case 0xE0:
				switch (key) {
					case 0x38: //Right alt down
						this->r_alt = true;
						special_key_down(special_key::right_alt);
						break;
					case 0xB8: //Right alt up
						this->r_alt = false;
						special_key_up(special_key::right_alt);
						break;

					case 0x1D: //Right ctrl down
						this->r_ctrl = true;
						special_key_down(special_key::right_ctrl);
						break;
					case 0x9D: //Right ctrl up
						this->r_ctrl = false;
						special_key_up(special_key::right_ctrl);
						break;
				}
				break;

			case 0x3A:
				switch (key) {
					case 0xBA: //Caps lock toggle
						this->caps_lock = !this->caps_lock;
						if (this->caps_lock) {
							special_key_down(special_key::caps_lock);
						} else {
							special_key_up(special_key::caps_lock);
						}
						break;
				}
				break;
		}

		this->special_next = false;
	} else {
		switch (key) {
			case 0xE0:
			case 0x3A:
				this->special_next = true;
				this->special_code = key;
				break;
			
			case 0x38: //Left alt down
				this->r_alt = true;
				special_key_down(special_key::left_alt);
				break;
			case 0xB8: //Left alt up
				this->r_alt = false;
				special_key_up(special_key::left_alt);
				break;

			case 0x1D: //Left ctrl down
				this->l_ctrl = true;
				special_key_down(special_key::left_ctrl);
				break;
			
			case 0x9D: //Left ctrl up
				this->l_ctrl = false;
				special_key_up(special_key::left_ctrl);
				break;

			case 0x2A: //Left shift up
				this->l_shift = true;
				special_key_down(special_key::left_shift);
				break;

			case 0xAA: //Left shift up
				this->l_shift = false;
				special_key_up(special_key::left_shift);
				break;

			case 0x36: //Right shift down
				this->r_shift = true;
				special_key_down(special_key::right_shift);
				break; 
			case 0xB6: //Right shift up
				this->r_shift = false;
				special_key_up(special_key::right_shift);
				break;

			default:
				char tmp = input::keymap(keyboard_layout, key, l_alt, r_alt, l_ctrl, r_ctrl, l_shift, r_shift, caps_lock);
				// if in printable range
				if ((tmp >= 0x20 && tmp <= 0x7E || tmp == '\n'|| tmp == '\b') && print_char) {
					printf("%c", tmp);
				}

				current_char = tmp;
		}
	}

}

void ps2_keyboard::write(fs::vfs::file_t* file, void* buffer, size_t size, size_t offset) {
	char* buf = (char*) buffer;

	switch (buf[0]) {
		case 1: // opcode change layout
			{
				strcpy(keyboard_layout, buf + 1);
				
				debugf("Keyboard layout changed to %s\n", keyboard_layout);
			}
			break;
		
		case 2: // opcode set debug mode
			{
				uint8_t mode = buf[1];
				this->keyboard_debug = mode;
				debugf("Keyboard debug mode changed to %s\n", mode ? "true" : "false");
			}
			break;
		
		case 3: // opcode set print char
			{
				uint8_t mode = buf[1];
				this->print_char = mode;
				debugf("Keyboard print char changed to %s\n", mode ? "true" : "false");
			}
			break;
		
		default:
			debugf("ps2_keyboard::read: unknown opcode %d", buf[0]);
			break;
	}
}

void ps2_keyboard::read(fs::vfs::file_t* file, void* buffer, size_t size, size_t offset) {
	debugf("ps2_keyboard::read not implemented");
}