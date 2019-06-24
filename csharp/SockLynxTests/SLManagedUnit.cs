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
    const ushort LISTEN_PORT = 58343;

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
    public void Util_HtoN()
    {
        ushort s = 12345;
        uint i = 1234512345;
        ulong l = 12345123451234512345;

        ushort s2 = Util.HtoN(s);
        Assert.AreNotEqual(s, s2);
        Assert.AreEqual(s, Util.HtoN(s2));

        uint i2 = Util.HtoN(i);
        Assert.AreNotEqual(i, i2);
        Assert.AreEqual(i, Util.HtoN(i2));

        ulong l2 = Util.HtoN(l);
        Assert.AreNotEqual(l, l2);
        Assert.AreEqual(l, Util.HtoN(l2));
    }

    [Test]
    public void Util_MemCmp()
    {
        byte[] dst = new byte[16];

        Random rand = new Random();
        byte[] src = new byte[16];
        rand.NextBytes(src);
        for (int i = 0; i < src.Length; i++)
            src[i] = Math.Min(src[i], (byte)1);

        fixed (byte* psrc = src)
        fixed (byte* pdst = dst)
        {
            for (int i = 1; i <= src.Length; i++)
            {
                Assert.False(Util.MemCmp(pdst, 0, psrc, 0, i));
                dst[i - 1] = src[i - 1];
                Assert.True(Util.MemCmp(pdst, 0, psrc, 0, i));
            }

            for (int i = 0; i < src.Length - 1; i++)
            {
                Assert.True(Util.MemCmp(pdst, i, psrc, i, src.Length - i));
            }
        }
    }

    [Test]
    public void Util_MemCpy()
    {
        byte[] dst = new byte[16];

        Random rand = new Random();
        byte[] src = new byte[16];
        rand.NextBytes(src);
        for (int i = 0; i < src.Length; i++)
            src[i] = Math.Min(src[i], (byte)1);

        fixed (byte* psrc = src)
        fixed (byte* pdst = dst)
        {
            for (int i = 1; i <= src.Length; i++)
            {
                Assert.False(Util.MemCmp(pdst, 0, psrc, 0, i));
                Util.MemCpy(pdst, 0, psrc, 0, i);
                Assert.True(Util.MemCmp(pdst, 0, psrc, 0, i));
            }
        }
    }

    [Test]
    public void C_IPv4_NewFromHost()
    {
        C.IPv4 ipv4 = C.IPv4.New(127, 0, 0, 1);

        /* certainty */
        C.IPv4 ip = default;
        ip.byte_addr[0] = 1;
        ip.byte_addr[1] = 0;
        ip.byte_addr[2] = 0;
        ip.byte_addr[3] = 127;
        Assert.AreEqual(ipv4.int_addr, Util.HtoN(ip.int_addr));

        /* ridiculous certainty */
        Assert.True(IPAddress.TryParse("127.0.0.1", out IPAddress netip));
        fixed (byte* pSrc = netip.GetAddressBytes())
            Assert.AreEqual(ipv4.int_addr, *(uint*)pSrc);

        /* ludicrous certainty */
        uint addr = (127 << 24) | 1;
        Assert.AreEqual(ipv4.int_addr, Util.HtoN(addr));
    }

    [Test]
    public void C_IPv6_NewFromHost()
    {
        Assert.True(IPAddress.TryParse("fe80::300e:5130:704b:a647%21", out IPAddress netip));

        C.IPv6 ipv6 = C.IPv6.New(0xfe80, 0, 0, 0, 0x300e, 0x5130, 0x704b, 0xa647);

        byte[] src = netip.GetAddressBytes();
        for (int i = 0; i < src.Length; i++)
            Assert.AreEqual(ipv6.byte_addr[i], src[i]);
    }

    [Test]
    public void C_Buffer_Size()
    {
        if (IntPtr.Size == 8)
        {
            Assert.AreEqual(sizeof(C.Buffer), 16);
        }
        else
        {
            Assert.AreEqual(sizeof(C.Buffer), 8);
        }
    }

    [Test]
    public void C_Buffer_New()
    {
        Random rand = new Random();
        byte[] src = new byte[1408];
        rand.NextBytes(src);

        fixed (byte* psrc = src)
        {
            C.Buffer buffer = C.Buffer.New(psrc, src.Length);
            Assert.AreEqual((UIntPtr)buffer.buf, (UIntPtr)psrc);
            Assert.AreEqual((UIntPtr)buffer.len, (UIntPtr)src.Length);
        }
    }

    [Test]
    public void C_Endpoint_NewV4()
    {
        Assert.True(IPAddress.TryParse("127.0.0.1", out IPAddress netip));

        C.Endpoint endpoint = C.Endpoint.NewV4(_port, C.IPv4.New(127, 0, 0, 1));
        Assert.AreEqual(endpoint.af, C.AF.IPv4);
        Assert.AreEqual(endpoint.port, _port);

        fixed (byte* pnetip = netip.GetAddressBytes())
            Assert.True(Util.MemCmp((byte*)&endpoint.addr4, 0, pnetip, 0, sizeof(C.IPv4)));
    }

    [Test]
    public void C_Endpoint_NewV6()
    {
        Assert.True(IPAddress.TryParse("fe80::300e:5130:704b:a647%21", out IPAddress netip));

        C.Endpoint endpoint = C.Endpoint.NewV6(_port, C.IPv6.New(0xfe80, 0, 0, 0, 0x300e, 0x5130, 0x704b, 0xa647));
        Assert.AreEqual(endpoint.af, C.AF.IPv6);
        Assert.AreEqual(endpoint.port, _port);

        fixed (byte* pnetip = netip.GetAddressBytes())
            Assert.True(Util.MemCmp((byte*)&endpoint.addr6, 0, pnetip, 0, sizeof(C.IPv6)));
    }

    [Test]
    public void C_Socket_NewUDP()
    {
        Assert.True(IPAddress.TryParse("fe80::300e:5130:704b:a647%21", out IPAddress netip));

        C.Socket sock = C.Socket.NewUDP(C.Endpoint.NewV6(_port, C.IPv6.New(0xfe80, 0, 0, 0, 0x300e, 0x5130, 0x704b, 0xa647)));
        Assert.AreEqual(sock.fd, 0);
        Assert.AreEqual(sock.dir, 0);
        Assert.AreEqual(sock.state, 0);
        Assert.AreEqual(sock.type, 0);
        Assert.AreEqual(sock.proto, 0);
        Assert.AreEqual(sock.error, 0);
        Assert.AreEqual(sock.endpoint.port, _port);
        Assert.AreEqual(sock.endpoint.af, C.AF.IPv6);


        fixed (byte* pnetip = netip.GetAddressBytes())
            Assert.True(Util.MemCmp((byte*)&sock.endpoint.addr6, 0, pnetip, 0, sizeof(C.IPv6)));
    }
}
