#include <ps2_keyboard.h>

#include <ps2_layout.h>
#include <utils/log.h>
#include <stdint.h>

using namespace ps2;

ps2_keyboard::ps2_keyboard() : interrupts::interrupt_handler(0x21), dataport(0x60), commandport(0x64) {}

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

	input::default_input_device = this;
}

bool ps2_keyboard::is_presend() {
	return true;
}

char* ps2_keyboard::get_name() {
	return (char*) "ps2 keyboard";
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
	
	switch (key)
	{
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
		current_char = keymap_de(key, l_shift, r_shift, caps_lock);
		printf("%c", current_char);
		break;
	}

}