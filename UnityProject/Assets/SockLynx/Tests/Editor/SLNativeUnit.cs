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

using NUnit.Framework;
using System;
using System.Net;

/* disable unreachable warning due to branching on const C.SL_IPV6_ENABLED */
#pragma warning disable CS0162

namespace SL
{
    public unsafe class SLNativeUnit
    {
        const ushort LISTEN_PORT = 51343;

        ushort _port = Util.HtoN(LISTEN_PORT);

        [OneTimeSetUp]
        public void FixtureSetup()
        {
        }

        [OneTimeTearDown]
        public void FixtureCleanup()
        {
        }

        [SetUp]
        public void TestSetup()
        {
        }

        [TearDown]
        public void TestCleanup()
        {
        }

        [Test]
        public void UDP_Setup()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());
                Assert.AreEqual(C.ContextState.Started, _api._ctx->state);
                Assert.AreNotEqual(0, _api._ctx->af_inet);
                Assert.AreNotEqual(0, _api._ctx->af_inet6);

                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);
            }
        }

        [Test]
        public void UDP_DoubleSetup()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());
                Assert.AreEqual(C.ContextState.Started, _api._ctx->state);
                Assert.AreNotEqual(0, _api._ctx->af_inet);
                Assert.AreNotEqual(0, _api._ctx->af_inet6);

                Assert.True(_api.Setup());
                Assert.AreEqual(C.ContextState.Started, _api._ctx->state);
                Assert.AreNotEqual(0, _api._ctx->af_inet);
                Assert.AreNotEqual(0, _api._ctx->af_inet6);

                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);
            }
        }

        [Test]
        public void UDP_Cleanup()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);
            }
        }

        [Test]
        public void UDP_DoubleCleanup()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);

                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);
            }
        }

        [Test]
        public void UDP_SetupCleanup()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());
                Assert.AreEqual(C.ContextState.Started, _api._ctx->state);
                Assert.AreNotEqual(0, _api._ctx->af_inet);
                Assert.AreNotEqual(0, _api._ctx->af_inet6);

                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);
            }
        }

        [Test]
        public void UDP_DoubleSetupCleanup()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());
                Assert.AreEqual(C.ContextState.Started, _api._ctx->state);
                Assert.AreNotEqual(0, _api._ctx->af_inet);
                Assert.AreNotEqual(0, _api._ctx->af_inet6);

                Assert.True(_api.Setup());
                Assert.AreEqual(C.ContextState.Started, _api._ctx->state);
                Assert.AreNotEqual(0, _api._ctx->af_inet);
                Assert.AreNotEqual(0, _api._ctx->af_inet6);

                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);

                Assert.True(_api.Cleanup());
                Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);
            }
        }

        [Test]
        public void C_Endpoint_NewV4()
        {
            Assert.True(IPAddress.TryParse("127.0.0.1", out IPAddress netip));

            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());

                try
                {
                    C.Endpoint endpoint = C.Endpoint.NewV4(_api._ctx, _port, C.IPv4.New(127, 0, 0, 1));
                    Assert.AreEqual(_api._ctx->af_inet, endpoint.af);
                    Assert.AreEqual(_port, endpoint.port);

                    fixed (byte* pnetip = netip.GetAddressBytes())
                        Assert.True(Util.MemCmp((byte*)&endpoint.addr4, 0, pnetip, 0, sizeof(C.IPv4)));

                    Assert.True(_api.Cleanup());
                }
                finally
                {
                    _api.Cleanup();
                }
            }
        }

        [Test]
        public void C_Endpoint_NewV6()
        {
            if (!C.SL_IPV6_ENABLED) return;

            Assert.True(IPAddress.TryParse("fe80::300e:5130:704b:a647%21", out IPAddress netip));

            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());
                try
                {
                    C.Endpoint endpoint = C.Endpoint.NewV6(_api._ctx, _port, C.IPv6.New(0xfe80, 0, 0, 0, 0x300e, 0x5130, 0x704b, 0xa647));
                    Assert.AreEqual(_api._ctx->af_inet6, endpoint.af);
                    Assert.AreEqual(_port, endpoint.port);

                    fixed (byte* pnetip = netip.GetAddressBytes())
                        Assert.True(Util.MemCmp((byte*)&endpoint.addr6, 0, pnetip, 0, C.SL_IP6_SIZE));

                    Assert.True(_api.Cleanup());
                }
                finally
                {
                    _api.Cleanup();
                }
            }
        }

        [Test]
        public void C_Socket_NewUDP()
        {
            Assert.True(IPAddress.TryParse("127.0.0.1", out IPAddress netip));

            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());

                try
                {
                    C.Socket sock = C.Socket.NewUDP(_api._ctx, C.Endpoint.NewV4(_api._ctx, _port, C.IPv4.New(127, 0, 0, 1)));
                    Assert.AreEqual(0, sock.fd);
                    Assert.AreEqual(0, sock.dir);
                    Assert.AreEqual(C.SocketState.New, sock.state);
                    Assert.AreEqual(C.SockType.Dgram, (C.SockType)sock.type);
                    Assert.AreEqual(C.SockProto.UDP, (C.SockProto)sock.proto);
                    Assert.AreEqual(0, sock.error);
                    Assert.AreEqual(C.SocketFlags.None, sock.flags);
                    Assert.AreEqual(_api._ctx->af_inet, sock.endpoint.af);
                    Assert.AreEqual(_port, sock.endpoint.port);

                    fixed (byte* pnetip = netip.GetAddressBytes())
                        Assert.True(Util.MemCmp((byte*)&sock.endpoint.addr4, 0, pnetip, 0, sizeof(C.IPv4)));

                    Assert.True(_api.Cleanup());
                }
                finally
                {
                    _api.Cleanup();
                }
            }
        }

        [Test]
        public void UDP_SocketOpenClose()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());

                C.Socket sock = C.Socket.NewUDP(_api._ctx, C.Endpoint.NewV4(_api._ctx, _port));
                try
                {
                    Assert.True(API.SocketOpen(&sock));
                    Assert.AreEqual(C.SocketState.Bound, sock.state);
                    Assert.AreEqual(C.SocketFlags.None, sock.flags);
                    Assert.AreEqual(0, sock.error);
                    Assert.False(C.Socket.HasFlag(&sock, C.SocketFlags.NonBlocking));

                    Assert.True(API.SocketClose(&sock));
                    Assert.AreEqual(C.SocketState.Closed, sock.state);
                    Assert.AreEqual(0, sock.fd);

                    Assert.True(_api.Cleanup());
                    Assert.AreEqual(C.ContextState.Stopped, _api._ctx->state);
                }
                finally
                {
                    _api.Cleanup();
                }
            }
        }

        [Test]
        public void UDP_SocketSetBlocking()
        {
            using (SL.API _api = new API())
            {
                Assert.True(_api.Setup());

                C.Socket sock = C.Socket.NewUDP(_api._ctx, C.Endpoint.NewV4(_api._ctx, _port));
                try
                {
                    Assert.True(API.SocketOpen(&sock));
                    Assert.True(API.SocketNonBlocking(&sock, true));
                    Assert.True(C.Socket.HasFlag(&sock, C.SocketFlags.NonBlocking));

                    Assert.True(API.SocketClose(&sock));
                    Assert.True(_api.Cleanup());
                }
                finally
                {
                    API.SocketClose(&sock);
                    _api.Cleanup();
                }
            }
        }
    }
}
#pragma warning disable CS0162
