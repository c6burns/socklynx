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

#include "socklynx/endpoint.h"

#include "socklynx/sock.h"


SL_INLINE_IMPL uint16_t sl_endpoint_af_get(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return endpoint->addr4.sin_family;
}

SL_INLINE_IMPL int sl_endpoint_af_set(sl_endpoint_t *endpoint, uint16_t af)
{
	SL_ASSERT(endpoint);
    switch (af) {
    case SL_SOCK_AF_IPV4:
    case SL_SOCK_AF_IPV6:
        endpoint->addr4.sin_family = af;
        return SL_OK;
    }
	return SL_ERR;
}

SL_INLINE_DECL struct sockaddr *sl_endpoint_addr_get(sl_endpoint_t *endpoint)
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
	if (sl_endpoint_is_ipv4(endpoint)) return sizeof(sockaddr4);
	if (sl_endpoint_is_ipv6(endpoint)) return sizeof(sockaddr6);
	return SL_ERR;
}
