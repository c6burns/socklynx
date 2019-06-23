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
            UInt32 len;
            byte* buf;
#else
            byte* buf;
            UIntPtr len;
#endif
            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            public static Buffer New(byte* buf, UInt32 len)
            {
                Buffer buffer = default;
                buffer.buf = buf;
                buffer.len = len;
                return buffer;
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct IPV6Addr
        {
            public fixed byte addr6[16];
        }

        [StructLayout(LayoutKind.Explicit, Size = 28)]
        public struct Endpoint
        {
            /* common members */
            [FieldOffset(0)] UInt16 af;
            [FieldOffset(2)] UInt16 port;

            /* ipv4 members */
            [FieldOffset(4)] UInt32 addr4;

            /* ipv6 members */
            [FieldOffset(4)] UInt32 flowinfo;
            [FieldOffset(8)] IPV6Addr addr6;
            [FieldOffset(24)] UInt32 scope_id;

            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            public static Endpoint NewIPv4(UInt16 port = 0, UInt32 addr4 = 0)
            {
                Endpoint endpoint = default;
                endpoint.port = (UInt16)port;
                endpoint.addr4 = (UInt32)addr4;
                return endpoint;
            }

            [MethodImpl(MethodImplOptions.AggressiveInlining)]
            public static Endpoint NewIPv6(UInt16 port = 0, IPV6Addr* addr6 = null, UInt32 flowinfo = 0, UInt32 scope_id = 0)
            {
                Endpoint endpoint = default;
                endpoint.port = (UInt16)port;
                endpoint.flowinfo = flowinfo;
                endpoint.scope_id = scope_id;
                if (addr6 != null) endpoint.addr6 = *addr6;
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
