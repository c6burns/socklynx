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

#define LISTEN_PORT = 51343;


SL_TEST_CASE_BEGIN(sl_udp_socketsendrecv_blocking)

    sl_sock_s sock_server;
    sl_sock_s sock_client;

    ASSERT_TRUE(socklynx_setup(&ctx));

    /* server side endpoints, socket, random payload, send/recv buffers */

    sl_sockaddr4_s loopback;
    loopback.af = ctx->af_inet;
    loopback.port = LISTEN_PORT;
    loopback.addr = 127 | (1 << 48);

    sl_endpoint_u ep_server;
    ep_server.addr4 = loopback;
    sl_endpoint_u ep_server_recv = default;
    sock_server.proto = SL_SOCK_PROTO_UDP;
    sock_server.type = SL_SOCK_TYPE_DGRAM;
    sock_server.endpoint = ep_server;
    sl_buf_s buf_server_recv;
    sl_buf_s buf_server_send;
    char pl_server_mem[1235];
    void* pl_server = &pl_server_mem[0];
    char mem_server_mem[1408];
    void* mem_server = &mem_server_mem[0];
    for (int i = 0; i < 1235; i++)
    {
        pl_server[i] = i ^ 0x9a * (i >> 8);
    }

    /* client side endpoints, socket, random payload, send/recv buffers */
    sl_endpoint_u ep_client;
    loopback.port += 1;
    ep_client.addr4 = loopback;
    sl_endpoint_u ep_client_recv = default;
    sock_client.proto = SL_SOCK_PROTO_UDP;
    sock_client.type = SL_SOCK_TYPE_DGRAM;
    sock_client.endpoint = ep_client;
    sl_buf_s buf_client_recv;
    sl_buf_s buf_client_send;
    char pl_client_mem[256];
    void* pl_client = &pl_client_mem[0];
    char mem_client_mem[1408];
    void* mem_client = &mem_client_mem[0];
    for (int i = 0; i < 256; i++)
    {
        pl_client[i] = i ^ 0x9a * (i >> 8);
    }

    ASSERT_TRUE(socklynx_socket_open(&sock_server));
    ASSERT_TRUE(socklynx_socket_open(&sock_client));

    ASSERT_TRUE(pl_client.Length == socklynx_socket_send(&sock_client, &buf_client_send, 1, &ep_server));
    ASSERT_TRUE(pl_client.Length == socklynx_socket_recv(&sock_server, &buf_server_recv, 1, &ep_server_recv));
    ASSERT_TRUE(memcmp(pl_client, buf_server_recv.buf, 1235));
    ASSERT_TRUE(memcmp((void*)&ep_client + 2, (void*)&ep_server_recv + 2, sizeof(sl_endpoint_u) - 2));

    ASSERT_TRUE(pl_server.Length == socklynx_socket_send(&sock_server, &buf_server_send, 1, &ep_client));
    ASSERT_TRUE(pl_server.Length == socklynx_socket_recv(&sock_client, &buf_client_recv, 1, &ep_client_recv));
    ASSERT_TRUE(memcmp(pl_server, buf_client_recv.buf, 256));
    ASSERT_TRUE(memcmp((void*)&ep_server + 2, (void*)&ep_client_recv + 2, sizeof(sl_endpoint_u) - 2));

    ASSERT_TRUE(socklynx_socket_close(&sock_server));
    ASSERT_TRUE(socklynx_socket_close(&sock_client));
    ASSERT_TRUE(socklynx_cleanup(&ctx));

SL_TEST_CASE_END(sl_udp_socketsendrecv_blocking)