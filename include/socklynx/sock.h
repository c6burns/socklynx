/*
 * Copyright (c) 2019 Chris Burns <chris@kitty.city>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SL_SOCK_H
#define SL_SOCK_H

#include "socklynx/common.h"
#include "socklynx/sys.h"
#include "socklynx/endpoint.h"

#include <memory.h>

#if SL_PLATFORM_WINDOWS || SL_PLATFORM_XBONE
#	include <winsock2.h>
#	include <ws2ipdef.h>
#else
#	include <unistd.h>
#	include <fcntl.h>
#	include <sys/socket.h>
#endif

/* sock state */
#define SL_SOCK_STATE_NEW 0
#define SL_SOCK_STATE_CREATED 1
#define SL_SOCK_STATE_BOUND 2
#define SL_SOCK_STATE_OPEN 3
#define SL_SOCK_STATE_CLOSED 4
#define SL_SOCK_STATE_ERROR 5

/* sock direction */
#define SL_SOCK_DIR_NONE 0
#define SL_SOCK_DIR_INCOMING 1
#define SL_SOCK_DIR_OUTGOING 2

/* sock address family */
#define SL_SOCK_AF_IPV4 AF_INET
#define SL_SOCK_AF_IPV6 AF_INET6

/* sock type */
#define SL_SOCK_TYPE_DGRAM SOCK_DGRAM
#define SL_SOCK_TYPE_STREAM SOCK_STREAM

/* sock proto */
#define SL_SOCK_PROTO_UDP IPPROTO_UDP
#define SL_SOCK_PROTO_TCP IPPROTO_TCP

typedef struct sl_sock_s {
	int64_t fd;
	uint32_t dir;
	uint32_t state;
	uint32_t type;
	uint32_t proto;
	uint32_t err;
	sl_endpoint_t endpoint;
} sl_sock_t;


SL_INLINE_DECL int sl_sock_create(sl_sock_t *sock, uint32_t af, uint32_t type, uint32_t proto);


#endif
