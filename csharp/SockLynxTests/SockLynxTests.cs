using NUnit.Framework;
using SL;
using System;
using System.Net;

public unsafe class SLUnitTests
{
    const UInt16 LISTEN_PORT = 58343;

    UInt16 _port = Util.HtoN(LISTEN_PORT);

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
    public void IPv4_NewFromHost()
    {
        C.IPv4Addr ipv4 = C.IPv4Addr.New(127, 0, 0, 1);

        /* certainty */
        C.IPv4Addr ip = default;
        ip.byte_addr[0] = 1;
        ip.byte_addr[1] = 0;
        ip.byte_addr[2] = 0;
        ip.byte_addr[3] = 127;
        Assert.AreEqual(ipv4.int_addr, Util.HtoN(ip.int_addr));

        /* ridiculous certainty */
        Assert.True(IPAddress.TryParse("127.0.0.1", out IPAddress netip));
        fixed (byte* pSrc = netip.GetAddressBytes())
            Assert.AreEqual(ipv4.int_addr, *(UInt32*)pSrc);

        /* ludicrous certainty */
        UInt32 addr = (127 << 24) | 1;
        Assert.AreEqual(ipv4.int_addr, Util.HtoN(addr));
    }

    [Test]
    public void IPv6_NewFromHost()
    {
        Assert.True(IPAddress.TryParse("fe80::300e:5130:704b:a647%21", out IPAddress netip));

        C.IPv6Addr ipv6 = C.IPv6Addr.New(0xfe80, 0, 0, 0, 0x300e, 0x5130, 0x704b, 0xa647);

        byte[] src = netip.GetAddressBytes();
        for (int i = 0; i < src.Length; i++)
            Assert.AreEqual(ipv6.byte_addr[i], src[i]);
    }
}
