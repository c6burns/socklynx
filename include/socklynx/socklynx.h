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

#ifndef SL_H
#define SL_H

#include "socklynx/common.h"
#include "socklynx/error.h"
#include "socklynx/endpoint.h"
#include "socklynx/buf.h"
#include "socklynx/sock.h"
#include "socklynx/sys.h"


SL_API int32_t SL_CALL socklynx_setup(void);
SL_API int32_t SL_CALL socklynx_cleanup(void);
SL_API int32_t SL_CALL socklynx_socket_nonblocking(sl_sock_t *sock, uint32_t enabled);
SL_API int32_t SL_CALL socklynx_socket_open(sl_sock_t *sock);
SL_API int32_t SL_CALL socklynx_socket_close(sl_sock_t *sock);
SL_API int32_t SL_CALL socklynx_socket_send(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint);
SL_API int32_t SL_CALL socklynx_socket_recv(sl_sock_t *sock, sl_buf_t *buf, int32_t bufcount, sl_endpoint_t *endpoint);


#endif
