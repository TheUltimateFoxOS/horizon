#pragma once

#include <stdint.h>

namespace smp {

	struct bootstrap_data_t {
		uint8_t status;
		uint64_t pagetable;
		uint64_t idt;
		uint64_t gdt;
		uint64_t entry;
	} __attribute__ ((packed));


	enum ap_status {
		incpp = 1,
		init_done
	};

	void startup();
	void ap_main();
}