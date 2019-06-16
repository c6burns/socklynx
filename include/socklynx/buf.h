#ifndef SL_BUF_H
#define SL_BUF_H

#include "common.h"

#if SL_PLATFORM_WINDOWS

typedef struct sl_buf_s {
	uint32_t len;
	char *base;
} sl_buf_t;

#else

typedef struct sl_buf_s {
	char *base;
	size_t len;
} sl_buf_t;

#endif

#if SL_64
SL_STATIC_ASSERT(sizeof(sl_buf_t) == 16);
#else
SL_STATIC_ASSERT(sizeof(sl_buf_t) == 8);
#endif

#endif
