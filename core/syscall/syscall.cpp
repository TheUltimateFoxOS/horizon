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
	syscall_id_list = new list<syscall_entry_t>(20);

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
	register_syscall(sys_open, "sys_open");
	register_syscall(sys_close, "sys_close");
	register_syscall(sys_delete, "sys_delete");
	register_syscall(sys_mkdir, "sys_mkdir");
	register_syscall(sys_dir_at, "sys_dir_at");
	register_syscall(sys_fs_at, "sys_fs_at");
	register_syscall(sys_get_file_size, "sys_get_file_size");
	register_syscall(sys_exit, "sys_exit");
	register_syscall(sys_spawn, "sys_spawn");
	register_syscall(sys_env, "sys_env");
	register_syscall(sys_time, "sys_time");
	register_syscall(sys_clock, "sys_clock");
	register_syscall(sys_clock_ticks_per_second, "sys_clock_ticks_per_second");
	register_syscall(sys_touch, "sys_touch");
	register_syscall(sys_dns_resolve, "sys_dns_resolve");
	register_syscall(sys_icmp_echo_request, "sys_icmp_echo_request");
	register_syscall(sys_socket_connect, "sys_socket_connect");
	register_syscall(sys_socket_disconnect, "sys_socket_disconnect");
	register_syscall(sys_socket_send, "sys_socket_send");
	register_syscall(sys_socket_recv, "sys_socket_recv");
	register_syscall(sys_thread, "sys_thread");
	register_syscall(sys_sound_push_note, "sys_sound_push_note");
	register_syscall(sys_sound_sync, "sys_sound_sync");
	register_syscall(sys_sound_get_channel_count, "sys_sound_get_channel_count");
	register_syscall(sys_delay, "sys_delay");
	register_syscall(sys_delete_dir, "sys_delete_dir");
	register_syscall(sys_mouse_get_buttons, "sys_mouse_get_buttons");
	register_syscall(sys_mouse_get_pos, "sys_mouse_get_pos");
	register_syscall(sys_mouse_reset, "sys_mouse_reset");
	register_syscall(sys_memory_info, "sys_memory_info");
	register_syscall(sys_ipc_register, "sys_ipc_register");
	register_syscall(sys_ipc_unregister, "sys_ipc_unregister");
	register_syscall(sys_ipc_get_hid, "sys_ipc_get_hid");
	register_syscall(sys_ipc_send_message, "sys_ipc_send_message");
	register_syscall(sys_insmod, "sys_insmod");
	register_syscall(sys_dbg, "sys_dbg");
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
