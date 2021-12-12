#include <syscall/syscall.h>

#include <fs/fd.h>

using namespace syscall;

void syscall::sys_get_file_size(interrupts::s_registers* regs) {
	fs::file_descriptor* fd = fs::global_fd_manager->get_fd(regs->rbx);
	if (fd == nullptr) {
		regs->rax = -1;
		return;
	}

	regs->rax = fd->file->size;
}