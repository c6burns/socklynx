#include "socklynx/sock.h"

#include "socklynx/buf.h"


SL_INLINE_IMPL int sl_sock_fd_set(sl_sock_t *sock, int64_t sockfd)
{
	SL_ASSERT(sock);
#if SL_SOCK_API_WINSOCK
	if (sockfd == INVALID_SOCKET) {
#else
	if (sockfd < 0) {
#endif
		sl_sock_error_set(sock, sl_sys_errno());
		return SL_ERR;
	}
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


SL_INLINE_IMPL int sl_sock_error_set(sl_sock_t *sock, uint32_t error)
{
	SL_ASSERT(sock);
	sock->error = error;
	return SL_OK;
}


SL_INLINE_IMPL int sl_sock_create(sl_sock_t *sock, uint32_t af, uint32_t type, uint32_t proto)
{
	SL_ASSERT(sock);
	SL_GUARD(sock->state != SL_SOCK_STATE_NEW);
	
	memset(sock, 0, sizeof(*sock));
	SL_GUARD(sl_endpoint_af_set(&sock->endpoint, af));
	SL_GUARD(sl_sock_type_set(sock, type));
	SL_GUARD(sl_sock_proto_set(sock, proto));
	SL_GUARD(sl_sock_fd_set(sock, socket(af, type, proto)));
	SL_GUARD(sl_sock_state_set(sock, SL_SOCK_STATE_CREATED));

	return SL_OK;
}


SL_INLINE_IMPL int sl_sock_bind(sl_sock_t *sock, void *addr, int addrlen)
{
	SL_ASSERT(sock);
	SL_ASSERT(addr && addrlen);
	SL_GUARD(sock->state != SL_SOCK_STATE_CREATED);

	sl_endpoint_t *ep = addr;
	int eplen = sl_endpoint_size(ep);
	SL_GUARD(eplen != addrlen);
	SL_GUARD(eplen != sl_endpoint_size(&sock->endpoint));

	if (bind(sock->fd, addr, addrlen)) {
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
	if (ioctlsocket(sock->fd, FIONBIO, &argp)) {
#else
	if (fcntl(handle, F_SETFL, ~O_NONBLOCK & fcntl(handle, F_GETFL, 0))) {
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
	if (ioctlsocket(sock->fd, FIONBIO, &argp)) {
#else
	if (fcntl(handle, F_SETFL, O_NONBLOCK | fcntl(handle, F_GETFL, 0))) {
#endif
		sl_sock_error_set(sock, sl_sys_errno());
		return SL_ERR;
	}
	return SL_OK;
}


SL_INLINE_IMPL int sl_sock_send(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint)
{
	SL_ASSERT(sock);
	SL_ASSERT(buf && bufcount);
	SL_ASSERT(endpoint);
	SL_GUARD(sock->state != SL_SOCK_STATE_BOUND);

	int64_t bytes_sent;
#if SL_SOCK_API_WINSOCK
	if (WSASendTo(sock->fd, (LPWSABUF)buf, (DWORD)bufcount, (LPDWORD)&bytes_sent, 0, (struct sockaddr *)endpoint, sl_endpoint_size(endpoint), NULL, NULL)) {
#else
	struct msghdr mhdr = { 0 };
	message.msg_name = (struct sockaddr *)endpoint;
	message.msg_namelen = (int)address->length;
	message.msg_iov = buf;
	message.msg_iovlen = bufcount;
	if ((bytes_sent = (ssize_t)sendmsg(sock->fd, &mhdr, 0)) < 0) {
#endif
		sl_sock_error_set(sock, sl_sys_errno());
		return SL_ERR;
	}

	return SL_OK;
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
	if (WSARecvFrom(sock->fd, (LPWSABUF)buf, (DWORD)bufcount, (LPDWORD)&bytes_recv, 0, (struct sockaddr *)endpoint, &epsize, NULL, NULL)) {
#else
	struct msghdr mhdr = { 0 };
	message.msg_name = (struct sockaddr *)endpoint;
	message.msg_namelen = (socklen_t)epsize;
	message.msg_iov = buf;
	message.msg_iovlen = bufcount;
	if ((bytes_recv = (int64_t)recvmsg(sock->fd, &mhdr, 0)) < 0) {
#endif
		sl_sock_error_set(sock, sl_sys_errno());
		return SL_ERR;
	}

	return SL_OK;
}
