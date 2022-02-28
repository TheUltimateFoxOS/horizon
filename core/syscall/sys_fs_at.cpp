#include <syscall/syscall.h>

#include <fs/vfs.h>

using namespace syscall;

void syscall::sys_fs_at(interrupts::s_registers* regs) {
	char* path = (char*) regs->rbx;
	int idx = regs->rcx;

	bool success = fs::vfs::global_vfs->fs_at(idx, path);

	regs->rax = success;
}