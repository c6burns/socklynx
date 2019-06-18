#include "socklynx/sock.h"


SL_INLINE_IMPL int sl_sock_create(sl_sock_t *sock, uint32_t af, uint32_t type, uint32_t proto)
{
	SL_GUARD_NULL(sock);
	SL_GUARD(sock->state != SL_SOCK_STATE_NEW);
	memset(sock, 0, sizeof(*sock));

	SL_GUARD(sl_endpoint_af_set(&sock->endpoint, af));

	switch (type) {
	case SL_SOCK_TYPE_DGRAM:
	case SL_SOCK_TYPE_STREAM:
		sock->type = type;
		break;
	default:
		return SL_ERR;
	}

	switch (proto) {
	case SL_SOCK_PROTO_UDP:
	case SL_SOCK_PROTO_TCP:
		sock->proto = proto;
		break;
	default:
		return SL_ERR;
	}

	int64_t sockfd = socket(sl_endpoint_af(&sock->endpoint), sock->type, sock->proto);
#if SL_SOCK_API_WINSOCK
	if (sockfd == INVALID_SOCKET) {
#else
	if (sockfd < 0) {
#endif
		sock->err = sl_sys_errno();
		return SL_ERR;
	}

	sock->fd = sockfd;
	return SL_OK;
}
