#include <syscall/syscall.h>

#include <input/input.h>
#include <utils/lock.h>
#include <utils/abort.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>
#include <apic/apic.h>

using namespace syscall;

define_spinlock(sys_read_lock);

void syscall::sys_read(interrupts::s_registers* regs) {
	LAPIC_ID(core_id);

	scheduler::task_t* task = scheduler::task_queue[core_id]->list[0];

	switch (regs->rbx) {
		case 0:
			{
				if (task->stdin_pipe == nullptr || !task->pipe_enabled) {
					// Read from stdin
					if (input::default_input_device == nullptr) {
						regs->rax = -1;
						return;
					}

					atomic_acquire_spinlock(sys_read_lock);
					for (int i = 0; i < regs->rdx; i++) {
						__asm__ __volatile__ ("sti");
						*((char*)regs->rcx + i) = input::default_input_device->getchar();
						__asm__ __volatile__ ("cli");
					}
					
					//*((char*)regs->rcx + regs->rdx) = '\0';

					atomic_release_spinlock(sys_read_lock);
				} else {
					task->stdin_pipe((char*)regs->rcx, regs->rdx);
				}
			}
			break;

		case 1:
			break;

		case 2:
			break;
		
		default:
			{
				// Read from file from fd
				fs::file_descriptor* fd = fs::global_fd_manager->get_fd(regs->rbx);
				if (fd == nullptr) {
					regs->rax = -1;
					return;
				}

				fd->read((void*) regs->rcx, regs->rdx, regs->rsi);
			}
			break;
	}
}
