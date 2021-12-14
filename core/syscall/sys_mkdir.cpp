#include <syscall/syscall.h>

#include <fs/vfs.h>

using namespace syscall;

void syscall::sys_mkdir(interrupts::s_registers* regs) {
	char* path = (char*) regs->rbx;

	fs::vfs::global_vfs->mkdir(path);
}