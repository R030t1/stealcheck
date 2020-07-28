#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <string>
#include <regex>
#include <iostream>
#include <fstream>
using namespace std;

uint64_t cpufreq();

int main(int argc, char *argv[]) {
	// If you have a newer processor you can request
	// cpuid level 0x16. For this impl. libpcre is
	// likely faster.
	uint64_t cf = cpufreq(),
	// Six is familiar but likely not right.
		cycles_per_loop = 6;

	struct timespec start = { 0 };
	clock_gettime(CLOCK_REALTIME, &start);

	// Confirm the cycle count of these instructions for
	// accurate results and/or implement loop with asm.
	uint64_t count = 0x10000000, orig = 0x10000000;
	while (count--);

	struct timespec end = { 0 };
	clock_gettime(CLOCK_REALTIME, &end);
	// Calculate delta.
	end.tv_sec  -= start.tv_sec;
	end.tv_nsec -= start.tv_nsec;

	double real = (end.tv_sec * 1.0) + (end.tv_nsec / 1000000000.0);
	double expected = (1.0 / cf) * orig * cycles_per_loop;
	printf("real\t\t%lfs\n", real);
	printf("expected\t%lfs\n", expected);

	return 0;
}

uint64_t cpufreq() {
	uint64_t res = 0;
	regex pattern("^cpu MHz.*?([\\d.]+)");
	smatch glean;

	string line;
	ifstream cpuinf("/proc/cpuinfo");
	while (getline(cpuinf, line)) {
		if (!regex_search(line, glean, pattern))
			continue;
		// This effectively returns the last one, but I didn't
		// want to add CPU pinning etc. They are typically close
		// together.
		res = stod(glean[1].str()) * 1000000;
	}

	return res;
}
