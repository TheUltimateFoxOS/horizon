#include <syscall/syscall.h>

#include <fs/vfs.h>

using namespace syscall;

void syscall::sys_dir_at(interrupts::s_registers* regs) {
	char* path = (char*) regs->rbx;
	int idx = regs->rcx;

	fs::vfs::dir_t dir = fs::vfs::global_vfs->dir_at(idx, path);

	fs::vfs::dir_t* dir_ptr = (fs::vfs::dir_t*) regs->rdx;
	*dir_ptr = dir;
}