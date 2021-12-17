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

void from_unix_time(unsigned long int unixtime, int* year, int* month, int* day, int* hour, int* minute, int* second) {
	const unsigned long int seconds_per_day = 86400ul;
	const unsigned long int days_no_leap_year = 365ul;
	const unsigned long int days_in_4_years = 1461ul;
	const unsigned long int days_in_100_years = 36524ul;
	const unsigned long int days_in_400_years = 146097ul;
	const unsigned long int num_days_1970_01_01 = 719468ul;

	unsigned long int day_n = num_days_1970_01_01 + unixtime / seconds_per_day;
	unsigned long int seconds_since_0 = unixtime % seconds_per_day;
	unsigned long int temp;

	temp = 4 * (day_n + days_in_100_years + 1) / days_in_400_years - 1;
	*year = 100 * temp;
	day_n -= days_in_100_years * temp + temp / 4;

	temp = 4 * (day_n + days_no_leap_year + 1) / days_in_4_years - 1;
	*year += temp;
	day_n -= days_no_leap_year * temp + temp / 4;

	*month = (5 * day_n + 2) / 153;
	*day = day_n - (*month * 153 + 2) / 5 + 1;

	*month += 3;
	if (*month > 12) {
		*month -= 12;
		++*year;
	}

	*hour = seconds_since_0 / 3600;
	*minute = seconds_since_0 % 3600 / 60;
	*second = seconds_since_0 % 60;
}