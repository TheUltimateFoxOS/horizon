#include <syscall/syscall.h>

#include <input/input.h>
#include <utils/lock.h>
#include <utils/abort.h>

using namespace syscall;

define_spinlock(sys_read_lock);

void syscall::sys_read(interrupts::s_registers* regs) {
	switch (regs->rbx) {
		case 0:
			{
				// Read from stdin
				if (input::default_input_device == nullptr) {
					regs->rax = -1;
					return;
				}

				atomic_acquire_spinlock(sys_read_lock);
				for (int i = 0; i < regs->rdx - 1; i++) {
					__asm__ __volatile__ ("sti");
					*((char*)regs->rcx + i) = input::default_input_device->getchar();
					__asm__ __volatile__ ("cli");
				}
				
				*((char*)regs->rcx + regs->rdx) = '\0';

				atomic_release_spinlock(sys_read_lock);
			}
			break;

		case 1:
			break;

		case 2:
			break;
		
		default:
			{
				// Read from file from fd
				abortf("sys_read: reading from file using fd not implemented");
			}
			break;
	}
}
