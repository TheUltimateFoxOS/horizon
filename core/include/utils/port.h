#pragma once


#include <stdint.h>

class Port8Bit{
	private:
		uint16_t portnumber;
	public:
		uint16_t offset;
		Port8Bit(uint16_t port);
		~Port8Bit();
		
		void Write(uint8_t data);
		uint8_t Read();
};

class Port16Bit{
	private:
		uint16_t portnumber;
	public:
		uint16_t offset;
		Port16Bit(uint16_t port);
		~Port16Bit();
		
		void Write(uint16_t data);
		uint16_t Read();
};

class Port32Bit{
	private:
		uint16_t portnumber;
	public:
		uint16_t offset;
		Port32Bit(uint16_t port);
		~Port32Bit();
		
		void Write(uint32_t data);
		uint32_t Read();
};

void io_wait();

unsigned char inb(unsigned short _port);
void outb(unsigned short _port, unsigned char _data);
uint16_t inw(uint16_t _port);
void outw(uint16_t _port, uint16_t _data);
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t data); 