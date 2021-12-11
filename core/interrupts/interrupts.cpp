#include <interrupts/interrupts.h>
#include <interrupts/interrupt_handler.h>
#include <interrupts/idt.h>

#include <memory/page_frame_allocator.h>

#include <utils/port.h>
#include <utils/abort.h>
#include <utils/log.h>

#include <apic/apic.h>

using namespace interrupts;

namespace interrupts {
	interrupts::idt_t idtr;
}

//#interrupts::get_panic_message-doc: Converts a exception number in a string.
char* interrupts::get_interrupt_name(int interrupt_number) {
	switch(interrupt_number){
		case 0x0:
			return((char*) "Divide by Zero");
			break;
		case 0x1:
			return((char*) "Debug");
			break;
		case 0x2:
			return((char*) "Non Maskable Interrupt");
			break;
		case 0x3:
			return((char*) "Breakpoint");
			break;
		case 0x4:
			return((char*) "Overflow");
			break;
		case 0x5:
			return((char*) "Bound Range");
			break;
		case 0x6:
			return((char*) "Invalid Opcode");
			break;
		case 0x7:
			return((char*) "Device Not Available");
			break;
		case 0x8:
			return((char*) "Double Fault");
			break;
		case 0x9:
			return((char*) "Coprocessor Segment Overrun");
			break;
		case 0xa:
			return((char*) "Invalid TSS");
			break;
		case 0xb:
			return((char*) "Segment not Present");
			break;
		case 0xc:
			return((char*) "Stack Fault");
			break;
		case 0xd:
			return((char*) "General Protection");
			break;
		case 0xe:
			return((char*) "Page Fault");
			break;
		case 0x10:
			return((char*) "x87 Floating Point");
			break;
		case 0x11:
			return((char*) "Alignment Check");
			break;
		case 0x12:
			return((char*) "Machine Check");
			break;
		case 0x13:
			return((char*) "SIMD Floating Point");
			break;
		case 0x1e:
			return((char*) "Security-sensitive event in Host");
			break;
		default:
			return((char*) "Reserved");
			break;
	}
}

//#interrupts::set_idt_gate-doc: Sets the interrupt gate for the given interrupt.
void interrupts::set_idt_gate(void* handler, uint8_t entry_offset, uint8_t type_attr, uint8_t selector) {
	interrupts::idt_desc_entry_t* interrupt = (interrupts::idt_desc_entry_t*)(idtr.offset + entry_offset * sizeof(interrupts::idt_desc_entry_t));
	interrupt->set_offset((uint64_t) handler);
	interrupt->type_attr = type_attr;
	interrupt->selector = selector;
}

//#interrupts::prepare_interrupts-doc: Sets up the IDT and the interrupt handlers. Also remaps the programable interrupt controller.
void interrupts::prepare_interrupts() {
	debugf("Initialising idt...\n");
	idtr.limit =  0x0FFF;
	idtr.offset = (uint64_t) memory::global_allocator.request_page();

	set_idt_gate((void*) intr_stub_0, 0, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_1, 1, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_2, 2, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_3, 3, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_4, 4, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_5, 5, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_6, 6, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_7, 7, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_8, 8, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_9, 9, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_10, 10, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_11, 11, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_12, 12, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_13, 13, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_14, 14, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_15, 15, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_16, 16, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_17, 17, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_18, 18, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_32, 32, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_33, 33, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_34, 34, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_35, 35, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_36, 36, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_37, 37, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_38, 38, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_39, 39, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_40, 40, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_41, 41, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_42, 42, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_43, 43, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_44, 44, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_45, 45, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_46, 46, idt_ta_interrupt_gate, 0x08);
	set_idt_gate((void*) intr_stub_47, 47, idt_ta_interrupt_gate, 0x08);

	set_idt_gate((void*) intr_stub_255, 255, idt_ta_interrupt_gate, 0x08);

	asm ("lidt %0" : : "m" (idtr));

	Port8Bit pic1_data(0x21);
	Port8Bit pic1_command(0x20);
	Port8Bit pci2_data(0xa1);
	Port8Bit pci2_command(0xa0);

	uint8_t a1 = pic1_data.Read();
	io_wait();
	uint8_t a2 = pci2_data.Read();
	io_wait();

	pic1_command.Write(ICW1_INIT | ICW1_ICW4);
	io_wait();
	pci2_command.Write(ICW1_INIT | ICW1_ICW4);
	io_wait();

	pic1_data.Write(0x20);
	io_wait();
	pci2_data.Write(0x28);
	io_wait();

	pic1_data.Write(4);
	io_wait();
	pci2_data.Write(2);
	io_wait();

	pic1_data.Write(ICW4_8086);
	io_wait();
	pci2_data.Write(ICW4_8086);
	io_wait();

	pic1_data.Write(a1);
	io_wait();
	pci2_data.Write(a2);

	pic1_data.Write(0);
	pci2_data.Write(0);

	__asm__ __volatile__ ("sti");
}

//#intr_common_handler_c-doc: The general purpose interrupt handler. This handler is called when an interrupt is received. The handler will check if there is a interrupt handler for the interrupt. If there is a interrupt handler, the handler will be called. If the interrupt is a exception, the handler will cause a panic if there is no signal handler.
extern "C" void intr_common_handler_c(s_registers* regs) {
	if(regs->interrupt_number <= 0x1f) {
		abortf(get_interrupt_name(regs->interrupt_number));
	}

	if (regs->interrupt_number == 0xff) {
		LAPIC_ID(core_id);
		debugf("Halting core %d\n", core_id);

		while (true) {
			__asm__ __volatile__ ("cli; hlt");
		}
	}

	if(regs->interrupt_number >= 0x20 && regs->interrupt_number <= 0x2f) {
		LAPIC_ID(core_id);

		if(handlers[regs->interrupt_number] != NULL) {
			handlers[regs->interrupt_number]->handle();
		}

		if(static_handlers[regs->interrupt_number] != NULL) {
			(*(static_handlers[regs->interrupt_number]))(regs);
		}

		if (apic::bsp_id == core_id) {		
			if (regs->interrupt_number >= 0x28) {
				Port8Bit p(0xa0);
				p.Write(0x20);
			}

			Port8Bit p(0x20);
			p.Write(0x20);
		} else {
			apic::lapic_eoi();
		}
	}
}