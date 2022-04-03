#include <syscall/syscall.h>

#include <fs/vfs.h>
#include <utils/log.h>

using namespace syscall;

void syscall::sys_delete_dir(interrupts::s_registers* regs) {
	char* path = (char*) regs->rbx;

	debugf("Deleting directory: %s\n", path);
	fs::vfs::global_vfs->delete_dir(path);
}
