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

#ifndef SL_ENDPOINT_H
#define SL_ENDPOINT_H

#include "socklynx/common.h"
#include "socklynx/error.h"

#if SL_PLATFORM_WINDOWS || SL_PLATFORM_XBONE
#	include <winsock2.h>
#	include <ws2ipdef.h>
#else
#	include <unistd.h>
#	include <fcntl.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <netinet/ip.h>
#endif

typedef struct sockaddr_in sockaddr4;
typedef struct sockaddr_in6 sockaddr6;

typedef union sl_endpoint_u {
	sockaddr4 addr4;
	sockaddr6 addr6;
} sl_endpoint_t;


SL_INLINE_DECL uint16_t sl_endpoint_af_get(sl_endpoint_t *endpoint);
SL_INLINE_DECL int sl_endpoint_af_set(sl_endpoint_t *endpoint, uint16_t af);
SL_INLINE_DECL struct sockaddr *sl_endpoint_addr_get(sl_endpoint_t *endpoint);
SL_INLINE_DECL bool sl_endpoint_is_ipv4(sl_endpoint_t *endpoint);
SL_INLINE_DECL bool sl_endpoint_is_ipv6(sl_endpoint_t *endpoint);
SL_INLINE_DECL int sl_endpoint_size(sl_endpoint_t *endpoint);


#endif
