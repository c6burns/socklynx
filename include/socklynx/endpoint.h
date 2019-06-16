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

union socklynx_endpoint_u {
	struct sockaddr_in addr4;
	struct sockaddr_in6 addr6;
};

typedef union socklynx_endpoint_u socklynx_endpoint_t;

SL_INLINE int socklynx_endpoint_size(socklynx_endpoint_t *endpoint)
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

SL_INLINE bool socklynx_endpoint_is4(socklynx_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return (endpoint->addr4.sin_family == AF_INET);
}

SL_INLINE struct sockaddr_in *socklynx_endpoint_get4(socklynx_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	SL_ASSERT(socklynx_endpoint_is4(endpoint));
	return &endpoint->addr4;
}

SL_INLINE bool socklynx_endpoint_is6(socklynx_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return (endpoint->addr4.sin_family == AF_INET6);
}

SL_INLINE struct sockaddr_in6 *socklynx_endpoint_get6(socklynx_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	SL_ASSERT(socklynx_endpoint_is6(endpoint));
	return &endpoint->addr6;
}

#endif
