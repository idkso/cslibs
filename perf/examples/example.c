#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "perf.h"

int main(void) {
    struct perf_ctx ctx1, ctx2;
	struct perf_res res1, res2;

	int hw = perf_init(&ctx1, PERF_HW, CPU_CYCLES | INSTRUCTIONS | CACHE_REFERENCES | CACHE_MISSES, EXCLUDE_DEFAULT);
	int sw = perf_init(&ctx2, PERF_SW, PAGE_FAULTS | CONTEXT_SWITCHES, EXCLUDE_DEFAULT);

	if (hw == -1 || sw == -1) {
		fprintf(stderr, "error: %s\n", strerror(errno));
		exit(1);
	}
	
	perf_reset(sw);
	perf_reset(hw);
	
	perf_start(sw);
	perf_start(hw);

	char *ptr = malloc(4096);

	memset(ptr, 'F', 4096);
	
	printf("hello this is an example printf\n");

	int fd = open("file.txt", O_CREAT | O_RDWR, 0777);

	write(fd, ptr, 4096);
	
	close(fd);
	
	perf_stop(hw);
	perf_stop(sw);

	perf_read(hw, &ctx1, &res1);
	perf_read(sw, &ctx2, &res2);

	printf("cpu cycles: %ld\n", res1.cpu_cycles);
	printf("instructions: %ld\n", res1.instructions);
	printf("cache references: %ld\n", res1.cache_references);
	printf("cache misses: %ld\n", res1.cache_misses);
	printf("page faults: %ld\n", res2.page_faults);
	printf("context switches: %ld\n", res2.context_switches);
	
	perf_deinit(&ctx1);
	perf_deinit(&ctx2);
    return 0;
}
