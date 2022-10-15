#![no_std]

pub mod ps2_mouse;
use ps2_mouse::Ps2MouseDriver;
use  lazy_static::lazy_static;
use foxos_rs::driver::RustDriver;
use foxos_rs::interrupt::RustInterruptHandler;

pub struct PtrStore<T>(*mut T);
impl<T> PtrStore<T> {
	pub fn new(t: *mut T) -> Self {
		Self {
			0: t
		}
	}

	pub fn ptr(&self) -> *mut T {
		self.0
	}
}
unsafe impl<T> Send for PtrStore<T> {}
unsafe impl<T> Sync for PtrStore<T> {}

lazy_static! {
	pub static ref PS2_MOUSE: PtrStore<Ps2MouseDriver> = PtrStore::new(&mut Ps2MouseDriver::new() as *mut Ps2MouseDriver);

	pub static ref PS2_MOUSE_DRIVER: RustDriver = RustDriver::new(PS2_MOUSE.ptr());
	pub static ref PS2_MOUSE_INTERRUPT: RustInterruptHandler = RustInterruptHandler::new(0x2C, PS2_MOUSE.ptr());
}

#[no_mangle]
extern "C" fn init() {}

#[no_mangle]
extern "C" fn device_init() {
	PS2_MOUSE_DRIVER.register();
	PS2_MOUSE_INTERRUPT.register();
}