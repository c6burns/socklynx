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
	struct sockaddr_in addr;
	struct sockaddr_in6 addr6;
};

typedef union socklynx_endpoint_u socklynx_endpoint_t;

SL_INLINE bool socklynx_endpoint_is4(socklynx_endpoint_t *endpoint)
{
	return (endpoint->addr.sin_family == AF_INET);
}

SL_INLINE bool socklynx_endpoint_is6(socklynx_endpoint_t *endpoint)
{
	return (endpoint->addr.sin_family == AF_INET6);
}

#endif
