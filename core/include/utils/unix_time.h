#pragma once

long long to_unix_time(int year, int month, int day, int hour, int minute, int second);
void from_unix_time(unsigned long int unixtime, int* year, int* month, int* day, int* hour, int* minute, int* second);