#include "socklynx/sock.h"


SL_INLINE_IMPL int sl_sock_fd_set(sl_sock_t *sock, int64_t sockfd)
{
	SL_GUARD_NULL(sock);
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
	SL_GUARD_NULL(sock);
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
	SL_GUARD_NULL(sock);
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
	SL_GUARD_NULL(sock);
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
	SL_GUARD_NULL(sock);
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
	SL_GUARD_NULL(sock);
	sock->error = error;
	return SL_OK;
}


SL_INLINE_IMPL int sl_sock_create(sl_sock_t *sock, uint32_t af, uint32_t type, uint32_t proto)
{
	SL_GUARD_NULL(sock);
	SL_GUARD(sock->state != SL_SOCK_STATE_NEW);
	memset(sock, 0, sizeof(*sock));

	SL_GUARD(sl_endpoint_af_set(&sock->endpoint, af));
	SL_GUARD(sl_sock_type_set(sock, type));
	SL_GUARD(sl_sock_proto_set(sock, proto));
	SL_GUARD(sl_sock_fd_set(sock, socket(af, type, proto)));
	SL_GUARD(sl_sock_state_set(sock, SL_SOCK_STATE_CREATED));

	return SL_OK;
}
