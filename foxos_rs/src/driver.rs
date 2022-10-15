use cstr_core::c_char;
use cstr_core::CString;

pub type DriverActivate = extern fn(d: *mut RustDriver);
pub type DriverIsPresent = extern fn(d: *mut RustDriver) -> bool;
pub type DriverGetName = extern fn(d: *mut RustDriver) -> *const c_char;

extern {
	fn __rust_driver_alloc() -> u64;
	fn __rust_driver_register(d: *const RustDriver);
}

#[repr(C)]
pub struct RustDriver {
    pub activate: DriverActivate,
    pub is_present: DriverIsPresent,
	pub get_name: DriverGetName,
	pub cpp_driver: u64,
	pub functions: *mut dyn DriverFunctions,
}

pub trait DriverFunctions  {
	fn activate(&mut self);
	fn is_present(&mut self) -> bool;
	fn get_name(&mut self) -> *const c_char;
}

extern fn activate(d: *mut RustDriver) {
	let functions = unsafe {
		&mut *(&mut *d).functions
	};

	functions.activate()
}

extern fn is_present(d: *mut RustDriver) -> bool {
	let functions = unsafe {
		&mut *(&mut *d).functions
	};

	functions.is_present()
}

extern fn get_name(d: *mut RustDriver) -> *const c_char {
	let functions = unsafe {
		&mut *(&mut *d).functions
	};

	functions.get_name()
}

impl RustDriver {
	pub fn new(functions: *mut dyn DriverFunctions) -> Self {
		let cpp_driver = unsafe {
			__rust_driver_alloc()
		};

		Self {
			activate,
			is_present,
			get_name,
			cpp_driver,
			functions
		}
	}

	pub fn register(&self) {
		unsafe {
			__rust_driver_register(self);
		}
	}
}

unsafe impl Send for RustDriver {}
unsafe impl Sync for RustDriver {}

impl Drop for RustDriver {
    fn drop(&mut self) {
		panic!("RustDriver should not be dropped!");
    }
}