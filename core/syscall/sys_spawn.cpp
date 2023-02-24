#include <syscall/syscall.h>

#include <scheduler/scheduler.h>
#include <elf/elf_loader.h>
#include <fexec/fexec_loader.h>

#include <utils/log.h>

#include <apic/apic.h>

using namespace syscall;

void syscall::sys_spawn(interrupts::s_registers* regs) {
    LAPIC_ID(core_id);
    scheduler::task_t* self = scheduler::task_queue[core_id]->list[0];

    char* name = (char*) regs->rbx;
    const char** argv = (const char**) regs->rcx;
    const char** envp = (const char**) regs->rdx;

    scheduler::executable_type_t type = (scheduler::executable_type_t) regs->rdi;

    scheduler::task_t* task;
    switch (type) {
        case scheduler::ELF_EXECUTABLE:
            task = elf::load_elf(name, argv, envp);
            break;
        case scheduler::FEXEC_EXECUTABLE:
            task = fexec::load_fexec(name, argv, envp);
            break;
        default:
            task = nullptr;
            break;
    }

    if (task == nullptr) {
        debugf("Failed to load elf: %s\n", name);
    } else {
        task->lock = true;
        task->type = type;
        if ((bool) regs->rsi) {
            // clone cwd
            char* self_cwd = (char*) scheduler::get_cwd_self();
            memset(task->cwd, 0, sizeof(task->cwd));
            strcpy(task->cwd, self_cwd);
        }

        if (self->stdin_pipe) {
            debugf("Cloning stdin pipe\n");
            task->stdin_pipe = self->stdin_pipe;
        }

        if (self->stdout_pipe) {
            debugf("Cloning stdout pipe\n");
            task->stdout_pipe = self->stdout_pipe;
        }

        if (self->stderr_pipe) {
            debugf("Cloning stderr pipe\n");
            task->stderr_pipe = self->stderr_pipe;
        }

        if (self->pipe_enabled) {
            debugf("Cloning pipe enabled\n");
            task->pipe_enabled = self->pipe_enabled;
        }

        task->lock = false;
    }

    regs->rax = (uint64_t) task;
}