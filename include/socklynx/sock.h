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

#if SL_SOCK_API_WINSOCK
#    include <winsock2.h>
#    include <ws2ipdef.h>
#else
#    include <fcntl.h>
#    include <sys/socket.h>
#    include <unistd.h>
#endif

/* sock state */
#define SL_SOCK_STATE_NEW 0
#define SL_SOCK_STATE_CREATED 1
#define SL_SOCK_STATE_BOUND 2
#define SL_SOCK_STATE_OPEN 3
#define SL_SOCK_STATE_CLOSED 4
#define SL_SOCK_STATE_ERROR 5

/* sock direction */
#define SL_SOCK_DIR_NONE 0
#define SL_SOCK_DIR_INCOMING 1
#define SL_SOCK_DIR_OUTGOING 2

/* sock type */
#define SL_SOCK_TYPE_DGRAM SOCK_DGRAM
#define SL_SOCK_TYPE_STREAM SOCK_STREAM

/* sock proto */
#define SL_SOCK_PROTO_UDP IPPROTO_UDP
#define SL_SOCK_PROTO_TCP IPPROTO_TCP

typedef struct sl_sock_s {
    int64_t fd;
    uint32_t dir;
    uint32_t state;
    uint32_t type;
    uint32_t proto;
    uint32_t error;
    sl_endpoint_t endpoint;
} sl_sock_t;

SL_INLINE_IMPL int sl_sock_error_set(sl_sock_t *sock, uint32_t error)
{
    SL_ASSERT(sock);
    sock->error = error;
    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_fd_set(sl_sock_t *sock, int64_t sockfd)
{
    SL_ASSERT(sock);
#if SL_SOCK_API_WINSOCK
    if (sockfd == INVALID_SOCKET) {
        sl_sock_error_set(sock, sl_sys_errno());
        switch (sock->error) {
        case WSANOTINITIALISED:
            break;
        case WSAENETDOWN:
            break;
        case WSAEAFNOSUPPORT:
            break;
        case WSAEINPROGRESS:
            break;
        case WSAEMFILE:
            break;
        case WSAEINVAL:
            break;
        case WSAEINVALIDPROVIDER:
            break;
        case WSAEINVALIDPROCTABLE:
            break;
        case WSAENOBUFS:
            break;
        case WSAEPROTONOSUPPORT:
            break;
        case WSAEPROTOTYPE:
            break;
        case WSAEPROVIDERFAILEDINIT:
            break;
        case WSAESOCKTNOSUPPORT:
            break;

        }
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

SL_INLINE_IMPL int sl_sock_dir_set(sl_sock_t *sock, uint32_t dir)
{
    SL_ASSERT(sock);
    switch (dir) {
    case SL_SOCK_DIR_INCOMING:
    case SL_SOCK_DIR_OUTGOING:
        sock->dir = dir;
        return SL_OK;
    }
    return SL_ERR;
}

SL_INLINE_IMPL int sl_sock_state_set(sl_sock_t *sock, uint32_t state)
{
    SL_ASSERT(sock);
    switch (state) {
    case SL_SOCK_STATE_NEW:
    case SL_SOCK_STATE_CREATED:
    case SL_SOCK_STATE_BOUND:
    case SL_SOCK_STATE_OPEN:
    case SL_SOCK_STATE_CLOSED:
    case SL_SOCK_STATE_ERROR:
        sock->state = state;
        return SL_OK;
    }
    return SL_ERR;
}

SL_INLINE_IMPL int sl_sock_type_set(sl_sock_t *sock, uint32_t type)
{
    SL_ASSERT(sock);
    switch (type) {
    case SL_SOCK_TYPE_DGRAM:
    case SL_SOCK_TYPE_STREAM:
        sock->type = type;
        return SL_OK;
    }
    return SL_ERR;
}

SL_INLINE_IMPL int sl_sock_proto_set(sl_sock_t *sock, uint32_t proto)
{
    SL_ASSERT(sock);
    switch (proto) {
    case SL_SOCK_PROTO_UDP:
    case SL_SOCK_PROTO_TCP:
        sock->proto = proto;
        return SL_OK;
    }
    return SL_ERR;
}

SL_INLINE_IMPL int sl_sock_create(sl_sock_t *sock, uint32_t type, uint32_t proto)
{
    SL_ASSERT(sock);
    SL_GUARD(sock->state != SL_SOCK_STATE_NEW);

    SL_GUARD(sl_endpoint_af_check(&sock->endpoint));
    SL_GUARD(sl_sock_type_set(sock, type));
    SL_GUARD(sl_sock_proto_set(sock, proto));
    SL_GUARD(sl_sock_fd_set(sock, socket(sl_endpoint_af_get(&sock->endpoint), type, proto)));
    if (sl_endpoint_is_ipv6(&sock->endpoint)) {
        int optval = 0;
        SL_GUARD(setsockopt(sock->fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&optval, sizeof(optval)));
    }
    SL_GUARD(sl_sock_state_set(sock, SL_SOCK_STATE_CREATED));

    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_close(sl_sock_t *sock)
{
    SL_ASSERT(sock);

#if SL_SOCK_API_WINSOCK
    if (closesocket((SOCKET)sock->fd)) {
#else
    if (close((int)sock->fd)) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }

    memset(sock, 0, sizeof(*sock));
    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_bind(sl_sock_t *sock)
{
    SL_ASSERT(sock);
    SL_GUARD(sock->state != SL_SOCK_STATE_CREATED);

#if SL_SOCK_API_WINSOCK
    if (bind((SOCKET)sock->fd, sl_endpoint_addr_get(&sock->endpoint), sl_endpoint_size(&sock->endpoint))) {
#else
    if (bind((int)sock->fd, sl_endpoint_addr_get(&sock->endpoint), (socklen_t)sl_endpoint_size(&sock->endpoint))) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
    SL_GUARD(sl_sock_state_set(sock, SL_SOCK_STATE_BOUND));

    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_blocking_set(sl_sock_t *sock)
{
#if SL_SOCK_API_WINSOCK
    uint32_t argp = 0;
    if (ioctlsocket((SOCKET)sock->fd, FIONBIO, &argp)) {
#else
    if (fcntl((int)sock->fd, F_SETFL, ~O_NONBLOCK & fcntl((int)sock->fd, F_GETFL, 0))) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_nonblocking_set(sl_sock_t *sock)
{
#if SL_SOCK_API_WINSOCK
    uint32_t argp = 1;
    if (ioctlsocket((SOCKET)sock->fd, FIONBIO, &argp)) {
#else
    if (fcntl((int)sock->fd, F_SETFL, O_NONBLOCK | fcntl((int)sock->fd, F_GETFL, 0))) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }
    return SL_OK;
}

SL_INLINE_IMPL int sl_sock_send(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint)
{
    SL_ASSERT(sock);
    SL_ASSERT(buf && bufcount > 0);
    SL_ASSERT(endpoint);
    SL_GUARD(sock->state != SL_SOCK_STATE_BOUND);

    int64_t bytes_sent;
    const struct sockaddr *sa = sl_endpoint_addr_get(endpoint);
    const int sa_len = sl_endpoint_size(endpoint);
#if SL_SOCK_API_WINSOCK
    if (WSASendTo((SOCKET)sock->fd, (LPWSABUF)buf, (DWORD)bufcount, (LPDWORD)&bytes_sent, 0, sa, sa_len, NULL, NULL))
    {
#else
    struct msghdr mhdr = {0};
    mhdr.msg_name = endpoint;
    mhdr.msg_namelen = (socklen_t)sizeof(*endpoint);
    mhdr.msg_iov = (struct iovec *)buf;
    mhdr.msg_iovlen = (size_t)bufcount;
    if ((bytes_sent = (ssize_t)sendmsg(sock->fd, &mhdr, 0)) < 0) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }

    return bytes_sent;
}

SL_INLINE_IMPL int sl_sock_recv(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint)
{
    SL_ASSERT(sock);
    SL_ASSERT(buf && bufcount);
    SL_ASSERT(endpoint);
    SL_GUARD(sock->state != SL_SOCK_STATE_BOUND);

    int64_t bytes_recv;
    int32_t epsize = sizeof(*endpoint);
#if SL_SOCK_API_WINSOCK
    DWORD flags = 0;
    if (WSARecvFrom(sock->fd, (LPWSABUF)buf, (DWORD)bufcount, (LPDWORD)&bytes_recv, (LPDWORD)&flags, sl_endpoint_addr_get(endpoint), &epsize, NULL, NULL)) {
#else
    struct msghdr mhdr = {0};
    mhdr.msg_name = endpoint;
    mhdr.msg_namelen = (socklen_t)epsize;
    mhdr.msg_iov = (struct iovec *)buf;
    mhdr.msg_iovlen = (size_t)bufcount;
    if ((bytes_recv = (int64_t)recvmsg(sock->fd, &mhdr, 0)) < 0) {
#endif
        sl_sock_error_set(sock, sl_sys_errno());
        return SL_ERR;
    }

    return bytes_recv;
}

#endif
