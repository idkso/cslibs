#define _GNU_SOURCE
#include "perf.h"
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#define BOOL(x) !!(x)

static size_t
perf_len(struct perf_ctx *ctx)
{
  size_t ret = 0;
  ret += sizeof(uint64_t);
  ret += (sizeof(uint64_t) * 2) * ctx->len;
  return ret;
}

int
perf_init(struct perf_ctx *ctx, uint8_t type, uint8_t types,
              uint8_t exclude)
{
  int fd1 = -1, fd2;
  struct perf_event_attr pe = {0};

  memset(ctx, 0, sizeof(*ctx));

  ctx->type = type;

  pe.size = sizeof(pe);
  pe.disabled = 1;
  pe.exclude_kernel = BOOL(exclude & EXCLUDE_KERNEL);
  pe.exclude_hv = BOOL(exclude & EXCLUDE_HV);
  pe.exclude_user = BOOL(exclude & EXCLUDE_USER);
  pe.exclude_idle = BOOL(exclude & EXCLUDE_IDLE);
  pe.exclude_host = BOOL(exclude & EXCLUDE_HOST);
  pe.exclude_guest = BOOL(exclude & EXCLUDE_GUEST);
  pe.exclude_callchain_kernel = BOOL(exclude & EXCLUDE_CALLCHAIN_KERNEL);
  pe.exclude_callchain_user = BOOL(exclude & EXCLUDE_CALLCHAIN_USER);
  pe.read_format = PERF_FORMAT_GROUP | PERF_FORMAT_ID;

  pe.type = PERF_TYPE_HARDWARE;

#define SET(t)                                                                 \
  do {                                                                         \
    int fd, ret;                                                               \
    pe.config = t;                                                             \
    if (fd1 == -1) {                                                           \
      fd1 = syscall(SYS_perf_event_open, &pe, 0, -1, -1, 0);                   \
      fd = fd1;                                                                \
    } else {                                                                   \
      fd2 = syscall(SYS_perf_event_open, &pe, 0, -1, fd1, 0);                  \
      fd = fd2;                                                                \
    }                                                                          \
    if (fd == -1)                                                              \
      return -1;                                                               \
    ret = ioctl(fd, PERF_EVENT_IOC_ID, ctx->ids + ctx->len);                   \
    if (ret == -1)                                                             \
      return -1;                                                               \
    ctx->types[ctx->len] = t;                                                  \
    ctx->fds[ctx->len] = fd;                                                   \
    ctx->len++;                                                                \
  } while (0);

  if (type == PERF_SW)
    pe.type = PERF_TYPE_SOFTWARE;

  if (BOOL(types & CPU_CYCLES))
    SET(PERF_COUNT_HW_CPU_CYCLES);
  if (BOOL(types & INSTRUCTIONS))
    SET(PERF_COUNT_HW_INSTRUCTIONS);
  if (BOOL(types & CACHE_REFERENCES))
    SET(PERF_COUNT_HW_CACHE_REFERENCES);
  if (BOOL(types & CACHE_MISSES))
    SET(PERF_COUNT_HW_CACHE_MISSES);
  if (BOOL(types & BRANCH_INSTRUCTIONS))
    SET(PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
  if (BOOL(types & BRANCH_MISSES))
    SET(PERF_COUNT_HW_BRANCH_MISSES);
  if (BOOL(types & BUS_CYCLES))
    SET(PERF_COUNT_HW_BUS_CYCLES);

#undef SET
  return fd1;
}

/* returns -1 for error */
int
perf_read(int fd, struct perf_ctx *ctx, struct perf_res *res)
{
  struct perf_buf buf;
  if (read(fd, &buf, perf_len(ctx)) == -1)
    return -1;
  for (uint64_t i = 0; i < buf.len; i++) {
    for (uint64_t j = 0; j < ctx->len; j++) {
      if (buf.res[i].id != ctx->ids[j])
        continue;

      switch (ctx->types[j]) {
      case PERF_COUNT_HW_CPU_CYCLES:
        res->cpu_cycles = buf.res[i].value;
        break;
      case PERF_COUNT_HW_INSTRUCTIONS:
        res->instructions = buf.res[i].value;
        break;
      case PERF_COUNT_HW_CACHE_REFERENCES | PERF_COUNT_SW_PAGE_FAULTS:
        if (ctx->type == PERF_HW)
          res->cache_references = buf.res[i].value;
        else
          res->page_faults = buf.res[i].value;
        break;
      case PERF_COUNT_HW_CACHE_MISSES | PERF_COUNT_SW_CONTEXT_SWITCHES:
        if (ctx->type == PERF_HW)
          res->cache_misses = buf.res[i].value;
        else
          res->context_switches = buf.res[i].value;
        break;
      case PERF_COUNT_HW_BRANCH_INSTRUCTIONS | PERF_COUNT_SW_CPU_MIGRATIONS:
        if (ctx->type == PERF_HW)
          res->branch_instructions = buf.res[i].value;
        else
          res->cpu_migrations = buf.res[i].value;
        break;
      case PERF_COUNT_HW_BRANCH_MISSES:
        res->branch_misses = buf.res[i].value;
        break;
      case PERF_COUNT_HW_BUS_CYCLES:
        res->bus_cycles = buf.res[i].value;
        break;
      }
    }
  }

  return 0;
}

void
perf_deinit(struct perf_ctx *ctx)
{
  for (uint64_t i = 0; i < ctx->len; i++) {
    close(ctx->fds[i]);
  }
}
