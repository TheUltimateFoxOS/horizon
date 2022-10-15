
use foxos_rs::debugln;
use foxos_rs::driver::DriverFunctions;
use foxos_rs::interrupt::InterruptFunctions;
use foxos_rs::port::{inb, outb};
use foxos_rs::framebuffer::get_default_framebuffer;
use foxos_rs::mouse_input_device::{update, reset};

use cstr_core::CString;
use cstr_core::c_char;
use lazy_static::lazy_static;

lazy_static! {
	pub static ref PS2_MOUSE_NAME: CString = CString::new("ps2 mouse").unwrap();
}
static DATA_PORT: u16 = 0x60;
static COMMAND_PORT: u16 = 0x64;

static PS2X_SIGN: u8 = 0b00010000;
static PS2Y_SIGN: u8 = 0b00100000;
static PS2X_OVERFLOW: u8 = 0b01000000;
static PS2Y_OVERFLOW: u8 = 0b10000000;


pub struct Ps2MouseDriver {
	mouse_cycle: u8,
	mouse_packet: [u8; 4],
	mouse_packet_ready: bool,
	last_x: u64,
	last_y: u64
}
impl DriverFunctions for Ps2MouseDriver {
	fn activate(&mut self) {
		outb(COMMAND_PORT, 0xa8);
		self.mouse_wait();
		outb(COMMAND_PORT, 0x20);
		self.mouse_wait_input();
		let mut status = inb(DATA_PORT);
		status |= 0b10;
		self.mouse_wait();
		outb(COMMAND_PORT, 0x60);
		self.mouse_wait();
		outb(DATA_PORT, status);
		self.mouse_write(0xf6);
		self.mouse_read();
		self.mouse_write(0xf4);
		self.mouse_read();

		reset();
	}

	fn is_present(&mut self) -> bool {
		true
	}

	fn get_name(&mut self) -> *const c_char {
		PS2_MOUSE_NAME.as_ptr()
	}
}
impl InterruptFunctions for Ps2MouseDriver {
	fn handle(&mut self) {
		let data = self.mouse_read();

		match self.mouse_cycle {
			0 => {
				if (data & 0b00001000) != 0 {
					self.mouse_packet[0] = data;
					self.mouse_cycle += 1;
				}
			}

			1 => {
				self.mouse_packet[1] = data;
				self.mouse_cycle += 1;
			}

			2 => {
				self.mouse_packet[2] = data;
				self.mouse_cycle = 0;
				self.mouse_packet_ready = true;
			}
			_ => {
				debugln!("unexpected value ({}) for mouse_cycle. resetting...", self.mouse_cycle);
				self.mouse_cycle = 0;
			}
		};

		if self.mouse_packet_ready {
			let mut x_negative = false;
			let mut y_negative = false;
			let mut x_overflow = false;
			let mut y_overflow = false;
			let mut x = self.last_x;
			let mut y = self.last_y;

			if (self.mouse_packet[0] & PS2X_SIGN) != 0 {
				x_negative = true;
			}

			if (self.mouse_packet[0] & PS2Y_SIGN) != 0 {
				y_negative = true;
			}

			if (self.mouse_packet[0] & PS2X_OVERFLOW) != 0 {
				x_overflow = true;
			}

			if (self.mouse_packet[0] & PS2Y_OVERFLOW) != 0 {
				y_overflow = true;
			}

			if !x_negative {
				x += self.mouse_packet[1] as u64;
				if x_overflow {
					x += 255;
				}
			} else {
				self.mouse_packet[1] = (256 - self.mouse_packet[1] as u64) as u8;
				x -= self.mouse_packet[1] as u64;
				if x_overflow {
					x -= 255;
				}
			}
	
			if !y_negative {
				y -= self.mouse_packet[2] as u64;
				if y_overflow {
					y -= 255;
				}
			} else {
				self.mouse_packet[2] = (256 - self.mouse_packet[2] as u64) as u8;
				y += self.mouse_packet[2] as u64;
				if y_overflow {
					y += 255;
				}
			}

			let fb = get_default_framebuffer();

			if y > (fb.height - 1u32) as u64 {
				y = (fb.height - 1) as u64;
			}

			if x > (fb.width - 1u32) as u64 {
				x = (fb.width - 1) as u64;
			}

			let left_button = (self.mouse_packet[0] & 1) != 0;
			let right_button = ((self.mouse_packet[0] >> 1) & 1) != 0;
			let middle_button = ((self.mouse_packet[0] >> 2) & 1) != 0;

			update(x, y, left_button, right_button, middle_button);

			self.last_x = x;
			self.last_y = y;
			self.mouse_packet_ready = false;
		}
	}
}
impl Ps2MouseDriver {
	pub fn new() -> Self {
		Self {
			mouse_cycle: 0,
			mouse_packet: [ 0, 0, 0, 0 ],
			mouse_packet_ready: false,
			last_x: 0,
			last_y: 0
		}
	}

	fn mouse_wait(&self) {
		let mut timeout = 1000;
		while timeout != 0 {
			if (inb(COMMAND_PORT) & 0b10) == 0 {
				return;
			}
			timeout -= 1;
		}

		debugln!("Ps2MouseDriver::mouse_wait() timed out!");
	}

	fn mouse_wait_input(&self) {
		let mut timeout = 1000;
		while timeout != 0 {
			if (inb(COMMAND_PORT) & 0b1) != 0 {
				return;
			}
			timeout -= 1;
		}

		debugln!("Ps2MouseDriver::mouse_wait_input() timed out!");
	}

	fn mouse_write(&self, d: u8) {
		self.mouse_wait();
		outb(COMMAND_PORT, 0xD4);
		self.mouse_wait();
		outb(DATA_PORT, d);
	}

	fn mouse_read(&self) -> u8 {
		self.mouse_wait_input();
		inb(DATA_PORT)
	}
}