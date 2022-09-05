#include <interrupts/interrupts.h>
#include <interrupts/interrupt_handler.h>
#include <interrupts/idt.h>

#include <memory/page_frame_allocator.h>
#include <memory/page_table_manager.h>

#include <utils/port.h>
#include <utils/abort.h>
#include <utils/log.h>
#include <utils/string.h>

#include <scheduler/scheduler.h>

#include <apic/apic.h>

#include <config.h>

using namespace interrupts;

namespace interrupts {
	__attribute__((aligned(0x1000)))
	interrupts::idt_t idtr;

	__attribute__((aligned(0x1000)))
	interrupts::idt_desc_entry_t idt_d[256];
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

extern "C" void load_idt(void* idt_ptr);

//#interrupts::prepare_interrupts-doc: Sets up the IDT and the interrupt handlers. Also remaps the programable interrupt controller.
void interrupts::prepare_interrupts() {
	__asm__ __volatile__ ("cli");

	debugf("Initialising idt...\n");
	idtr.limit =  0x0FFF;
	idtr.offset = (uint64_t) idt_d;
	
	memset(idt_d, 0, sizeof(interrupts::idt_desc_entry_t) * 256);

	debugf("IDT at %p\n", idtr.offset);

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

	debugf("IDTR at %p\n", &idtr);

	// asm ("lidt %0" : : "m" (idtr));

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

	load_idt(&idtr);

	__asm__ __volatile__ ("sti");
}

uint64_t task_start_address = 0; //Used in the stack trace
uint64_t task_end_address = 0;

//#intr_common_handler_c-doc: The general purpose interrupt handler. This handler is called when an interrupt is received. The handler will check if there is a interrupt handler for the interrupt. If there is a interrupt handler, the handler will be called. If the interrupt is a exception, the handler will cause a panic if there is no signal handler.
extern "C" void intr_common_handler_c(s_registers* regs) {
	if(regs->interrupt_number <= 0x1f) {
		char* interrupt_name = get_interrupt_name(regs->interrupt_number);
		debugf("Kernel panic: %s", interrupt_name);

		if (regs->interrupt_number == 0xa || regs->interrupt_number == 0xb || regs->interrupt_number == 0xc || regs->interrupt_number == 0xd) { //Selector error code
			debugf_raw(".");

			if (regs->error_code & 0x1) {
				debugf_raw(" Exception was external to the CPU.");
			}

			uint64_t tmp = regs->error_code >> 1;
			if (tmp & 0b00) {
				debugf_raw(" Exception was in the GDT.");
			} else if (tmp & 0b01) {
				debugf_raw(" Exception was in the IDT.");
			} else if (tmp & 0b10) {
				debugf_raw(" Exception was in the LDT.");
			} else if (tmp & 0b11) {
				debugf_raw(" Exception was in the IDT.");
			}

			tmp >>= 2;
			debugf_raw(" Index: %x.", tmp);
		}

		if (regs->interrupt_number == 0xe) { //Page fault error code
			uint64_t faulting_address = regs->cr2;
			debugf_raw(" at %x.", faulting_address);

			if (!(regs->error_code & 0x1)) {
				debugf_raw(" Page not present.");
			}
			if (regs->error_code & 0x2) {
				debugf_raw(" Write operation failed.");
			}
			if (regs->error_code & 0x4) {
				debugf_raw(" Processor was in user-mode.");
			}
			if (regs->error_code & 0x8) {
				debugf_raw(" Overwritten CPU-reserved bits of page entry.");
			}
			if (regs->error_code & 0x10) {
				debugf_raw(" Caused by an instruction fetch.");
			}
		}

		debugf_raw("\n");
		debugf("Error code: %x\n", regs->error_code);

		LAPIC_ID(id);

		scheduler::task_t* task = scheduler::task_queue[id]->list[0];

		task_start_address = (uint64_t) task->offset;
		task_end_address = task_start_address + (task->page_count * 0x1000);

		debugf("Caused by task \"%s\" at %x. Stack trace:\n", task->argv[0], ((uint64_t) regs->rip) - task_start_address);

		elf::unwind(10, regs->rbp, [](int frame_num, uint64_t rip) {
			if (rip >= task_start_address && rip < task_end_address) {
				debugf("%d: %x\n", frame_num, rip - task_start_address);
			}
		});


		task_start_address = 0;
		task_end_address = 0;

	#ifdef SEND_SIGNALS
		if (!scheduler::handle_signal(regs->interrupt_number)) {
			abortf(interrupt_name);
		}
	#else
		#warning "Signal sending is disabled!!!"
		abortf(interrupt_name);
	#endif
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