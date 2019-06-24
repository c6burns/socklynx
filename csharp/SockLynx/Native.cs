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

using System;
using System.Security;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace SL
{
    [SuppressUnmanagedCodeSecurity]
    public static unsafe class C
    {
        const MethodImplOptions Inline = MethodImplOptions.AggressiveInlining;

        /*
         * Address Family does NOT match up to the OS, it is converted on the C side
         * the idea here is to use values that no platform would possibly be using
         */
        public enum AF : ushort
        {
            IPv4 = 12340,
            IPv6 = 12341,
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Buffer
        {
#if SL_SOCK_API_WINSOCK
            public uint len;
            public byte* buf;
#else
            public byte* buf;
            public UIntPtr len;
#endif
            [MethodImpl(Inline)]
            public static Buffer New(byte* buf, uint len)
            {
                Buffer buffer = default;
                buffer.buf = buf;
#if SL_SOCK_API_WINSOCK
                buffer.len = len;
#else
                buffer.len = (UIntPtr)len;
#endif
                return buffer;
            }
            [MethodImpl(Inline)] public static Buffer New(byte* buf, int len) => New(buf, (uint)len);
            [MethodImpl(Inline)] public static Buffer New(byte* buf, IntPtr len) => New(buf, (uint)len);
            [MethodImpl(Inline)] public static Buffer New(byte* buf, UIntPtr len) => New(buf, (uint)len);
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct IPv4Addr
        {
            [FieldOffset(0)] public uint int_addr;
            [FieldOffset(0)] public fixed byte byte_addr[4];

            [MethodImpl(Inline)]
            public static IPv4Addr New(byte b0, byte b1, byte b2, byte b3)
            {
                IPv4Addr ip = default;
                ip.byte_addr[0] = b0;
                ip.byte_addr[1] = b1;
                ip.byte_addr[2] = b2;
                ip.byte_addr[3] = b3;
                return ip;
            }
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct IPv6Addr
        {
            [FieldOffset(0)] public fixed byte byte_addr[16];
            [FieldOffset(0)] public fixed ushort short_addr[8];

            [MethodImpl(Inline)]
            public static IPv6Addr New(ushort s0, ushort s1, ushort s2, ushort s3, ushort s4, ushort s5, ushort s6, ushort s7)
            {
                IPv6Addr ip = default;
                ip.short_addr[0] = Util.HtoN(s0);
                ip.short_addr[1] = Util.HtoN(s1);
                ip.short_addr[2] = Util.HtoN(s2);
                ip.short_addr[3] = Util.HtoN(s3);
                ip.short_addr[4] = Util.HtoN(s4);
                ip.short_addr[5] = Util.HtoN(s5);
                ip.short_addr[6] = Util.HtoN(s6);
                ip.short_addr[7] = Util.HtoN(s7);
                return ip;
            }
            [MethodImpl(Inline)]
            public static IPv6Addr New(int s0, int s1, int s2, int s3, int s4, int s5, int s6, int s7)
            {
                return New((ushort)s0, (ushort)s1, (ushort)s2, (ushort)s3, (ushort)s4, (ushort)s5, (ushort)s6, (ushort)s7);
            }
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct Endpoint
        {
            /* common members */
            [FieldOffset(0)] private AF af;
            [FieldOffset(2)] public ushort port;

            /* ipv4 members */
            [FieldOffset(4)] public IPv4Addr addr4;

            /* ipv6 members */
            [FieldOffset(4)] public uint flowinfo;
            [FieldOffset(8)] public IPv6Addr addr6;
            [FieldOffset(24)] public uint scope_id;

            [MethodImpl(Inline)]
            public static Endpoint NewV4(ushort port = 0, IPv4Addr addr4 = default)
            {
                Endpoint endpoint = default;
                endpoint.af = AF.IPv4;
                endpoint.port = port;
                endpoint.addr4 = addr4;
                return endpoint;
            }
            [MethodImpl(Inline)]
            public static Endpoint NewV4(int port = 0, IPv4Addr addr4 = default)
            {
                return NewV4((ushort)port, addr4);
            }

            [MethodImpl(Inline)]
            public static Endpoint NewV6(ushort port = 0, IPv6Addr addr6 = default, uint flowinfo = 0, uint scope_id = 0)
            {
                Endpoint endpoint = default;
                endpoint.af = AF.IPv6;
                endpoint.port = port;
                endpoint.flowinfo = flowinfo;
                endpoint.scope_id = scope_id;
                endpoint.addr6 = addr6;
                return endpoint;
            }
            [MethodImpl(Inline)]
            public static Endpoint NewV6(int port = 0, IPv6Addr addr6 = default, uint flowinfo = 0, uint scope_id = 0)
            {
                return NewV6((ushort)port, addr6, flowinfo, scope_id);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Socket
        {
            /*
             * private members are handled on native side, for good reason
             * eg. sock type and proto are defined differently on diff platforms
             */
            Int64 fd;
            uint dir;
            uint state;
            uint type;
            uint proto;
            public uint error;
            public Endpoint endpoint;

            [MethodImpl(Inline)]
            public static Socket NewUDP(Endpoint endpoint = default)
            {
                Socket sock = default;
                sock.endpoint = endpoint;
                return sock;
            }
        }


        [DllImport("socklynx", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_setup();

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_cleanup();

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_nonblocking(Socket* sock, int enabled);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_open(Socket* sock, Endpoint* endpoint);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_close(Socket* sock);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_send(Socket* sock, Buffer* buf, int bufcount, Endpoint* endpoint);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_recv(Socket* sock, Buffer* buf, int bufcount, Endpoint* endpoint);
    }
}
