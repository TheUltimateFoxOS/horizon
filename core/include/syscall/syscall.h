#pragma once

#include <utils/list.h>
#include <stdint.h>

#include <interrupts/interrupts.h>

namespace syscall {
	struct syscall_entry_t {
		uint64_t id;
		char name[32];
		void (*handler)(interrupts::s_registers*);
	};

	extern list<syscall_entry_t>* syscall_id_list;

	void setup();

	void register_syscall(void (*handler)(interrupts::s_registers*), const char* name);

	void get_id_syscall(interrupts::s_registers* regs);

	extern "C" uint64_t syscall_table[];
	extern "C" uint64_t syscall_table_size;
	extern "C" void syscall_interrupt_stub();

	void sys_memory_alloc(interrupts::s_registers* regs);
	void sys_memory_free(interrupts::s_registers* regs);
	void sys_write(interrupts::s_registers* regs);
	void sys_read(interrupts::s_registers* regs);
	void sys_open(interrupts::s_registers* regs);
	void sys_close(interrupts::s_registers* regs);
	void sys_get_file_size(interrupts::s_registers* regs);
	void sys_delete(interrupts::s_registers* regs);
	void sys_mkdir(interrupts::s_registers* regs);
	void sys_dir_at(interrupts::s_registers* regs);
	void sys_fs_at(interrupts::s_registers* regs);
	void sys_exit(interrupts::s_registers* regs);
	void sys_spawn(interrupts::s_registers* regs);
	void sys_env(interrupts::s_registers* regs);
	void sys_time(interrupts::s_registers* regs);
	void sys_touch(interrupts::s_registers* regs);
	void sys_delete_dir(interrupts::s_registers* regs);

	void sys_dns_resolve(interrupts::s_registers* regs);
	void sys_icmp_echo_request(interrupts::s_registers* regs);

	void sys_socket_connect(interrupts::s_registers* regs);
	void sys_socket_disconnect(interrupts::s_registers* regs);
	void sys_socket_send(interrupts::s_registers* regs);
	void sys_socket_recv(interrupts::s_registers* regs);

	void sys_thread(interrupts::s_registers* regs);

	void sys_sound_push_note(interrupts::s_registers* regs);
	void sys_sound_sync(interrupts::s_registers* regs);
	void sys_sound_get_channel_count(interrupts::s_registers* regs);

	void sys_delay(interrupts::s_registers* regs);

	void sys_mouse_reset(interrupts::s_registers* regs);
	void sys_mouse_get_pos(interrupts::s_registers* regs);
	void sys_mouse_get_buttons(interrupts::s_registers* regs);

	void sys_memory_info(interrupts::s_registers* regs);

	void sys_ipc_register(interrupts::s_registers* regs);
	void sys_ipc_unregister(interrupts::s_registers* regs);
	void sys_ipc_get_hid(interrupts::s_registers* regs);
	void sys_ipc_send_message(interrupts::s_registers* regs);
}