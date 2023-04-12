#ifndef PERF_LIB_H
#define PERF_LIB_H
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <linux/perf_event.h>
#include <sys/ioctl.h>

struct perf_res {
	uint64_t cpu_cycles;
	uint64_t instructions;
	uint64_t cache_references;
	uint64_t cache_misses;
	uint64_t branch_instructions;
	uint64_t branch_misses;
	uint64_t bus_cycles;
	uint64_t page_faults;
	uint64_t context_switches;
	uint64_t cpu_migrations;
};

struct perf_ctx {
	size_t len;
	uint8_t type;
	uint8_t types[8];
	int fds[8];
	uint64_t ids[8];
};

struct perf_buf {
	uint64_t len;
	struct {
		uint64_t value;
		uint64_t id;
	} res[16];
};

enum perf_types {
	CPU_CYCLES = (1 << 0),
	INSTRUCTIONS = (1 << 1),
	CACHE_REFERENCES = (1 << 2),
	CACHE_MISSES = (1 << 3),
	BRANCH_INSTRUCTIONS = (1 << 4),
	BRANCH_MISSES = (1 << 5),
	BUS_CYCLES = (1 << 6),
	PAGE_FAULTS = CACHE_REFERENCES,
	CONTEXT_SWITCHES = CACHE_MISSES,
	CPU_MIGRATIONS = BRANCH_INSTRUCTIONS,
	ALL_TYPES = 255,
};

enum perf_type {
	PERF_HW,
	PERF_SW,
};

enum perf_exclude {
	EXCLUDE_KERNEL = (1 << 0),
	EXCLUDE_HV = (1 << 1),
	EXCLUDE_USER = (1 << 2),
	EXCLUDE_IDLE = (1 << 3),
	EXCLUDE_HOST = (1 << 4),
	EXCLUDE_GUEST = (1 << 5),
	EXCLUDE_CALLCHAIN_KERNEL = (1 << 6),
	EXCLUDE_CALLCHAIN_USER = (1 << 7),
	EXCLUDE_DEFAULT = EXCLUDE_KERNEL | EXCLUDE_HV,
	EXCLUDE_ALL = 256,
};

// returns fd or -1 for error, must perf_deinit after use
int perf_init(struct perf_ctx *ctx, uint8_t type, uint8_t types, uint8_t disable);
// returns -1 for error
int perf_read(int fd, struct perf_ctx *ctx, struct perf_res *res);
void perf_deinit(struct perf_ctx *ctx);

#define perf_reset(fd) ioctl(fd, PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
#define perf_start(fd) ioctl(fd, PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
#define perf_stop(fd) ioctl(fd, PERF_EVENT_IOC_DISABLE, PERF_IOC_FLAG_GROUP);
#endif
