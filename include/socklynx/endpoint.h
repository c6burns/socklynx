#ifndef SL_ENDPOINT_H
#define SL_ENDPOINT_H

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
