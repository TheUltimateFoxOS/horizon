#include <utils/port.h>

Port8Bit::Port8Bit(uint16_t port){
	portnumber = port;
	offset = 0;
}

Port8Bit::~Port8Bit(){
	
}

uint8_t Port8Bit::Read(){
	uint8_t result;
	__asm__ ("inb %1, %0" : "=a" (result) : "Nd" ((uint16_t) portnumber + offset));
	return result;
}

void Port8Bit::Write(uint8_t data){
	__asm__ ("outb %0, %1" : : "a" (data), "Nd" ((uint16_t) portnumber + offset));
}

Port16Bit::Port16Bit(uint16_t port){
	portnumber = port;
	offset = 0;
}

Port16Bit::~Port16Bit(){
	
}

uint16_t Port16Bit::Read(){
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" ((uint16_t) portnumber + offset));
	return result;
}

void Port16Bit::Write(uint16_t data){
	asm volatile ("outw %0, %1" : : "a" (data), "Nd" ((uint16_t) portnumber + offset));
}

Port32Bit::Port32Bit(uint16_t port){
	portnumber = port;
	offset = 0;
}

Port32Bit::~Port32Bit(){
	
}

uint32_t Port32Bit::Read(){
	uint32_t result;
	asm volatile ("inl %1, %0" : "=a" (result) : "d" ((uint16_t) portnumber + offset));
	return result;
}

void Port32Bit::Write(uint32_t data){
	asm volatile ("outl %0, %1" : : "a" (data), "Nd" ((uint16_t) portnumber + offset));
}

void io_wait() {
	asm volatile ("outb %%al, $0x80" : : "a"(0));
}

unsigned char inb(unsigned short _port){
	unsigned char result;
	__asm__ ("inb %1, %0" : "=a" (result) : "Nd" (_port));
	return result;
}

void outb(unsigned short _port, unsigned char _data){
	__asm__ ("outb %0, %1" : : "a" (_data), "Nd" (_port));
}

uint16_t inw(uint16_t _port) {
	uint16_t result;
	__asm__ volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
	return result;
}

void outw(uint16_t _port, uint16_t data) {
	asm volatile ("outw %0, %1" : : "a" (data), "Nd" (_port));
}

uint32_t inl(uint16_t _port) {
	uint32_t result;
	__asm__ volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
	return result;
}

void outl(uint16_t _port, uint32_t data) {
	asm volatile ("outl %0, %1" : : "a" (data), "Nd" (_port));
}