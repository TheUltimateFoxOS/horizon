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

void ps2_keyboard::special_key_down(input::special_key key) {
	special_keys_down.triggered_by = key;
	input::handle_special_keys_down(&special_keys_down);
}

void ps2_keyboard::special_key_up(input::special_key key) {
	special_keys_down.triggered_by = key;
	input::handle_special_keys_up(&special_keys_down);
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
						this->special_keys_down.right_alt = true;
						special_key_down(input::special_key::right_alt);
						break;
					case 0xB8: //Right alt up
						this->special_keys_down.right_alt = false;
						special_key_up(input::special_key::right_alt);
						break;

					case 0x1D: //Right ctrl down
						this->special_keys_down.right_ctrl = true;
						special_key_down(input::special_key::right_ctrl);
						break;
					case 0x9D: //Right ctrl up
						this->special_keys_down.right_ctrl = false;
						special_key_up(input::special_key::right_ctrl);
						break;

					case 0x48: //Up arrow down
						this->special_keys_down.up_arrow = true;
						special_key_down(input::special_key::up_arrow);
						break;
					case 0xC8: //Up arrow up
						this->special_keys_down.up_arrow = false;
						special_key_up(input::special_key::up_arrow);
						break;
					
					case 0x50: //Down arrow down
						this->special_keys_down.down_arrow = true;
						special_key_down(input::special_key::down_arrow);
						break;
					case 0xD0: //Down arrow up
						this->special_keys_down.down_arrow = false;
						special_key_up(input::special_key::down_arrow);
						break;

					case 0x4B: //Left arrow down
						this->special_keys_down.left_arrow = true;
						special_key_down(input::special_key::left_arrow);
						break;
					case 0xCB: //Left arrow down
						this->special_keys_down.left_arrow = false;
						special_key_up(input::special_key::left_arrow);
						break;

					case 0x4D: //Right arrow down
						this->special_keys_down.right_arrow = true;
						special_key_down(input::special_key::right_arrow);
						break;
					case 0xCD: //Right arrow up
						this->special_keys_down.right_arrow = false;
						special_key_up(input::special_key::right_arrow);
						break;
				}
				break;

			case 0x3A:
				switch (key) {
					case 0xBA: //Caps lock toggle
						this->special_keys_down.caps_lock = !this->special_keys_down.caps_lock;
						if (this->special_keys_down.caps_lock) {
							special_key_down(input::special_key::caps_lock);
						} else {
							special_key_up(input::special_key::caps_lock);
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
				this->special_keys_down.left_alt = true;
				special_key_down(input::special_key::left_alt);
				break;
			case 0xB8: //Left alt up
				this->special_keys_down.left_alt = false;
				special_key_up(input::special_key::left_alt);
				break;

			case 0x1D: //Left ctrl down
				this->special_keys_down.left_ctrl = true;
				special_key_down(input::special_key::left_ctrl);
				break;
			case 0x9D: //Left ctrl up
				this->special_keys_down.left_ctrl = false;
				special_key_up(input::special_key::left_ctrl);
				break;

			case 0x2A: //Left shift down
				this->special_keys_down.left_shift = true;
				special_key_down(input::special_key::left_shift);
				break;

			case 0xAA: //Left shift up
				this->special_keys_down.left_shift = false;
				special_key_up(input::special_key::left_shift);
				break;

			case 0x36: //Right shift down
				this->special_keys_down.right_shift = true;
				special_key_down(input::special_key::right_shift);
				break; 
			case 0xB6: //Right shift up
				this->special_keys_down.right_shift = false;
				special_key_up(input::special_key::right_shift);
				break;

			default:
				char tmp = input::keymap(keyboard_layout, key, &special_keys_down);
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
				if (this->lock_keyboard_print) {
					debugf("Keyboard print char didn't change as it is locked\n");
				} else {
					uint8_t mode = buf[1];
					this->print_char = mode;
					debugf("Keyboard print char changed to %s\n", mode ? "true" : "false");
				}
			}
			break;

		case 4: //opcode lock print char
			{
				uint8_t mode = buf[1];
				this->lock_keyboard_print = mode;
				debugf("Keyboard lock print char changed to %s\n", mode ? "true" : "false");
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
