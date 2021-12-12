#pragma once

#include <driver/driver.h>
#include <driver/nic.h>
#include <interrupts/interrupt_handler.h>
#include <pci/pci.h>

namespace driver {
	class am79C973_driver: public driver::device_driver, public interrupts::interrupt_handler, public driver::nic_device {

		public:
			struct initialization_block_t {
				uint16_t mode;
				unsigned reserved1 : 4;
				unsigned numSendBuffers : 4;
				unsigned reserved2 : 4;
				unsigned numRecvBuffers : 4;
				uint64_t physicalAddress : 48;
				uint16_t reserved3;
				uint64_t logicalAddress;
				uint32_t recvBufferDescrAddress;
				uint32_t sendBufferDescrAddress;
			} __attribute__((packed));

			struct buffer_descriptor_t {
				uint32_t address;
				uint32_t flags;
				uint32_t flags2;
				uint32_t avail;
			} __attribute__((packed));

			am79C973_driver(pci::pci_header_0_t* header, uint16_t bus, uint16_t device, uint16_t function);
			~am79C973_driver();

			virtual void send(uint8_t* data, int32_t len);
			virtual uint64_t get_mac();

			virtual uint32_t get_ip();
			virtual void set_ip(uint32_t ip);

		private:
			pci::pci_header_0_t* header;
			uint16_t base_port;

			buffer_descriptor_t* sendBufferDescr;
			uint8_t* sendBufferDescrMemory;
			uint8_t* sendBuffers;
			uint8_t currentSendBuffer;
			
			buffer_descriptor_t* recvBufferDescr;
			uint8_t* recvBufferDescrMemory;
			uint8_t* recvBuffers;
			uint8_t currentRecvBuffer;

			initialization_block_t* init_block;

			virtual void handle();
			virtual void activate();
			virtual char* get_name();

			void receive();
	};
}