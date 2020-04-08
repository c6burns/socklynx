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

#include "socklynx/test_harness.h"
#include "socklynx/socklynx.h"


#define pl_server_len 1235
#define mem_server_len 1408
#define pl_client_len 256
#define mem_client_len 1408
static const uint16_t listen_port = 51343;


SL_TEST_CASE_BEGIN(sl_udp_socketsendrecv_blocking)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));

    sl_sockaddr4_t loopback = {0};
    loopback.af = ctx.af_inet;
    loopback.port = listen_port;
    loopback.addr = 127 | (1 << 24);

    /* server side endpoints, socket, random payload, send/recv buffers */
    sl_endpoint_t ep_server;
    ep_server.addr4 = loopback;

    sl_endpoint_t ep_server_recv;

    sl_sock_t sock_server = {0};
    sock_server.proto = SL_SOCK_PROTO_UDP;
    sock_server.type = SL_SOCK_TYPE_DGRAM;
    sock_server.endpoint = ep_server;

    char pl_server[pl_server_len];
    char mem_server[mem_server_len];

    for (int i = 0; i < pl_server_len; i++)
    {
        pl_server[i] = i ^ 0x9a * (i >> 8);
    }

    sl_buf_t buf_server_send;
    buf_server_send.base = &pl_server[0];
    buf_server_send.len = pl_server_len;

    sl_buf_t buf_server_recv;
    buf_server_recv.base = &mem_server[0];
    buf_server_recv.len = mem_server_len;

    /* client side endpoints, socket, random payload, send/recv buffers */
    sl_endpoint_t ep_client;
    loopback.port += 1;
    ep_client.addr4 = loopback;

    sl_endpoint_t ep_client_recv;

    sl_sock_t sock_client = {0};
    sock_client.proto = SL_SOCK_PROTO_UDP;
    sock_client.type = SL_SOCK_TYPE_DGRAM;
    sock_client.endpoint = ep_client;

    char pl_client[pl_client_len];
    char mem_client[mem_client_len];

    for (int i = 0; i < pl_client_len; i++)
    {
        pl_client[i] = i ^ 0xb7 * (i >> 8);
    }

    sl_buf_t buf_client_send;
    buf_client_send.base = &pl_client[0];
    buf_client_send.len = pl_client_len;

    sl_buf_t buf_client_recv;
    buf_client_recv.base = &mem_client[0];
    buf_client_recv.len = mem_client_len;


    ASSERT_SUCCESS(sl_sock_create(&sock_server, SL_SOCK_TYPE_DGRAM, SL_SOCK_PROTO_UDP));
    ASSERT_SUCCESS(sl_sock_bind(&sock_server));
    ASSERT_SUCCESS(sl_sock_create(&sock_client, SL_SOCK_TYPE_DGRAM, SL_SOCK_PROTO_UDP));
    ASSERT_SUCCESS(sl_sock_bind(&sock_client));

    ASSERT_TRUE(pl_client_len == sl_sock_send(&sock_client, &buf_client_send, 1, &ep_server));
    ASSERT_TRUE(pl_client_len == sl_sock_recv(&sock_server, &buf_server_recv, 1, &ep_server_recv));
    ASSERT_TRUE(memcmp(&pl_client[0], buf_server_recv.base, pl_server_len));
    ASSERT_TRUE(memcmp(&ep_client + 2, &ep_server_recv + 2, sizeof(sl_endpoint_t) - 2));

    ASSERT_TRUE(pl_server_len == sl_sock_send(&sock_server, &buf_server_send, 1, &ep_client));
    ASSERT_TRUE(pl_server_len == sl_sock_recv(&sock_client, &buf_client_recv, 1, &ep_client_recv));
    ASSERT_SUCCESS(memcmp(pl_server, buf_client_recv.base, pl_server_len));
    ASSERT_TRUE(memcmp(&ep_server + 2, &ep_client_recv + 2, sizeof(sl_endpoint_t) - 2));

    ASSERT_SUCCESS(sl_sock_close(&sock_server));
    ASSERT_SUCCESS(sl_sock_close(&sock_client));
    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));

SL_TEST_CASE_END(sl_udp_socketsendrecv_blocking)