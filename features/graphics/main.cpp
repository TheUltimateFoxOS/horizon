#include <elf/kernel_module.h>
#include <graphic_syscalls.h>
#include <utils/log.h>

void init() {
	debugf("Registering graphics syscalls\n");
	syscall::register_syscall(syscall::sys_copy_to_fb, "sys_copy_to_fb");
	syscall::register_syscall(syscall::sys_copy_from_fb, "sys_copy_from_fb");
	syscall::register_syscall(syscall::sys_fb_info, "sys_fb_info");
}


define_module("graphics", init, null_ptr_func, null_ptr_func);