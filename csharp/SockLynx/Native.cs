using System;
using System.Security;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace SL
{
    [SuppressUnmanagedCodeSecurity]
    public static unsafe class C
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Buffer
        {
#if SL_SOCK_API_WINSOCK
            public UInt32 len;
            public byte* buf;
#else
            public byte* buf;
            public UIntPtr len;
#endif
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            public static Buffer New(byte* buf, UInt32 len)
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
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct IPv4Addr
        {
            [FieldOffset(0)] public UInt32 int_addr;
            [FieldOffset(0)] public fixed byte byte_addr[4];

            [MethodImpl(MethodImplOptions.AggressiveInlining)]
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
            [FieldOffset(0)] public fixed UInt16 short_addr[8];

            public static IPv6Addr New(int s0, int s1, int s2, int s3, int s4, int s5, int s6, int s7)
            {
                IPv6Addr ip = default;
                ip.short_addr[0] = Util.HtoN((UInt16)s0);
                ip.short_addr[1] = Util.HtoN((UInt16)s1);
                ip.short_addr[2] = Util.HtoN((UInt16)s2);
                ip.short_addr[3] = Util.HtoN((UInt16)s3);
                ip.short_addr[4] = Util.HtoN((UInt16)s4);
                ip.short_addr[5] = Util.HtoN((UInt16)s5);
                ip.short_addr[6] = Util.HtoN((UInt16)s6);
                ip.short_addr[7] = Util.HtoN((UInt16)s7);
                return ip;
            }
        }

        [StructLayout(LayoutKind.Explicit)]
        public struct Endpoint
        {
            /* common members */
            [FieldOffset(0)] public UInt16 af;
            [FieldOffset(2)] public UInt16 port;

            /* ipv4 members */
            [FieldOffset(4)] public IPv4Addr addr4;

            /* ipv6 members */
            [FieldOffset(4)] public UInt32 flowinfo;
            [FieldOffset(8)] public IPv6Addr addr6;
            [FieldOffset(24)] public UInt32 scope_id;

            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            public static Endpoint NewV4(UInt16 port = 0, IPv4Addr addr4 = default)
            {
                Endpoint endpoint = default;
                endpoint.port = (UInt16)port;
                endpoint.addr4 = addr4;
                return endpoint;
            }

            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            public static Endpoint NewV6(UInt16 port = 0, IPv6Addr addr6 = default, UInt32 flowinfo = 0, UInt32 scope_id = 0)
            {
                Endpoint endpoint = default;
                endpoint.port = (UInt16)port;
                endpoint.flowinfo = flowinfo;
                endpoint.scope_id = scope_id;
                endpoint.addr6 = addr6;
                return endpoint;
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct Socket
        {
            Int64 fd;
            UInt32 dir;
            UInt32 state;
            UInt32 type;
            UInt32 proto;
            UInt32 error;
            Endpoint endpoint;

            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            public static Socket New()
            {
                Socket sock = default;
                return sock;
            }
        }


        [DllImport("socklynx", CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 socklynx_setup();

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 socklynx_cleanup();

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 socklynx_socket_nonblocking(Socket* sock, Int32 enabled);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 socklynx_socket_open(Socket* sock, Endpoint* endpoint);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 socklynx_socket_close(Socket* sock);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 socklynx_socket_send(Socket* sock, Buffer* buf, Int32 bufcount, Endpoint* endpoint);

        [DllImport("socklynx", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
        internal static extern Int32 socklynx_socket_recv(Socket* sock, Buffer* buf, Int32 bufcount, Endpoint* endpoint);
    }
}
