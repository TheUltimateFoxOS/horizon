use cstr_core::c_char;
use cstr_core::CString;
use core::fmt;
use lazy_static::lazy_static;
use spin::Mutex;

extern {
	fn __rust_print(s: *const c_char);
	fn __rust_debug(s: *const c_char);
	fn __rust_abort(s: *const c_char) -> !;
}

pub fn print_raw(s: &str) {
	unsafe {
		__rust_print(CString::new(s).unwrap().as_ptr());
	}
}

pub fn debug_raw(s: &str) {
	unsafe {
		__rust_debug(CString::new(s).unwrap().as_ptr());
	}
}

pub fn abort_raw(s: &str) -> ! {
	unsafe {
		__rust_abort(CString::new(s).unwrap().as_ptr());
	}
}


pub enum WriterType {
	DISPLAY, DEBUG
}

pub struct Writer {
	writer_type: WriterType
}

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
		match self.writer_type {
			WriterType::DISPLAY => {
				print_raw(s);
			}
			WriterType::DEBUG => {
				debug_raw(s);
			}
		}
        Ok(())
    }
}
impl Writer {
	pub fn new(writer_type: WriterType) -> Self {
		Self {
			writer_type
		}
	}
}

lazy_static! {
    pub static ref DISPLAY_WRITER: Mutex<Writer> = Mutex::new(Writer::new(WriterType::DISPLAY));
    pub static ref DEBUG_WRITER: Mutex<Writer> = Mutex::new(Writer::new(WriterType::DEBUG));
}

pub fn _print(args: fmt::Arguments, writer: &Mutex<Writer>) {
    use core::fmt::Write;
    writer.lock().write_fmt(args).unwrap();
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => ($crate::io::_print(format_args!($($arg)*), &$crate::io::DISPLAY_WRITER));
}

#[macro_export]
macro_rules! println {
    () => ($crate::print!("\n"));
    ($($arg:tt)*) => ($crate::print!("{}}\n", format_args!($($arg)*)));
}

#[macro_export]
macro_rules! debug {
    ($($arg:tt)*) => ($crate::io::_print(format_args!($($arg)*), &$crate::io::DEBUG_WRITER));
}

#[macro_export]
macro_rules! debugln {
    () => ($crate::debug!("\n"));
    ($($arg:tt)*) => ($crate::debug!("[{}:{}] {}\n", file!(), line!(), format_args!($($arg)*)));
}