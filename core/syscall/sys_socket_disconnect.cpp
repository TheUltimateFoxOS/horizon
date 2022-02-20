#include <syscall/syscall.h>

#include <driver/nic.h>
#include <net/network_stack.h>
#include <net/socket_manager.h>
#include <utils/log.h>

using namespace syscall;

void syscall::sys_socket_disconnect(interrupts::s_registers* regs) {
	net::socket_manager_socket* socket = net::global_socket_manager->get_socket(regs->rbx);

	if (socket == nullptr) {
		regs->rdx = -1;
		return;
	}

	socket->disconnect();

	net::global_socket_manager->free_socket(socket->socket_id);
	delete socket;

	regs->rdx = 0;
}