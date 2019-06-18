#include "socklynx/endpoint.h"


SL_INLINE_IMPL uint16_t sl_endpoint_af(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return endpoint->addr4.sin_family;
}

SL_INLINE_IMPL uint16_t sl_endpoint_af_set(sl_endpoint_t *endpoint, int32_t af)
{
	SL_ASSERT(endpoint);
	return endpoint->addr4.sin_family;
}

SL_INLINE_IMPL bool sl_endpoint_is_ipv4(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return (sl_endpoint_af(endpoint) == AF_INET);
}

SL_INLINE_IMPL bool sl_endpoint_is_ipv6(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	return (sl_endpoint_af(endpoint) == AF_INET6);
}

SL_INLINE_IMPL int sl_endpoint_size(sl_endpoint_t *endpoint)
{
	SL_ASSERT(endpoint);
	if (sl_endpoint_is_ipv4(endpoint)) return sizeof(sockaddr4);
	if (sl_endpoint_is_ipv4(endpoint)) return sizeof(sockaddr6);
	return SL_ERR;
}
