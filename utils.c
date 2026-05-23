#include "utils.h"

long elapsed_ms(struct timespec a) {
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	return (now.tv_sec - a.tv_sec) * 1000L +
		   (now.tv_nsec - a.tv_nsec) / 1000000L;
}
