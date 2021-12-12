#include <syscall/syscall.h>

#include <fs/fd.h>


void syscall::sys_open(interrupts::s_registers* regs) {
	int fd = fs::global_fd_manager->alloc_fd();

	fs::file_descriptor* fd_obj = new fs::file_descriptor(fd);
	fs::global_fd_manager->register_fd(fd_obj);

	fd_obj->open((char*) regs->rbx);

	if (fd_obj->file == nullptr) {
		delete fd_obj;
		fs::global_fd_manager->free_fd(fd);

		regs->rdx = -1;
		return;
	}

	regs->rdx = fd;
}