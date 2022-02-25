#include <syscall/syscall.h>

#include <utils/log.h>
#include <utils/lock.h>
#include <utils/abort.h>
#include <fs/fd.h>
#include <scheduler/scheduler.h>
#include <apic/apic.h>

#include <renderer/font_renderer.h>

using namespace syscall;

define_spinlock(sys_write_lock);

void syscall::sys_write(interrupts::s_registers* regs) {
	LAPIC_ID(core_id);

	scheduler::task_t* task = scheduler::task_queue[core_id]->list[0];

	switch (regs->rbx) {
		case 0:
			break;

		case 1:
			{
				if (task->stdout_pipe == nullptr || !task->pipe_enabled) {
					atomic_acquire_spinlock(sys_write_lock);
					// Write to stdout
					for (int i = 0; i < regs->rdx; i++) {
						printf("%c", *((char*)regs->rcx + i));
					}
					atomic_release_spinlock(sys_write_lock);
				} else {
					task->stdout_pipe((char*)regs->rcx, regs->rdx);
				}
			}
			break;

		case 2:
			{
				if (task->stderr_pipe == nullptr || !task->pipe_enabled) {	
					atomic_acquire_spinlock(sys_write_lock);
					// Write to stderr
					renderer::global_font_renderer->set_color(0xffff0000);
					for (int i = 0; i < regs->rdx; i++) {
						printf("%c", *((char*)regs->rcx + i));
					}
					renderer::global_font_renderer->reset_color();
					atomic_release_spinlock(sys_write_lock);
				} else {
					task->stderr_pipe((char*)regs->rcx, regs->rdx);
				}
			}
			break;
		
		default:
			{
				// Write to file from fd
				fs::file_descriptor* fd = fs::global_fd_manager->get_fd(regs->rbx);
				if (fd == nullptr) {
					regs->rax = -1;
					return;
				}

				fd->write((void*) regs->rcx, regs->rdx, regs->rsi);
			}
			break;
	}
}
