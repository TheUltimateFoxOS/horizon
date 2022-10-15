extern {
	fn __rust_get_default_framebuffer() -> Framebuffer;
}

#[repr(C)]
pub struct Framebuffer {
	pub base_address: u64,
	pub buffer_size: u64,
	pub width: u32,
	pub height: u32
}

pub fn get_default_framebuffer() -> Framebuffer {
	unsafe {
		__rust_get_default_framebuffer()
	}
}