#ifndef SL_BUF_H
#define SL_BUF_H

/*
 * Copyright 2019 Chris Burns <chris@kitty.city>
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

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
