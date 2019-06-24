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

#define SL_EXPORTS

#include "socklynx/socklynx.h"

sl_sys_t g_sys = {0};

SL_API int32_t SL_CALL socklynx_setup(void)
{
    return sl_sys_setup(&g_sys);
}

SL_API int32_t SL_CALL socklynx_cleanup(void)
{
    return sl_sys_cleanup(&g_sys);
}

SL_API int32_t SL_CALL socklynx_socket_nonblocking(sl_sock_t *sock, uint32_t enabled)
{
    if (enabled) return sl_sock_nonblocking_set(sock);
    return sl_sock_blocking_set(sock);
}

SL_API int32_t SL_CALL socklynx_socket_open(sl_sock_t *sock)
{
    SL_GUARD_NULL(sock);
    SL_GUARD(sl_sock_create(sock, sl_endpoint_af_get(&sock->endpoint), SL_SOCK_TYPE_DGRAM, SL_SOCK_PROTO_UDP));
    SL_GUARD(sl_sock_bind(sock));
    return SL_OK;
}

SL_API int32_t SL_CALL socklynx_socket_close(sl_sock_t *sock)
{
    SL_GUARD_NULL(sock);
    return sl_sock_close(sock);
}

SL_API int32_t SL_CALL socklynx_socket_send(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint)
{
    SL_GUARD_NULL(sock);
    SL_GUARD_NULL(buf);
    SL_GUARD_NULL(endpoint);
    return sl_sock_send(sock, buf, bufcount, endpoint);
}

SL_API int32_t SL_CALL socklynx_socket_recv(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint)
{
    SL_GUARD_NULL(sock);
    SL_GUARD_NULL(buf);
    SL_GUARD_NULL(endpoint);
    return sl_sock_recv(sock, buf, bufcount, endpoint);
}
