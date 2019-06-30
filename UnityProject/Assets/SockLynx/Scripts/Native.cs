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

/* comment this to remove ipv6 code and shorten the length of Endpoint to 16 bytes from 28 */
#define SL_IPV6_ENABLED

#if UNITY_STANDALONE_WIN || UNITY_XBONE
#   define SL_SOCK_API_WINSOCK
#endif

using System;
using System.Security;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

#if UNITY_EDITOR
[assembly: InternalsVisibleTo("SockLynxTests")]
#endif
namespace SL
{
    [SuppressUnmanagedCodeSecurity]
    public static unsafe class C
    {
        public const int SL_OK = 0;
        public const int SL_IP4_SIZE = sizeof(uint);
        public const int SL_IP6_SIZE = 16;
        public const int SL_ENDPOINT4_SIZE = 16;
        public const int SL_ENDPOINT6_SIZE = 28;
#if SL_IPV6_ENABLED
        public const int SL_ENDPOINT_SIZE = SL_ENDPOINT6_SIZE;
        internal const int SL_SOCK_SIZE_UNALIGNED = 32 + SL_ENDPOINT_SIZE;
        public const int SL_SOCK_SIZE = (SL_SOCK_SIZE_UNALIGNED & ~(sizeof(ulong) - 1)) + sizeof(ulong);
        public const bool SL_IPV6_ENABLED = true;
#else
        public const int SL_ENDPOINT_SIZE = SL_ENDPOINT4_SIZE;
        public const int SL_SOCK_SIZE = 32 + SL_ENDPOINT_SIZE;
        public const bool SL_IPV6_ENABLED = false;
#endif

        public enum ContextState : int
        {
            Stopped,
            Started,
        }

        [StructLayout(LayoutKind.Explicit, Size = 8)]
        public struct Context
        {
            [FieldOffset(0)] public readonly ContextState state;
            [FieldOffset(4)] public readonly ushort af_inet;
            [FieldOffset(6)] public readonly ushort af_inet6;

            [MethodImpl(Sys.SL_INLINE)]
            public static bool Initialized(Context* ctx)
            {
                return (ctx->state == ContextState.Started);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Buffer
        {
#if SL_SOCK_API_WINSOCK
            public uint len;
            public byte* buf;
#else
            public byte* buf;
            public void* len;
#endif
            [MethodImpl(Sys.SL_INLINE)]
            public static Buffer New(byte* buf, uint len)
            {
                Buffer buffer = default;
                buffer.buf = buf;
#if SL_SOCK_API_WINSOCK
                buffer.len = len;
#else
                buffer.len = (void*)len;
#endif
                return buffer;
            }
            [MethodImpl(Sys.SL_INLINE)] public static Buffer New(byte* buf, int len) => New(buf, (uint)len);
        }

        [StructLayout(LayoutKind.Explicit, Size = SL_IP4_SIZE)]
        public struct IPv4
        {
            [FieldOffset(0)] public uint int_addr;
            [FieldOffset(0)] public fixed byte byte_addr[4];

            [MethodImpl(Sys.SL_INLINE)]
            public static IPv4 New(byte b0, byte b1, byte b2, byte b3)
            {
                IPv4 ip = default;
                ip.byte_addr[0] = b0;
                ip.byte_addr[1] = b1;
                ip.byte_addr[2] = b2;
                ip.byte_addr[3] = b3;
                return ip;
            }
        }

#if SL_IPV6_ENABLED
        [StructLayout(LayoutKind.Explicit, Size = SL_IP6_SIZE)]
        public struct IPv6
        {
            [FieldOffset(0)] public fixed byte byte_addr[16];
            [FieldOffset(0)] public fixed ushort short_addr[8];

            [MethodImpl(Sys.SL_INLINE)]
            public static IPv6 New(ushort s0, ushort s1, ushort s2, ushort s3, ushort s4, ushort s5, ushort s6, ushort s7)
            {
                IPv6 ip = default;
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

            [MethodImpl(Sys.SL_INLINE)]
            public static IPv6 New(int s0, int s1, int s2, int s3, int s4, int s5, int s6, int s7)
            {
                return New((ushort)s0, (ushort)s1, (ushort)s2, (ushort)s3, (ushort)s4, (ushort)s5, (ushort)s6, (ushort)s7);
            }
        }
#endif

        [StructLayout(LayoutKind.Explicit, Size = SL_ENDPOINT_SIZE)]
        public struct Endpoint
        {
            /* common members */
            [FieldOffset(0)] public ushort af;
            [FieldOffset(2)] public ushort port;

            /* ipv4 members and methods */
            [FieldOffset(4)] public IPv4 addr4;

            /* ipv6 members and methods */
            [FieldOffset(4)] public uint flowinfo;
            [FieldOffset(8)] public IPv6 addr6;
            [FieldOffset(24)] public uint scope_id;

            [MethodImpl(Sys.SL_INLINE)]
            public static Endpoint NewV4(Context* ctx, ushort port = 0, IPv4 addr4 = default)
            {
                Endpoint endpoint = default;
                endpoint.af = ctx->af_inet;
                endpoint.port = port;
                endpoint.addr4 = addr4;
                return endpoint;
            }

            [MethodImpl(Sys.SL_INLINE)]
            public static Endpoint NewV4(Context* ctx, int port = 0, IPv4 addr4 = default)
            {
                return NewV4(ctx, (ushort)port, addr4);
            }

#if SL_IPV6_ENABLED
            

            [MethodImpl(Sys.SL_INLINE)]
            public static Endpoint NewV6(Context* ctx, ushort port = 0, IPv6 addr6 = default, uint flowinfo = 0, uint scope_id = 0)
            {
                Endpoint endpoint = default;
                endpoint.af = ctx->af_inet6;
                endpoint.port = port;
                endpoint.flowinfo = flowinfo;
                endpoint.scope_id = scope_id;
                endpoint.addr6 = addr6;
                return endpoint;
            }

            [MethodImpl(Sys.SL_INLINE)]
            public static Endpoint NewV6(Context* ctx, int port = 0, IPv6 addr6 = default, uint flowinfo = 0, uint scope_id = 0)
            {
                return NewV6(ctx, (ushort)port, addr6, flowinfo, scope_id);
            }
#endif
        }


        public enum SocketFlags : uint
        {
            None,
            NonBlocking = (1 << 0),
            WouldBlockOnRead = (1 << 1),
            WouldBlockOnWrite = (1 << 2),
            IPv4Disabled = (1 << 3),
            IPv6Disabled = (1 << 4),
        }

        public enum SocketState : uint
        {
            New,
            Created,
            Bound,
            Open,
            Closed,
            Error,
        }

        [StructLayout(LayoutKind.Explicit, Size = SL_SOCK_SIZE)]
        public struct Socket
        {
            [FieldOffset(0)] internal long fd;
            [FieldOffset(8)] internal uint dir;
            [FieldOffset(12)] internal SocketState state;
            [FieldOffset(16)] internal uint type;
            [FieldOffset(20)] internal uint proto;
            [FieldOffset(24)] internal uint error;
            [FieldOffset(28)] internal SocketFlags flags;
            [FieldOffset(32)] public Endpoint endpoint;

            [MethodImpl(Sys.SL_INLINE)]
            public static Socket NewUDP(Context* ctx, Endpoint endpoint = default)
            {
                Socket sock = default;
                sock.endpoint = endpoint;
                return sock;
            }

            [MethodImpl(Sys.SL_INLINE)]
            public static bool HasFlag(Socket* sock, SocketFlags flags)
            {
                return sock->flags.HasFlag(flags);
            }
        }

        [DllImport("socklynx", CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_setup(Context* ctx);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_cleanup(Context* ctx);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_nonblocking(Socket* sock, int enabled);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_open(Socket* sock);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_close(Socket* sock);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_send(Socket* sock, Buffer* buf, int bufcount, Endpoint* endpoint);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int socklynx_socket_recv(Socket* sock, Buffer* buf, int bufcount, Endpoint* endpoint);
    }
}
