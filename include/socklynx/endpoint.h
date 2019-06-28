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
#    include <winsock2.h>
#    include <ws2ipdef.h>
#else
#    include <fcntl.h>
#    include <netinet/in.h>
#    include <netinet/ip.h>
#    include <sys/socket.h>
#    include <unistd.h>
#endif

/* sock address family */
#define SL_MANAGED_AF_IPV4 12340
#define SL_MANAGED_AF_IPV6 12341
#define SL_SOCK_AF_IPV4 AF_INET
#define SL_SOCK_AF_IPV6 AF_INET6

typedef struct sl_sockaddr4_s {
	uint16_t af;
	uint16_t port;
	uint32_t addr;
	uint8_t pad[8];
} sl_sockaddr4_t;

typedef struct sl_sockaddr6_s {
	uint16_t af;
	uint16_t port;
	uint32_t flowinfo;
	uint8_t addr[16];
	uint32_t scope_id;
} sl_sockaddr6_t;

typedef union sl_endpoint_u {
    sl_sockaddr4_t addr4;
    sl_sockaddr6_t addr6;
} sl_endpoint_t;

SL_INLINE_IMPL int sl_endpoint_af_check(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    switch (endpoint->addr4.af) {
    case SL_MANAGED_AF_IPV4:
        endpoint->addr4.af = SL_SOCK_AF_IPV4;
        return SL_OK;
    case SL_MANAGED_AF_IPV6:
        endpoint->addr4.af = SL_SOCK_AF_IPV6;
        return SL_OK;
    case SL_SOCK_AF_IPV4:
    case SL_SOCK_AF_IPV6:
        return SL_OK;
    }
    return SL_ERR;
}

SL_INLINE_IMPL uint16_t sl_endpoint_af_get(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
	/* this should mask out anything weird (eg. apple using the high byte for len) */
    return (endpoint->addr4.af & 0x0000ffff);
}

SL_INLINE_IMPL int sl_endpoint_af_set(sl_endpoint_t *endpoint, uint16_t af)
{
    SL_ASSERT(endpoint);
    switch (af) {
    case SL_MANAGED_AF_IPV4:
        endpoint->addr4.af = SL_SOCK_AF_IPV4;
        return SL_OK;
    case SL_MANAGED_AF_IPV6:
        endpoint->addr4.af = SL_SOCK_AF_IPV6;
        return SL_OK;
    case SL_SOCK_AF_IPV4:
    case SL_SOCK_AF_IPV6:
        endpoint->addr4.af = af;
        return SL_OK;
    }
    return SL_ERR;
}

SL_INLINE_IMPL struct sockaddr *sl_endpoint_addrany_set(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (struct sockaddr *)endpoint;
}

SL_INLINE_IMPL struct sockaddr *sl_endpoint_addr_get(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (struct sockaddr *)endpoint;
}

SL_INLINE_IMPL bool sl_endpoint_is_ipv4(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (sl_endpoint_af_get(endpoint) == SL_SOCK_AF_IPV4);
}

SL_INLINE_IMPL bool sl_endpoint_is_ipv6(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (sl_endpoint_af_get(endpoint) == SL_SOCK_AF_IPV6);
}

SL_INLINE_IMPL int sl_endpoint_size(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    if (sl_endpoint_is_ipv4(endpoint)) return sizeof(sl_sockaddr4_t);
    if (sl_endpoint_is_ipv6(endpoint)) return sizeof(sl_sockaddr6_t);
    return SL_ERR;
}

#endif
