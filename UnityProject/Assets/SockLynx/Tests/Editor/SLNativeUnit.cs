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
using SL;
using System;
using System.Net;

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
        C.Context ctx = default;
        Assert.True(Sys.Setup(&ctx));
        Assert.AreEqual(C.ContextState.Started, ctx.state);
        Assert.AreNotEqual(0, ctx.af_inet);
        Assert.AreNotEqual(0, ctx.af_inet6);

        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);
    }

    [Test]
    public void UDP_DoubleSetup()
    {
        C.Context ctx = default;
        Assert.True(Sys.Setup(&ctx));
        Assert.AreEqual(C.ContextState.Started, ctx.state);
        Assert.AreNotEqual(0, ctx.af_inet);
        Assert.AreNotEqual(0, ctx.af_inet6);

        Assert.True(Sys.Setup(&ctx));
        Assert.AreEqual(C.ContextState.Started, ctx.state);
        Assert.AreNotEqual(0, ctx.af_inet);
        Assert.AreNotEqual(0, ctx.af_inet6);

        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);
    }

    [Test]
    public void UDP_Cleanup()
    {
        C.Context ctx = default;
        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);
    }

    [Test]
    public void UDP_DoubleCleanup()
    {
        C.Context ctx = default;
        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);

        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);
    }

    [Test]
    public void UDP_SetupCleanup()
    {
        C.Context ctx = default;
        Assert.True(Sys.Setup(&ctx));
        Assert.AreEqual(C.ContextState.Started, ctx.state);
        Assert.AreNotEqual(0, ctx.af_inet);
        Assert.AreNotEqual(0, ctx.af_inet6);

        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);
    }

    [Test]
    public void UDP_DoubleSetupCleanup()
    {
        C.Context ctx = default;
        Assert.True(Sys.Setup(&ctx));
        Assert.AreEqual(C.ContextState.Started, ctx.state);
        Assert.AreNotEqual(0, ctx.af_inet);
        Assert.AreNotEqual(0, ctx.af_inet6);

        Assert.True(Sys.Setup(&ctx));
        Assert.AreEqual(C.ContextState.Started, ctx.state);
        Assert.AreNotEqual(0, ctx.af_inet);
        Assert.AreNotEqual(0, ctx.af_inet6);

        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);

        Assert.True(Sys.Cleanup(&ctx));
        Assert.AreEqual(C.ContextState.Stopped, ctx.state);
    }

    [Test]
    public void UDP_SocketOpenClose()
    {
        C.Context ctx = default;
        Assert.True(Sys.Setup(&ctx));

        C.Socket sock = C.Socket.NewUDP(&ctx, C.Endpoint.NewV4(&ctx, _port));
        try
        {
            Assert.True(UDP.SocketOpen(&sock));
            Assert.AreEqual(C.SocketState.Bound, sock.state);
            Assert.AreEqual(C.SocketFlags.None, sock.flags);
            Assert.AreEqual(0, sock.error);
            Assert.False(C.Socket.HasFlag(&sock, C.SocketFlags.NonBlocking));

            Assert.True(UDP.SocketClose(&sock));
            Assert.AreEqual(C.SocketState.Closed, sock.state);
            Assert.AreEqual(0, sock.fd);

            Assert.True(Sys.Cleanup(&ctx));
            Assert.AreEqual(C.ContextState.Stopped, ctx.state);
        } finally
        {
            Sys.Cleanup(&ctx);
        }
    }

    [Test]
    public void UDP_SocketSetBlocking()
    {
        C.Context ctx = default;
        Assert.True(Sys.Setup(&ctx));

        C.Socket sock = C.Socket.NewUDP(&ctx, C.Endpoint.NewV4(&ctx, _port));
        try
        {
            Assert.True(UDP.SocketOpen(&sock));
            Assert.True(UDP.SocketNonBlocking(&sock, true));
            Assert.True(C.Socket.HasFlag(&sock, C.SocketFlags.NonBlocking));

            Assert.True(UDP.SocketClose(&sock));
            Assert.True(Sys.Cleanup(&ctx));
        }
        finally
        {
            UDP.SocketClose(&sock);
            Sys.Cleanup(&ctx);
        }
    }
}
