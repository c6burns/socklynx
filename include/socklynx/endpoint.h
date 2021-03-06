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

#if SL_PLATFORM_OSX
#    define SL_AF_TYPE uint8_t
#else
#    define SL_AF_TYPE uint16_t
#endif

typedef enum sl_sock_af_e {
    SL_SOCK_AF_IPV4 = AF_INET,
    SL_SOCK_AF_IPV6 = AF_INET6,
} sl_sock_af_t;

typedef struct sl_sockaddr4_s {
#if SL_PLATFORM_OSX
    uint8_t len;
#endif
    SL_AF_TYPE af;
    uint16_t port;
    uint32_t addr;
    uint8_t pad[8];
} sl_sockaddr4_t;

typedef struct sl_sockaddr6_s {
#if SL_PLATFORM_OSX
    uint8_t len;
#endif
    SL_AF_TYPE af;
    uint16_t port;
    uint32_t flowinfo;
    uint8_t addr[16];
    uint32_t scope_id;
} sl_sockaddr6_t;

typedef union sl_endpoint_u {
    sl_sockaddr4_t addr4;
    sl_sockaddr6_t addr6;
} sl_endpoint_t;

SL_INLINE_IMPL uint16_t sl_endpoint_af_get(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (uint16_t)endpoint->addr4.af;
}

SL_INLINE_IMPL void sl_endpoint_af_set(sl_endpoint_t *endpoint, sl_sock_af_t af)
{
    SL_ASSERT(endpoint);
    endpoint->addr4.af = (SL_AF_TYPE)af;
}

SL_INLINE_IMPL struct sockaddr *sl_endpoint_addr_get(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (struct sockaddr *)endpoint;
}

SL_INLINE_IMPL bool sl_endpoint_is_ipv4(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (sl_endpoint_af_get(endpoint) == (uint16_t)SL_SOCK_AF_IPV4);
}

SL_INLINE_IMPL bool sl_endpoint_is_ipv6(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    return (sl_endpoint_af_get(endpoint) == (uint16_t)SL_SOCK_AF_IPV6);
}

SL_INLINE_IMPL int sl_endpoint_size(sl_endpoint_t *endpoint)
{
    SL_ASSERT(endpoint);
    if (sl_endpoint_is_ipv4(endpoint)) return sizeof(sl_sockaddr4_t);
    if (sl_endpoint_is_ipv6(endpoint)) return sizeof(sl_sockaddr6_t);
    return SL_ERR;
}

#endif
