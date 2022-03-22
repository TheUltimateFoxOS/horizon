#pragma once

#include <stdint.h>

#include <driver/driver.h>
#include <interrupts/interrupt_handler.h>

#include <pci/pci.h>
#include <stdint.h>

#include <utils/list.h>

namespace xhci {
	struct capabilites_regs {
		uint8_t caplength;
		uint8_t reserved;
		uint16_t hci_version;
		uint32_t hcsparams1;
		uint32_t hcsparams2;
		uint32_t hcsparams3;
		uint32_t hccparams1;
		uint32_t dboff;
		uint32_t rtsoff;
		uint32_t hccparams2;
	} __attribute__((packed));

	struct operational_regs {
		uint32_t usbcmd;
		uint32_t usbsts;
		uint32_t pagesize;
		uint64_t reserved;
		uint32_t dnctrl;
		uint64_t crcr;
		uint8_t reserved_0[16];
		uint64_t dcbaap;
		uint32_t config;
		uint8_t reserved_1[0x3C4];
		struct {
			uint32_t portsc;
			uint32_t portpmsc;
			uint32_t portli;
			uint32_t porthlpmc;
		} __attribute__((packed)) ports[];
	} __attribute__((packed));

	namespace usbcmd {
		constexpr uint32_t run = (1 << 0);
		constexpr uint32_t reset = (1 << 1);
		constexpr uint32_t irq_enable = (1 << 2);
		constexpr uint32_t host_system_err_enable = (1 << 3);
	}

	namespace usbsts {
		constexpr uint32_t halted = (1 << 0);
		constexpr uint32_t host_system_error = (1 << 2);
		constexpr uint32_t irq = (1 << 3);
		constexpr uint32_t not_ready = (1 << 11);
	}

	namespace portsc {
		constexpr uint32_t connect_status = (1 << 0);
		constexpr uint32_t enabled = (1 << 1);
		constexpr uint32_t overcurrent_active = (1 << 3);
		constexpr uint32_t reset = (1 << 4);
		constexpr uint32_t port_power = (1 << 9);
		constexpr uint32_t write_strobe = (1 << 16);
		constexpr uint32_t connect_status_change = (1 << 17);
		constexpr uint32_t port_enabled_change = (1 << 18);
		constexpr uint32_t warm_port_reset_change = (1 << 19);
		constexpr uint32_t overcurrent_change = (1 << 20);
		constexpr uint32_t reset_change = (1 << 21);
		constexpr uint32_t link_status_change = (1 << 22);
		constexpr uint32_t config_error_change = (1 << 23);
		constexpr uint32_t cold_attach_status = (1 << 24);
		constexpr uint32_t wake_on_connect = (1 << 25);
		constexpr uint32_t wake_on_disconnect = (1 << 26);
		constexpr uint32_t wake_on_overcurrent = (1 << 27);
		constexpr uint32_t removeable = (1 << 30);
		constexpr uint32_t warm_reset = (1u << 31);

		constexpr uint32_t status_change_bits = connect_status_change | port_enabled_change | overcurrent_change | reset_change | link_status_change;

		// Values for Port Speed bits 10:13
		constexpr uint32_t full_speed = 1;
		constexpr uint32_t low_speed = 2;
		constexpr uint32_t high_speed = 3;
		constexpr uint32_t super_speed = 4;
	}


	struct runtime_regs {
		uint32_t microframe_index;
		uint8_t reserved[0x1C];

		struct {
			uint32_t iman;
			uint32_t imod;
			uint32_t erst_size;
			uint32_t reserved;
			uint64_t erst_base;
			uint64_t erst_dequeue;
		} __attribute__((packed)) interrupters[];
	} __attribute__((packed));

	namespace iman {
		constexpr uint32_t irq_pending = (1 << 0);
		constexpr uint32_t irq_enable = (1 << 1);
	}
	

	struct ERST_entry {
		uint64_t ring_base;
		uint16_t ring_size;
		uint16_t reserved;
		uint32_t reserved_0;
	} __attribute__((packed));

	struct legacy_cap {
		uint32_t usblegsup;
		uint32_t usblegctlsts;
	} __attribute__((packed));

	namespace usblegsup {
		constexpr uint32_t bios_owned = (1 << 16);
		constexpr uint32_t os_owned = (1 << 24);
	}
	

	struct protocol_cap {
		uint32_t version;
		uint32_t name;
		uint32_t ports_info;
		uint32_t slot_type;
		uint32_t speeds[];
	} __attribute__((packed));

	struct protocol {
		uint8_t major, minor, slot_type;
		uint32_t port_off, port_count;
	};

	class xhci_driver : public driver::device_driver, public interrupts::interrupt_handler {
		public:
			xhci_driver(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function);
		
			virtual void handle();
			virtual void activate();
			virtual char* get_name();
		private:
			pci::pci_header_0_t* header;
			uint16_t bus;
			uint16_t device;
			uint16_t function;

			capabilites_regs* capabilites;
			operational_regs* operational;
			runtime_regs* runtime;
			volatile uint32_t* db;
			volatile uint32_t* ext_caps;

			bool is_intel;

			uint32_t* ext_capabilities;

			size_t n_slots;
			size_t n_interrupters;
			size_t n_ports;
			size_t context_size;
			size_t erst_max;
			size_t n_scratchbufs;
			size_t page_size;

			list<protocol> protocols;

			void intel_enable_ports();
			void reset_controller();
	};
}