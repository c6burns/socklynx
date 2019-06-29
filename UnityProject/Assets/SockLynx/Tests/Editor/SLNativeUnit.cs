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
    C.Socket _sock;

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
        _sock = default;
    }

    [TearDown]
    public void TestCleanup()
    {
        if (_sock.fd != 0)
        {
            fixed (C.Socket* sockptr = &_sock)
            {
                UDP.SocketClose(sockptr);
            }
        }
        Sys.Cleanup();
    }

    [Test]
    public void UDP_Setup()
    {
        Assert.True(Sys.Setup());
    }

    [Test]
    public void UDP_DoubleSetup()
    {
        Assert.True(Sys.Setup());
        Assert.True(Sys.Setup());
    }

    [Test]
    public void UDP_Cleanup()
    {
        Assert.True(Sys.Cleanup());
    }

    [Test]
    public void UDP_DoubleCleanup()
    {
        Assert.True(Sys.Cleanup());
        Assert.True(Sys.Cleanup());
    }

    [Test]
    public void UDP_SetupCleanup()
    {
        Assert.True(Sys.Setup());
        Assert.True(Sys.Cleanup());
    }

    [Test]
    public void UDP_SocketOpenClose()
    {
        _sock = C.Socket.NewUDP(C.Endpoint.NewV4(_port));
        fixed (C.Socket* sockptr = &_sock)
        {
            Assert.True(Sys.Setup());
            Assert.True(UDP.SocketOpen(sockptr));
            Assert.True(UDP.SocketClose(sockptr));
        }
    }

    [Test]
    public void UDP_SocketSetBlocking()
    {
        _sock = C.Socket.NewUDP(C.Endpoint.NewV4(_port));
        fixed (C.Socket* sockptr = &_sock)
        {
            Assert.True(Sys.Setup());
            Assert.True(UDP.SocketOpen(sockptr));
            Assert.True(UDP.SocketNonBlocking(sockptr, true));
            Assert.True(UDP.SocketClose(sockptr));
        }
    }
}
