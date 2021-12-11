#pragma once

#include <driver/clock.h>
#include <stdint.h>

namespace cmos {
	#define CMOS_READ_SEC		0x00
	#define CMOS_READ_MIN		0x02
	#define CMOS_READ_HOUR		0x04
	#define CMOD_READ_WEEKDAY	0x06
	#define CMOS_READ_DAY		0x07
	#define CMOS_READ_MONTH		0x08
	#define CMOS_READ_YEAR		0x09
	#define CMOS_READ_CENTURY	0x32

	class cmos_driver : public driver::clock_device {
		public:
			virtual clock_result_t time();

			uint8_t cmos_read(uint8_t address);
			uint8_t cmos_convert(uint8_t num);
			int cmos_command(uint8_t function);
	};
}