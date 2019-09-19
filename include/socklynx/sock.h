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

#include "socklynx/buf.h"
#include "socklynx/common.h"
#include "socklynx/endpoint.h"
#include "socklynx/error.h"
#include "socklynx/sys.h"

#include <memory.h>

typedef enum sl_sock_state_e {
    SL_SOCK_STATE_NEW,
    SL_SOCK_STATE_CREATED,
    SL_SOCK_STATE_BOUND,
    SL_SOCK_STATE_OPEN,
    SL_SOCK_STATE_CLOSED,
    SL_SOCK_STATE_ERROR,
} sl_sock_state_t;

typedef enum sl_sock_dir_e {
    SL_SOCK_DIR_NONE,
    SL_SOCK_DIR_INCOMING,
    SL_SOCK_DIR_OUTGOING,
} sl_sock_dir_t;

typedef enum sl_sock_type_e {
    SL_SOCK_TYPE_DGRAM = SOCK_DGRAM,
    SL_SOCK_TYPE_STREAM = SOCK_STREAM,
} sl_sock_type_t;

typedef enum sl_sock_proto_e {
    SL_SOCK_PROTO_UDP = IPPROTO_UDP,
    SL_SOCK_PROTO_TCP = IPPROTO_TCP,
} sl_sock_proto_t;

typedef enum sl_sock_flag_e {
    SL_SOCK_FLAG_NONBLOCKING = (1 << 0),
    SL_SOCK_FLAG_WOULDBLOCK_READ = (1 << 1),
    SL_SOCK_FLAG_WOULDBLOCK_WRITE = (1 << 2),
    SL_SOCK_FLAG_IPV4_DISABLED = (1 << 3),
    SL_SOCK_FLAG_IPV6_DISABLED = (1 << 4),
} sl_sock_flag_t;

typedef struct sl_sock_s {
    int64_t fd;
    uint32_t dir;
    uint32_t state;
    uint32_t type;
    uint32_t proto;
    uint32_t error;
    uint32_t flags;
    sl_endpoint_t endpoint;
} sl_sock_t;

SL_INLINE_IMPL void sl_sock_error_set(sl_sock_t *sock, uint32_t error)
{
    SL_ASSERT(sock);
    sock->error = error;
}

SL_INLINE_IMPL SL_SOCK_TYPE sl_sock_fd_get(sl_sock_t *sock)
{
    SL_ASSERT(sock);
    return (SL_SOCK_TYPE)sock->fd;
}

SL_INLINE_IMPL int sl_sock_fd_set(sl_sock_t *sock, int64_t sockfd)
{
    SL_ASSERT(sock);

#if SL_SOCK_API_WINSOCK
    if (sockfd == INVALID_SOCKET) {
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
#else
    if (sockfd < 0) {
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
#endif
    sock->fd = sockfd;

    return SL_OK;
}

SL_INLINE_IMPL void sl_sock_dir_set(sl_sock_t *sock, sl_sock_dir_t dir)
{
    SL_ASSERT(sock);
    sock->dir = dir;
}

SL_INLINE_IMPL void sl_sock_state_set(sl_sock_t *sock, sl_sock_state_t state)
{
    SL_ASSERT(sock);
    sock->state = state;
}

SL_INLINE_IMPL void sl_sock_flags_set(sl_sock_t *sock, sl_sock_flag_t flags)
{
    SL_ASSERT(sock);
    sock->flags |= flags;
}

SL_INLINE_IMPL void sl_sock_flags_unset(sl_sock_t *sock, sl_sock_flag_t flags)
{
    SL_ASSERT(sock);
    sock->flags &= ~flags;
}

SL_INLINE_IMPL void sl_sock_flags_clear(sl_sock_t *sock)
{
    SL_ASSERT(sock);
    sock->flags = 0;
}

SL_INLINE_IMPL void sl_sock_type_set(sl_sock_t *sock, sl_sock_type_t type)
{
    SL_ASSERT(sock);
    sock->type = type;
}

SL_INLINE_IMPL void sl_sock_proto_set(sl_sock_t *sock, sl_sock_proto_t proto)
{
    SL_ASSERT(sock);
    sock->proto = proto;
}

SL_INLINE_IMPL int sl_sock_create(sl_sock_t *sock, sl_sock_type_t type, sl_sock_proto_t proto)
{
    SL_ASSERT(sock);
    SL_ASSERT(sock->state == SL_SOCK_STATE_NEW || sock->state == SL_SOCK_STATE_CLOSED);

    sl_sock_type_set(sock, type);
    sl_sock_proto_set(sock, proto);
    SL_GUARD(sl_sock_fd_set(sock, socket(sl_endpoint_af_get(&sock->endpoint), type, proto)));
    if (sl_endpoint_is_ipv6(&sock->endpoint)) {
        int optval = 0;
        SL_GUARD(setsockopt(sl_sock_fd_get(sock), IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&optval, sizeof(optval)));
        sl_sock_flags_set(sock, SL_SOCK_FLAG_IPV4_DISABLED);
    }
    sl_sock_state_set(sock, SL_SOCK_STATE_CREATED);

    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_close(sl_sock_t *sock)
{
    SL_ASSERT(sock);

#if SL_SOCK_API_WINSOCK
    if (closesocket(sl_sock_fd_get(sock))) {
#else
    if (close(sl_sock_fd_get(sock))) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
    SL_GUARD(sl_sock_fd_set(sock, 0));
    sl_sock_state_set(sock, SL_SOCK_STATE_CLOSED);
    sl_sock_flags_clear(sock);

    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_bind(sl_sock_t *sock)
{
    SL_ASSERT(sock);
    SL_ASSERT(sock->state == SL_SOCK_STATE_CREATED);

#if SL_SOCK_API_WINSOCK
    if (bind(sl_sock_fd_get(sock), sl_endpoint_addr_get(&sock->endpoint), sl_endpoint_size(&sock->endpoint))) {
#else
    if (bind(sl_sock_fd_get(sock), sl_endpoint_addr_get(&sock->endpoint), (socklen_t)sl_endpoint_size(&sock->endpoint))) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
    sl_sock_state_set(sock, SL_SOCK_STATE_BOUND);

    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_blocking_set(sl_sock_t *sock)
{
    SL_ASSERT(sock);

#if SL_SOCK_API_WINSOCK
    uint32_t argp = 0;
    if (ioctlsocket(sl_sock_fd_get(sock), FIONBIO, &argp)) {
#else
    if (fcntl(sl_sock_fd_get(sock), F_SETFL, ~O_NONBLOCK & fcntl(sl_sock_fd_get(sock), F_GETFL, 0))) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
    sl_sock_flags_unset(sock, SL_SOCK_FLAG_NONBLOCKING);

    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_nonblocking_set(sl_sock_t *sock)
{
    SL_ASSERT(sock);

#if SL_SOCK_API_WINSOCK
    uint32_t argp = 1;
    if (ioctlsocket(sl_sock_fd_get(sock), FIONBIO, &argp)) {
#else
    if (fcntl(sl_sock_fd_get(sock), F_SETFL, O_NONBLOCK | fcntl(sl_sock_fd_get(sock), F_GETFL, 0))) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
    sl_sock_flags_set(sock, SL_SOCK_FLAG_NONBLOCKING);

    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_send(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint)
{
    SL_ASSERT(sock);
    SL_ASSERT(buf && bufcount > 0);
    SL_ASSERT(endpoint);
    SL_ASSERT(sock->state == SL_SOCK_STATE_BOUND);

    int64_t bytes_sent;
    struct sockaddr *sa = sl_endpoint_addr_get(endpoint);
    const int sa_len = sl_endpoint_size(endpoint);
#if SL_SOCK_API_WINSOCK
    if (WSASendTo(sl_sock_fd_get(sock), (LPWSABUF)buf, (DWORD)bufcount, (LPDWORD)&bytes_sent, 0, sa, sa_len, NULL, NULL))
    {
#else
    struct msghdr mhdr = {0};
    mhdr.msg_name = sa;
    mhdr.msg_namelen = sa_len;
    mhdr.msg_iov = (struct iovec *)buf;
    mhdr.msg_iovlen = (size_t)bufcount;
    if ((bytes_sent = (ssize_t)sendmsg(sl_sock_fd_get(sock), &mhdr, 0)) < 0) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }

    return (int)bytes_sent;
}

SL_INLINE_IMPL int sl_sock_recv(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint)
{
    SL_ASSERT(sock);
    SL_ASSERT(buf && bufcount);
    SL_ASSERT(endpoint);
    SL_ASSERT(sock->state == SL_SOCK_STATE_BOUND);

    int64_t bytes_recv;
    int32_t epsize = sizeof(*endpoint);
#if SL_SOCK_API_WINSOCK
    DWORD flags = 0;
    if (WSARecvFrom(sl_sock_fd_get(sock), (LPWSABUF)buf, (DWORD)bufcount, (LPDWORD)&bytes_recv, (LPDWORD)&flags, sl_endpoint_addr_get(endpoint), &epsize, NULL, NULL)) {
#else
    struct msghdr mhdr = {0};
    mhdr.msg_name = endpoint;
    mhdr.msg_namelen = (socklen_t)epsize;
    mhdr.msg_iov = (struct iovec *)buf;
    mhdr.msg_iovlen = (size_t)bufcount;
    if ((bytes_recv = (int64_t)recvmsg(sl_sock_fd_get(sock), &mhdr, 0)) < 0) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }

    return (int)bytes_recv;
}

#endif
