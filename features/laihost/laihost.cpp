extern "C" {
	#include <lai/host.h>
	#include <acpispec/tables.h>
}

#include <memory/page_table_manager.h>
#include <memory/page_frame_allocator.h>
#include <utils/abort.h>
#include <utils/log.h>
#include <utils/string.h>
#include <utils/port.h>
#include <pci/pci.h>
#include <timer/timer.h>
#include <acpi/acpi.h>

extern "C" {
	void* laihost_map(size_t address, size_t count) {
		for (int i = 0; i < count / 0x1000; i++) {
			memory::global_page_table_manager.map_memory((void*) (address + i * 0x1000), (void*) (address + i * 0x1000));
		}

		return (void*) address;
	}

	void laihost_unmap(void* pointer, size_t count) {
		debugf("WARNING: laihost_unmap: %x %d not implemented!\n", pointer, count);
	}

	void laihost_log(int level, const char* msg) {
		switch (level) {
			case LAI_WARN_LOG:
				debugf("WARNING: %s\n", msg);
				break;
			
			case LAI_DEBUG_LOG:
				debugf("DEBUG: %s\n", msg);
				break;
			
			default:
				debugf("UNKNOWN: %s\n", msg);
				break;
		}
	}

	__attribute__((noreturn))
	void laihost_panic(const char* msg) {
		abortf("laihost: %s\n", msg);
	}

	void* laihost_malloc(size_t size) {
		return memory::global_allocator.request_pages(size / 0x1000 + 1);
	}

	void* laihost_realloc(void *oldptr, size_t newsize, size_t oldsize) {
		if (newsize == 0) {
				laihost_free(oldptr, oldsize);
				return nullptr;
		} else if (!oldptr) {
			return laihost_malloc(newsize);
			} else if (newsize <= oldsize) {
				return oldptr;
			} else {
			void* newptr = laihost_malloc(newsize);
			memcpy(newptr, oldptr, oldsize);

			return newptr;
		}
	}

	void laihost_free(void *ptr, size_t size) {
		memory::global_allocator.free_pages(ptr, size / 0x1000 + 1);
	}

	void laihost_outb(uint16_t port, uint8_t val) {
		outb(port, val);
	}

	void laihost_outw(uint16_t port, uint16_t val) {
		outw(port, val);
	}

	void laihost_outd(uint16_t port, uint32_t val) {
		outl(port, val);
	}

	uint8_t laihost_inb(uint16_t port) {
		return inb(port);
	}

	uint16_t laihost_inw(uint16_t port) {
		return inw(port);
	}

	uint32_t laihost_ind(uint16_t port) {
		return inl(port);
	}

	void laihost_pci_writeb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint8_t val) {
		pci::pci_writeb(bus, slot, fun, offset, val);
	}

	void laihost_pci_writew(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint16_t val) {
		pci::pci_writew(bus, slot, fun, offset, val);
	}

	void laihost_pci_writed(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset, uint32_t val) {
		pci::pci_writed(bus, slot, fun, offset, val);
	}

	uint8_t laihost_pci_readb(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
		return pci::pci_readb(bus, slot, fun, offset);
	}

	uint16_t laihost_pci_readw(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
		return pci::pci_readw(bus, slot, fun, offset);
	}

	uint32_t laihost_pci_readd(uint16_t seg, uint8_t bus, uint8_t slot, uint8_t fun, uint16_t offset) {
		return pci::pci_readd(bus, slot, fun, offset);
	}

	void laihost_sleep(uint64_t ms) {
		timer::global_timer->sleep(ms);
	}

	uint64_t laihost_timer(void) {
		laihost_panic("laihost_timer not implemented! What is that even?");
	}

	void laihost_handle_amldebug(lai_variable_t* var) {
		debugf("DEBUG: laihost_handle_amldebug with %x\n", var);
	}

	void laihost_handle_global_notify(lai_nsnode_t* node, int _) {
		debugf("DEBUG: laihost_handle_global_notify with %x\n", node);

	}


	int laihost_sync_wait(struct lai_sync_state *sync, unsigned int val, int64_t timeout) {
		debugf("WARNING: laihost_sync_wait not implemented!\n");
		return -1;
	}

	void laihost_sync_wake(struct lai_sync_state *sync) {
		debugf("WARNING: laihost_sync_wake not implemented!\n");
	}

	void* laihost_scan(const char *sig, size_t index) {
		if (memcmp(sig, "DSDT", 4) == 0) {
			return memory::map_if_necessary((void*) (uint64_t) ((acpi_fadt_t*) acpi::find_table((char*) "FACP", 0))->dsdt);
		} else {
			return acpi::find_table((char*) sig, index);
		}
	}

	void* memcpy(void* dest, const void* src, size_t count) {
		for (size_t i = 0; i < count; i++) {
			((uint8_t*) dest)[i] = ((uint8_t*) src)[i];
		}

		return dest;
	}

	void* memset(void* dest, int val, size_t count) {
		for (size_t i = 0; i < count; i++) {
			((uint8_t*) dest)[i] = val;
		}

		return dest;
	}

	int memcmp(const void* dest, const void* src, size_t count) {
		for (size_t i = 0; i < count; i++) {
			if (((uint8_t*) dest)[i] != ((uint8_t*) src)[i]) {
				return ((uint8_t*) dest)[i] - ((uint8_t*) src)[i];
			}
		}

		return 0;
	}
}