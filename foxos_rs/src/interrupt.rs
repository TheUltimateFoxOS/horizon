pub type InterruptHandler = extern fn(d: *mut RustInterruptHandler);

extern {
	fn __rust_interrupt_handler_alloc(n: u8) -> u64;
	fn __rust_interrupt_handler_register(d: *const RustInterruptHandler);
}

#[repr(C)]
pub struct RustInterruptHandler {
    pub handle: InterruptHandler,
	pub cpp_interrupt_handler: u64,
	pub functions: *mut dyn InterruptFunctions,
}

pub trait InterruptFunctions  {
	fn handle(&mut self);
}

extern fn handle(d: *mut RustInterruptHandler) {
	let functions = unsafe {
		&mut *(&mut *d).functions
	};

	functions.handle()
}

impl RustInterruptHandler {
	pub fn new(intr: u8, functions: *mut dyn InterruptFunctions) -> Self {
		let cpp_interrupt_handler = unsafe {
			__rust_interrupt_handler_alloc(intr)
		};

		Self {
			handle,
			cpp_interrupt_handler,
			functions
		}
	}

	pub fn register(&self) {
		unsafe {
			__rust_interrupt_handler_register(self);
		}
	}
}

unsafe impl Send for RustInterruptHandler {}
unsafe impl Sync for RustInterruptHandler {}

impl Drop for RustInterruptHandler {
    fn drop(&mut self) {
		panic!("RustInterruptHandler should not be dropped!");
    }
}