#include <syscall/syscall.h>

#include <driver/nic.h>
#include <net/network_stack.h>
#include <net/socket_manager.h>
#include <utils/log.h>

using namespace syscall;

void syscall::sys_socket_connect(interrupts::s_registers* regs) {
	int socket_id = net::global_socket_manager->alloc_socket();

	net::socket_manager_socket* socket = new net::socket_manager_socket(regs->rbx);
	socket->socket_id = socket_id;
	net::global_socket_manager->register_socket(socket);

	socket->connect(regs->rcx, regs->rdx);

	regs->rdx = socket_id;
}