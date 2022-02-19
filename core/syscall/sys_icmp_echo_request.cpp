#include <syscall/syscall.h>

#include <driver/nic.h>
#include <net/network_stack.h>
#include <utils/log.h>

using namespace syscall;

void syscall::sys_icmp_echo_request(interrupts::s_registers* regs) {
	uint32_t ip = regs->rbx;
	int nic_id = regs->rcx;


	__asm__ __volatile__ ("sti");
	regs->rbx = driver::global_nic_manager->get_nic(nic_id)->network_stack->icmp->send_echo_reqest_and_wait(ip);
	__asm__ __volatile__ ("cli");
}