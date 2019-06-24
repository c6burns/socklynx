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

#ifndef SL_SYS_H
#define SL_SYS_H

#include "socklynx/common.h"
#include "socklynx/error.h"

#if SL_SOCK_API_WINSOCK
#    include <winsock2.h>
#endif

enum sl_sys_state {
    SL_SYS_STATE_STOPPED,
    SL_SYS_STATE_STARTED,
};

typedef struct sl_sys_s {
    enum sl_sys_state state;
} sl_sys_t;

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

#endif
