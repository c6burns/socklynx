#include "socklynx/sys.h"


SL_INLINE_IMPL int sl_sys_errno(void)
{
	/* PLATFORM TODO: extend OS error retrieval for your console platform */
#if SL_SOCK_API_WINSOCK
	return WSAGetLastError();
#else
	return errno;
#endif
	return SL_OK;
}

SL_INLINE_IMPL int sl_sys_setup(sl_sys_t *sys)
{
	SL_ASSERT(sys);
	if (sys->state == SL_SYS_STATE_STARTED) return SL_OK;

	int rv = SL_OK;
	/* PLATFORM TODO: extend network setup for your console platform */
#if SL_SOCK_API_WINSOCK
	WSADATA wsaData;
	rv = WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

	if (rv == SL_OK) sys->state = SL_SYS_STATE_STARTED;
	return rv;
}

SL_INLINE_IMPL int sl_sys_cleanup(sl_sys_t *sys)
{
	SL_ASSERT(sys);
	if (sys->state == SL_SYS_STATE_STOPPED) return SL_OK;

	int rv = SL_OK;
	/* PLATFORM TODO: extend network cleanup for your console platform */
#if SL_SOCK_API_WINSOCK
	rv = WSACleanup();
#endif

	if (rv == SL_OK) sys->state = SL_SYS_STATE_STOPPED;
	return rv;
}
