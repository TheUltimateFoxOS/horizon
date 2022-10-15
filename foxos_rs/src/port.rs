extern {
	fn __rust_io_wait();

	fn __rust_inb(port: u16) -> u8;
	fn __rust_outb(port: u16, data: u8);
	fn __rust_inw(port: u16) -> u16;
	fn __rust_outw(port: u16, data: u16);
	fn __rust_inl(port: u16) -> u32;
	fn __rust_outl(port: u16, data: u32); 
}

pub fn io_wait() {
	unsafe {
		__rust_io_wait()
	}
}

pub fn inb(port: u16) -> u8 {
	unsafe {
		__rust_inb(port)
	}
}

pub fn outb(port: u16, data: u8) {
	unsafe {
		__rust_outb(port, data)
	}
}

pub fn inw(port: u16) -> u16 {
	unsafe {
		__rust_inw(port)
	}
}

pub fn outw(port: u16, data: u16) {
	unsafe {
		__rust_outw(port, data)
	}
}

pub fn inl(port: u16) -> u32 {
	unsafe {
		__rust_inl(port)
	}
}

pub fn outl(port: u16, data: u32) {
	unsafe {
		__rust_outl(port, data)
	}
}