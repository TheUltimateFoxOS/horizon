#include <ps2_mouse.h>

#include <renderer/framebuffer.h>
#include <renderer/renderer.h>

#include <utils/log.h>

using namespace ps2;

ps2_mouse::ps2_mouse() : dataport(0x60), commandport(0x64), interrupts::interrupt_handler(0x2C) {
	
}

void ps2_mouse::activate() {
	commandport.Write(0xa8);
	mouse_wait();
	commandport.Write(0x20);
	mouse_wait_input();
	uint8_t status = dataport.Read();
	status |= 0b10;
	mouse_wait();
	commandport.Write(0x60);
	mouse_wait();
	dataport.Write(status);
	mouse_write(0xf6);
	mouse_read();
	mouse_write(0xf4);
	mouse_read();

	reset();

	input::default_mouse_input_device = this;
}

bool ps2_mouse::is_presend() {
	return true;
}

char* ps2_mouse::get_name() {
	return (char*) "ps2_mouse";
}

void ps2_mouse::handle() {
	uint8_t data = mouse_read();
	static bool skip = true;
	if (skip) {
		skip = false;
		return;
	}

	switch(mouse_cycle) {
		case 0:
			{
				if ((data & 0b00001000) == 0) {
					break;
				}
				mouse_packet[0] = data;
				mouse_cycle++;
			}
			break;
		case 1:
			{
				mouse_packet[1] = data;
				mouse_cycle++;
			}
			break;
		case 2:
			{
				mouse_packet[2] = data;
				mouse_packet_ready = true;
				mouse_cycle = 0;
			}
			break;
	}

	if (mouse_packet_ready) {
		bool x_negative, y_negative, x_overflow, y_overflow;
		int x = last_x;
		int y = last_y;

		if (mouse_packet[0] & PS2XSign) {
			x_negative = true;
		} else x_negative = false;

		if (mouse_packet[0] & PS2YSign) {
			y_negative = true;
		} else y_negative = false;

		if (mouse_packet[0] & PS2XOverflow) {
			x_overflow = true;
		} else x_overflow = false;

		if (mouse_packet[0] & PS2YOverflow) {
			y_overflow = true;
		} else y_overflow = false;

		if (!x_negative) {
			x += mouse_packet[1];
			if (x_overflow){
				x += 255;
			}
		} else {
			mouse_packet[1] = 256 - mouse_packet[1];
			x -= mouse_packet[1];
			if (x_overflow){
				x -= 255;
			}
		}

		if (!y_negative) {
			y -= mouse_packet[2];
			if (y_overflow){
				y -= 255;
			}
		} else {
			mouse_packet[2] = 256 - mouse_packet[2];
			y += mouse_packet[2];
			if (y_overflow){
				y += 255;
			}
		}

		bool left_button = mouse_packet[0] & 1;
		bool right_button = (mouse_packet[0] >> 1) & 1;
		bool middle_button = (mouse_packet[0] >> 2) & 1;

		if (x < 0) {
			x = 0;
		} else if (x > renderer::default_framebuffer.width - 1) {
			x = renderer::default_framebuffer.width - 1;
		}

		if (y < 0) {
			y = 0;
		} else if (y > renderer::default_framebuffer.height - 1) {
			y = renderer::default_framebuffer.height - 1;
		}

		update(x, y, left_button, right_button, middle_button);

		mouse_packet_ready = false;
	}
}

void ps2_mouse::mouse_wait() {
	uint32_t timeout = 1000;
	while (timeout--) {
		if ((commandport.Read() & 0b10) == 0) {
			return;
		}
	}

	debugf("ps2_mouse::mouse_wait() timed out\n");
}

void ps2_mouse::mouse_wait_input() {
	uint32_t timeout = 1000;
	while (timeout--){
		if (commandport.Read() & 0b1) {
			return;
		}
	}

	debugf("ps2_mouse::mouse_wait_input() timed out\n");
}

void ps2_mouse::mouse_write(unsigned char a_write) {
	mouse_wait();
	commandport.Write(0xD4);
	mouse_wait();
	dataport.Write(a_write);
}

unsigned char ps2_mouse::mouse_read() {
	mouse_wait_input();
	return dataport.Read();
}