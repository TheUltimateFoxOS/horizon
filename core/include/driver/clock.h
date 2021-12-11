#pragma once

namespace driver {
	class clock_device {
		public:
			struct clock_result_t {
				int seconds;
				int minutes;
				int hours;
				int day;
				int month;
				int year;
			};

			virtual clock_result_t time();
	};

	extern clock_device* default_clock_device;
}