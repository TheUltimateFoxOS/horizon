#include <syscall/syscall.h>

#include <utils/log.h>
#include <utils/string.h>

#include <interrupts/interrupts.h>

using namespace syscall;

namespace syscall {
	list<syscall_entry_t>* syscall_id_list = nullptr;
}

void syscall::get_id_syscall(interrupts::s_registers* regs) {
	char* name = (char*) regs->rbx;

	list<syscall_entry_t>::node* syscall_node = syscall_id_list->find<char*>([](char* name, list<syscall_entry_t>::node* n) {
		return strcmp(name, n->data.name) == 0;
	}, name);

	if (syscall_node == nullptr) {
		regs->rax = -1;
		return;
	} else {
		regs->rax = syscall_node->data.id;
	}
}

// void test_syscall(interrupts::s_registers* regs) {
// 	printf("syscall::test_syscall\n");
// }

void syscall::setup() {
	debugf("Setting up syscall's...\n");
	syscall_id_list = new list<syscall_entry_t>(10);

	syscall_table[0] = (uint64_t) get_id_syscall;
	syscall_table_size++;

	syscall_entry_t get_id_syscall_entry = {
		.id = 0,
		.name = { 'g', 'e', 't', '_', 'i', 'd', '_', 's', 'y', 's', 'c', 'a', 'l', 'l', '\0' },
		.handler = get_id_syscall
	};

	syscall_id_list->add(get_id_syscall_entry);

	debugf("Setting interrupt handler for syscall...\n");
	interrupts::set_idt_gate((void*) syscall_interrupt_stub, 0x30, idt_ta_interrupt_gate, 0x08);

	// register_syscall(test_syscall, "test_syscall");
	register_syscall(sys_memory_alloc, "sys_memory_alloc");
	register_syscall(sys_memory_free, "sys_memory_free");
	register_syscall(sys_write, "sys_write");
	register_syscall(sys_read, "sys_read");
	register_syscall(sys_exit, "sys_exit");
}

void syscall::register_syscall(void (*handler)(interrupts::s_registers*), const char* name) {
	debugf("Registering syscall %s...\n", name);
	syscall_entry_t syscall_entry = {
		.id = syscall_table_size,
		.handler = handler
	};

	strcpy(syscall_entry.name, name);

	syscall_table[syscall_table_size] = (uint64_t) handler;
	syscall_table_size++;

	syscall_id_list->add(syscall_entry);

	debugf("Syscall %s got id %d\n", name, syscall_entry.id);
}