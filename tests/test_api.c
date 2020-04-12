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


static const uint16_t listen_port = 51343;


SL_TEST_CASE_BEGIN(sl_udp_setup)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STARTED == ctx.state);
    ASSERT_TRUE(0 != ctx.af_inet);
    ASSERT_TRUE(0 != ctx.af_inet6);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

SL_TEST_CASE_END(sl_udp_setup);


SL_TEST_CASE_BEGIN(sl_udp_doublesetup)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STARTED == ctx.state);
    ASSERT_TRUE(0 != ctx.af_inet);
    ASSERT_TRUE(0 != ctx.af_inet6);

    ASSERT_SUCCESS(sl_sys_setup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STARTED == ctx.state);
    ASSERT_TRUE(0 != ctx.af_inet);
    ASSERT_TRUE(0 != ctx.af_inet6);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

SL_TEST_CASE_END(sl_udp_doublesetup);


SL_TEST_CASE_BEGIN(sl_udp_cleanup)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

SL_TEST_CASE_END(sl_udp_cleanup);


SL_TEST_CASE_BEGIN(sl_udp_doublecleanup)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

SL_TEST_CASE_END(sl_udp_doublecleanup);


SL_TEST_CASE_BEGIN(sl_udp_setupcleanup)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STARTED == ctx.state);
    ASSERT_TRUE(0 != ctx.af_inet);
    ASSERT_TRUE(0 != ctx.af_inet6);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

SL_TEST_CASE_END(sl_udp_setupcleanup);


SL_TEST_CASE_BEGIN(sl_udp_doublesetupcleanup)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STARTED == ctx.state);
    ASSERT_TRUE(0 != ctx.af_inet);
    ASSERT_TRUE(0 != ctx.af_inet6);

    ASSERT_SUCCESS(sl_sys_setup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STARTED == ctx.state);
    ASSERT_TRUE(0 != ctx.af_inet);
    ASSERT_TRUE(0 != ctx.af_inet6);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

SL_TEST_CASE_END(sl_udp_doublesetupcleanup);


SL_TEST_CASE_BEGIN(sl_udp_newsocket)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));

    sl_sockaddr4_t loopback = {0};
    loopback.af = ctx.af_inet;
    loopback.port = listen_port;
    loopback.addr = 127 | (1 << 24);

    sl_endpoint_t endpoint;
    endpoint.addr4 = loopback;

    sl_sock_t sock = {0};
    sock.proto = SL_SOCK_PROTO_UDP;
    sock.type = SL_SOCK_TYPE_DGRAM;
    sock.endpoint = endpoint;
    ASSERT_TRUE(0 == sock.fd);
    ASSERT_TRUE(0 == sock.dir);
    ASSERT_TRUE(SL_SOCK_STATE_NEW == sock.state);
    ASSERT_TRUE(SL_SOCK_TYPE_DGRAM == sock.type);
    ASSERT_TRUE(SL_SOCK_PROTO_UDP == sock.proto);
    ASSERT_TRUE(0 == sock.error);
    ASSERT_TRUE(0 == sock.flags);
    ASSERT_TRUE(ctx.af_inet == sock.endpoint.addr4.af);
    ASSERT_TRUE(listen_port == sock.endpoint.addr4.port);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));

SL_TEST_CASE_END(sl_udp_newsocket);


SL_TEST_CASE_BEGIN(sl_udp_socketopenclose)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));

    sl_sockaddr4_t loopback = {0};
    loopback.af = ctx.af_inet;
    loopback.port = listen_port;

    sl_endpoint_t endpoint;
    endpoint.addr4 = loopback;

    sl_sock_t sock = {0};
    sock.proto = SL_SOCK_PROTO_UDP;
    sock.type = SL_SOCK_TYPE_DGRAM;
    sock.endpoint = endpoint;

    ASSERT_SUCCESS(sl_sock_create(&sock, SL_SOCK_TYPE_DGRAM, SL_SOCK_PROTO_UDP));
    ASSERT_SUCCESS(sl_sock_bind(&sock));
    ASSERT_TRUE(SL_SOCK_STATE_BOUND == sock.state);
    ASSERT_TRUE(0 == sock.flags);
    ASSERT_TRUE(0 == sock.error);

    ASSERT_SUCCESS(sl_sock_close(&sock));
    ASSERT_TRUE(SL_SOCK_STATE_CLOSED == sock.state);
    ASSERT_TRUE(0 == sock.fd);

    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));
    ASSERT_TRUE(SL_SYS_STATE_STOPPED == ctx.state);

SL_TEST_CASE_END(sl_udp_socketopenclose);


SL_TEST_CASE_BEGIN(sl_udp_socketsetblocking)

    sl_sys_t ctx;

    ASSERT_SUCCESS(sl_sys_setup(&ctx));

    sl_sockaddr4_t loopback = {0};
    loopback.af = ctx.af_inet;
    loopback.port = listen_port;

    sl_endpoint_t endpoint;
    endpoint.addr4 = loopback;

    sl_sock_t sock = {0};
    sock.proto = SL_SOCK_PROTO_UDP;
    sock.type = SL_SOCK_TYPE_DGRAM;
    sock.endpoint = endpoint;

    ASSERT_SUCCESS(sl_sock_create(&sock, SL_SOCK_TYPE_DGRAM, SL_SOCK_PROTO_UDP));
    ASSERT_SUCCESS(sl_sock_bind(&sock));
    ASSERT_SUCCESS(sl_sock_nonblocking_set(&sock));
    ASSERT_TRUE(SL_SOCK_FLAG_NONBLOCKING & sock.flags);

    ASSERT_SUCCESS(sl_sock_close(&sock));
    ASSERT_SUCCESS(sl_sys_cleanup(&ctx));

SL_TEST_CASE_END(sl_udp_socketsetblocking);