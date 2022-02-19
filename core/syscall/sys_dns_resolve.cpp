#include <syscall/syscall.h>

#include <driver/nic.h>
#include <net/network_stack.h>
#include <utils/log.h>

using namespace syscall;

void syscall::sys_dns_resolve(interrupts::s_registers* regs) {
	char* domain = (char*) regs->rbx;
	int nic_id = regs->rcx;

	debugf("sys_dns_resolve: domain: %s, nic_id: %d\n", domain, nic_id);

	__asm__ __volatile__ ("sti");
	regs->rbx = driver::global_nic_manager->get_nic(nic_id)->network_stack->dns->resolve_A(domain);
	__asm__ __volatile__ ("cli");
}