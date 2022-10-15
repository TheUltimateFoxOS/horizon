use core::panic::PanicInfo;
use crate::io::abort_raw;
use crate::debugln;

#[panic_handler]
fn panic(info: &PanicInfo<'_>) -> ! {
	debugln!("Rust panic: {}", info);
	abort_raw("Rust panic! More information is in the serial log.");
}