#include <stdint.h>
#include <utils/cstr.h>

namespace pci {
	const char* device_classes[] {
		"Unclassified",
		"Mass Storage Controller",
		"Network Controller",
		"Display Controller",
		"Multimedia Controller",
		"Memory Controller",
		"Bridge Device",
		"Simple Communication Controller",
		"Base System Peripheral",
		"Input Device Controller",
		"Docking Station", 
		"Processor",
		"Serial Bus Controller",
		"Wireless Controller",
		"Intelligent Controller",
		"Satellite Communication Controller",
		"Encryption Controller",
		"Signal Processing Controller",
		"Processing Accelerator",
		"Non Essential Instrumentation"
	};

	//#get_vendor_name-doc: Get the vendor name of a pci device as a string.
	const char* get_vendor_name(uint16_t vendor_ID) {
		switch (vendor_ID) {
			case 0x8086:
				return "Intel Corp.";
			case 0x1022:
				return "AMD, Inc.";
			case 0x10DE:
				return "NVIDIA Corporation";
			case 0x10EC:
				return "Realtek Semiconductor Co., Ltd.";
		}
		return num_to_string(vendor_ID, 16);
	}

	//#get_device_name-doc: Get the device name of a pci device as a string.
	const char* get_device_name(uint16_t vendor_ID, uint16_t device_ID) {
		switch (vendor_ID) {
			case 0x8086: //Intel
				switch (device_ID) {
					case 0x29C0:
						return "Express DRAM Controller";
					case 0x2918:
						return "LPC Interface Controller";
					case 0x2922:
						return "6 port SATA Controller [AHCI mode]";
					case 0x2930:
						return "SMBus Controller";
					case 0x100E:
						return "Intel Gigabit Ethernet"; //Qemu, Bochs, and VirtualBox emmulated NICs
					case 0x10EA:
						return "82577LM Gigabit Network Connection";
					case 0x153A:
						return "Ethernet Connection I217-LM";
				}
			case 0x1022: //AMD
				switch (device_ID) {
					case 0x2000:
						return "AM79C973";
				}
			case 0x10EC: //Realtek
				switch (device_ID)
				{
					case 0x8139:
						return "RTL8193";
				}
		}
		return num_to_string(device_ID, 16);
	}

	//#mass_storage_controller_subclass_name-doc: Get the subclass name of a mass storage controller as a string.
	const char* mass_storage_controller_subclass_name(uint8_t subclass_code) {
		switch (subclass_code) {
			case 0x00:
				return "SCSI Bus Controller";
			case 0x01:
				return "IDE Controller";
			case 0x02:
				return "Floppy Disk Controller";
			case 0x03:
				return "IPI Bus Controller";
			case 0x04:
				return "RAID Controller";
			case 0x05:
				return "ATA Controller";
			case 0x06:
				return "Serial ATA";
			case 0x07:
				return "Serial Attached SCSI";
			case 0x08:
				return "Non-Volatile Memory Controller";
			case 0x80:
				return "Other";
		}
		return num_to_string(subclass_code, 16);
	}

	//#serial_bus_controller_subclass_name-doc: Get the subclass name of a serial bus controller as a string.
	const char* serial_bus_controller_subclass_name(uint8_t subclass_code) {
		switch (subclass_code) {
			case 0x00:
				return "FireWire (IEEE 1394) Controller";
			case 0x01:
				return "ACCESS Bus";
			case 0x02:
				return "SSA";
			case 0x03:
				return "USB Controller";
			case 0x04:
				return "Fibre Channel";
			case 0x05:
				return "SMBus";
			case 0x06:
				return "Infiniband";
			case 0x07:
				return "IPMI Interface";
			case 0x08:
				return "SERCOS Interface (IEC 61491)";
			case 0x09:
				return "CANbus";
			case 0x80:
				return "SerialBusController - Other";
		}
		return num_to_string(subclass_code, 16);
	}

	//#bridge_device_subclass_name-doc: Get the subclass name of a bridge device as a string.
	const char* bridge_device_subclass_name(uint8_t subclass_code) {
		switch (subclass_code) {
			case 0x00:
				return "Host Bridge";
			case 0x01:
				return "ISA Bridge";
			case 0x02:
				return "EISA Bridge";
			case 0x03:
				return "MCA Bridge";
			case 0x04:
				return "PCI-to-PCI Bridge";
			case 0x05:
				return "PCMCIA Bridge";
			case 0x06:
				return "NuBus Bridge";
			case 0x07:
				return "CardBus Bridge";
			case 0x08:
				return "RACEway Bridge";
			case 0x09:
				return "PCI-to-PCI Bridge";
			case 0x0a:
				return "InfiniBand-to-PCI Host Bridge";
			case 0x80:
				return "Other";
		}
		return num_to_string(subclass_code, 16);
	}

	//#get_subclass_name-doc: Get the subclass name of a device as a string.
	const char* get_subclass_name(uint8_t class_code, uint8_t subclass_code) {
		switch (class_code) {
			case 0x01:
				return mass_storage_controller_subclass_name(subclass_code);
			case 0x03:
				switch (subclass_code) {
					case 0x00:
						return "VGA Compatible Controller";
				}
			case 0x06:
				return bridge_device_subclass_name(subclass_code);
			case 0x0C:
				return serial_bus_controller_subclass_name(subclass_code);
		}
		return num_to_string(subclass_code, 16);
	}

	//#get_prog_IF_name-doc: Get the interface name of a device as a string.
	const char* get_prog_IF_name(uint8_t class_code, uint8_t subclass_code, uint8_t prog_IF) {
		switch (class_code) {
			case 0x01:
				switch (subclass_code) {
					case 0x06:
						switch (prog_IF) {
							case 0x00:
								return "Vendor Specific Interface";
							case 0x01:
								return "AHCI 1.0";
							case 0x02:
								return "Serial Storage Bus";
						}
				}
			case 0x03:
				switch (subclass_code) {
					case 0x00:
						switch (prog_IF) {
							case 0x00:
								return "VGA Controller";
							case 0x01:
								return "8514-Compatible Controller";
						}
				}
			case 0x0C:
				switch (subclass_code) {
					case 0x03:
						switch (prog_IF) {
							case 0x00:
								return "UHCI Controller";
							case 0x10:
								return "OHCI Controller";
							case 0x20:
								return "EHCI (USB2) Controller";
							case 0x30:
								return "XHCI (USB3) Controller";
							case 0x80:
								return "Unspecified";
							case 0xFE:
								return "USB Device (Not a Host Controller)";
						}
				}
		}
		return num_to_string(prog_IF, 16);
	}
}