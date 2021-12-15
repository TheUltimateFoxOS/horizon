#include <utils/unix_time.h>

long long to_unix_time(int year, int month, int day, int hour, int minute, int second) {
	const short days_since_newyear[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

	int leap_years = ((year - 1) - 1968) / 4 - ((year - 1) - 1900) / 100 + ((year - 1) - 1600) / 400;

	long long days_since_1970 = (year - 1970) * 365 + leap_years + days_since_newyear[month - 1] + day - 1;

	if ((month > 2) && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
		days_since_1970 += 1;
	}

	return second + 60 * ( minute + 60 * (hour + 24 * days_since_1970) );
}