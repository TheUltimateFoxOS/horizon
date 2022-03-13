#pragma once

#include <driver/driver.h>
#include <driver/nic.h>
#include <interrupts/interrupt_handler.h>
#include <pci/pci.h>

namespace driver {
	class rtl8139_driver: public driver::device_driver, public interrupts::interrupt_handler, public driver::nic_device {

		public:

			#define RX_BUF_SIZE 8192

			#define CAPR 0x38
			#define RX_READ_POINTER_MASK (~3)
			#define ROK (1<<0)
			#define RER (1<<1)
			#define TOK (1<<2)
			#define TER (1<<3)
			#define TX_TOK (1<<15)

			typedef struct tx_desc {
				uint32_t phys_addr;
				uint32_t packet_size;
			} tx_desc_t;

			rtl8139_driver(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function);
			~rtl8139_driver();

			virtual void send(uint8_t* data, int32_t len);
			virtual uint64_t get_mac();

			virtual uint32_t get_ip();
			virtual void set_ip(uint32_t ip);

			virtual void handle();
			virtual void activate();
			virtual char* get_name();

		private:
			void receive();

			uint8_t bar_type;
			uint16_t io_base;
			uint32_t mem_base;
			int eeprom_exist;
			char* rx_buffer;
			int tx_cur;

			pci::pci_header_0_t* header;
			uint16_t bus;
			uint16_t device;
			uint16_t function;

			uint32_t ip;

			uint32_t current_packet_ptr;
	};
}