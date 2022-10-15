extern {
	fn __rust_mid_reset();
	fn __rust_mid_update(x: u64, y: u64, left_button: bool, right_button: bool, middle_button: bool);
}

pub fn reset() {
	unsafe {
		__rust_mid_reset();
	}
}

pub fn update(x: u64, y: u64, left_button: bool, right_button: bool, middle_button: bool) {
	unsafe {
		__rust_mid_update(x, y, left_button, right_button, middle_button);
	}
}