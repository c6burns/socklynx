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

#if SL_PLATFORM_WINDOWS
#   include <winsock2.h>
#	include <ws2ipdef.h>
#else
#	include <sys/socket.h>
#	include <netinet/ip.h>
#endif

union sl_endpoint_u {
	struct sockaddr_in addr4;
	struct sockaddr_in6 addr6;
};

typedef union sl_endpoint_u sl_endpoint_t;

SL_INLINE int sl_endpoint_size(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	switch (endpoint->addr4.sin_family) {
	case AF_INET:
		return sizeof(struct sockaddr_in);
	case AF_INET6:
		return sizeof(struct sockaddr_in6);
	}
	return SL_ERR;
}

SL_INLINE bool sl_endpoint_is4(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return (endpoint->addr4.sin_family == AF_INET);
}

SL_INLINE struct sockaddr_in *sl_endpoint_get4(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	SL_ASSERT(sl_endpoint_is4(endpoint));
	return &endpoint->addr4;
}

SL_INLINE bool sl_endpoint_is6(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return (endpoint->addr4.sin_family == AF_INET6);
}

SL_INLINE struct sockaddr_in6 *sl_endpoint_get6(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	SL_ASSERT(sl_endpoint_is6(endpoint));
	return &endpoint->addr6;
}

#endif
