#include <syscall/syscall.h>

#include <fs/fd.h>


void syscall::sys_open(interrupts::s_registers* regs) {
	int fd = fs::global_fd_manager->alloc_fd();

	fs::file_descriptor* fd_obj = new fs::file_descriptor(fd);
	fs::global_fd_manager->register_fd(fd_obj);

	fd_obj->open((char*) regs->rbx);

	regs->rdx = fd;
}