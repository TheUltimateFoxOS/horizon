#include <syscall/syscall.h>

#include <fs/fd.h>

using namespace syscall;

void syscall::sys_delete(interrupts::s_registers* regs) {
	fs::file_descriptor* fd = fs::global_fd_manager->get_fd(regs->rbx);
	fd->delete_();
	
	delete fd;

	fs::global_fd_manager->free_fd(regs->rbx);
}
