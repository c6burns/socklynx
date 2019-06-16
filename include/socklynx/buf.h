#ifndef SL_BUF_H
#define SL_BUF_H

#include "common.h"

#if SL_PLATFORM_WINDOWS

typedef struct socklynx_buf_s {
	uint32_t len;
	char *base;
} socklynx_buf_t;

#else

typedef struct socklynx_buf_s {
	char *base;
	size_t len;
} socklynx_buf_t;

#endif

#if SL_64
SL_STATIC_ASSERT(sizeof(socklynx_buf_t) == 16);
#else
SL_STATIC_ASSERT(sizeof(socklynx_buf_t) == 8);
#endif

#endif
