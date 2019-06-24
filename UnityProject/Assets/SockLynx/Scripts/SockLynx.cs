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

using System.Runtime.CompilerServices;

namespace SL
{
    public static unsafe class UDP
    {
        const int SL_OK = 0;
        const MethodImplOptions Inline = MethodImplOptions.AggressiveInlining;

        [MethodImpl(Inline)]
        public static bool Setup()
        {
            return (C.socklynx_setup() == SL_OK);
        }

        [MethodImpl(Inline)]
        public static bool Cleanup()
        {
            return (C.socklynx_cleanup() == SL_OK);
        }

        [MethodImpl(Inline)]
        public static bool SocketOpen(C.Socket* sock, C.Endpoint* endpoint)
        {
            return (C.socklynx_socket_open(sock, endpoint) == SL_OK);
        }

        [MethodImpl(Inline)]
        public static bool SocketClose(C.Socket* sock)
        {
            return (C.socklynx_socket_close(sock) == SL_OK);
        }

        [MethodImpl(Inline)]
        public static bool SocketNonBlocking(C.Socket* sock, bool enabled)
        {
            return (C.socklynx_socket_nonblocking(sock, enabled ? 1 : 0) == SL_OK);
        }

        [MethodImpl(Inline)]
        public static int SocketSend(C.Socket* sock, C.Buffer* bufferArray, int bufferCount, C.Endpoint* endpoint)
        {
            return C.socklynx_socket_send(sock, bufferArray, bufferCount, endpoint);
        }

        [MethodImpl(Inline)]
        public static int SocketRecv(C.Socket* sock, C.Buffer* bufferArray, int bufferCount, C.Endpoint* endpoint)
        {
            return C.socklynx_socket_recv(sock, bufferArray, bufferCount, endpoint);
        }
    }
}
