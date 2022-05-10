#include <cmos.h>
#include <utils/port.h>

using namespace cmos;


uint8_t cmos_driver::cmos_read(uint8_t address) {
	Port8Bit port_0x70(0x70);
	Port8Bit port_0x71(0x71);

	if (address < 10){
		port_0x70.Write(0xa);

		while((port_0x71.Read() & (1 << 7)) != 0) {
			__asm__ __volatile__ ("pause" ::: "memory");
		}
	}

	port_0x70.Write(address);
	return port_0x71.Read();
}

uint8_t cmos_driver::cmos_convert(uint8_t num) {
	if((cmos_read(0xb) & (1 << 2)) == 0){
		return (num & 0xf) + ((num >> 4) & 0xf) * 10;
	} else {
		return num;
	}
}

int cmos_driver::cmos_command(uint8_t function) {
	return cmos_convert(cmos_read(function));
}

driver::clock_device::clock_result_t cmos_driver::time() {
	int sec = cmos_command(CMOS_READ_SEC);
	int min = cmos_command(CMOS_READ_MIN);
	int hour = cmos_command(CMOS_READ_HOUR);
	int day = cmos_command(CMOS_READ_DAY);
	int month = cmos_command(CMOS_READ_MONTH);
	int century = cmos_command(CMOS_READ_CENTURY);
	int year = cmos_command(CMOS_READ_YEAR) + (century * 100);

	driver::clock_device::clock_result_t result = {
		.seconds = sec,
		.minutes = min,
		.hours = hour,
		.day = day,
		.month = month,
		.year = year
	};
	
	return result;
}
