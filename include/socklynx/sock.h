#ifndef UDPP_BUF_H
#define UDPP_BUF_H

#include "platform.h"

#include <stdint.h>

#if UDPP_PLATFORM_WINDOWS
	typedef struct udpp_buf_s {
		char *base;
		uint32_t len;
	} udpp_buf_t;
#else
	typedef struct udpp_buf_s {
		char *base;
		size_t len;
	} udpp_buf_t;
#endif

#endif
