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
    public class Util
    {
        /* these methods assume little endian */
        #region Endian Swapping
        [MethodImpl(Sys.SL_INLINE)]
        public static ushort HtoN(ushort input)
        {
            return (ushort)((input >> 8) + (input << 8));
        }
        [MethodImpl(Sys.SL_INLINE)] public static short HtoN(short input) => (short)HtoN((ushort)input);
        [MethodImpl(Sys.SL_INLINE)] public static ushort NtoH(ushort input) => HtoN(input);
        [MethodImpl(Sys.SL_INLINE)] public static short NtoH(short input) => (short)HtoN((ushort)input);

        [MethodImpl(Sys.SL_INLINE)]
        public static uint HtoN(uint input)
        {
            uint num1 = input & 0x00FF00FFu;
            uint num2 = input & 0xFF00FF00u;
            return (num1 >> 8 | num1 << 24) + (num2 << 8 | num2 >> 24);
        }
        [MethodImpl(Sys.SL_INLINE)] public static int HtoN(int input) => (int)HtoN((uint)input);
        [MethodImpl(Sys.SL_INLINE)] public static uint NtoH(uint input) => HtoN(input);
        [MethodImpl(Sys.SL_INLINE)] public static int NtoH(int input) => (int)HtoN((uint)input);

        [MethodImpl(Sys.SL_INLINE)]
        public static ulong HtoN(ulong input)
        {
            return ((ulong)HtoN((uint)input) << 32) + HtoN((uint)(input >> 32));
        }
        [MethodImpl(Sys.SL_INLINE)] public static long HtoN(long input) => (long)HtoN((ulong)input);
        [MethodImpl(Sys.SL_INLINE)] public static ulong NtoH(ulong input) => HtoN(input);
        [MethodImpl(Sys.SL_INLINE)] public static long NtoH(long input) => (long)HtoN((ulong)input);
        #endregion

        #region Unsafe MemCpy / MemCmp
        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy1(byte* pdst, byte* psrc)
        {
            *pdst = *psrc;
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy2(byte* pdst, byte* psrc)
        {
            *(ushort*)pdst = *(ushort*)psrc;
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy3(byte* pdst, byte* psrc)
        {
            *(ushort*)(pdst + 0) = *(ushort*)(psrc + 0);
            *(ushort*)(pdst + 1) = *(ushort*)(psrc + 1);
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy4(byte* pdst, byte* psrc)
        {
            *(uint*)pdst = *(uint*)psrc;
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy5(byte* pdst, byte* psrc)
        {
            *(uint*)(pdst + 0) = *(uint*)(psrc + 0);
            *(uint*)(pdst + 1) = *(uint*)(psrc + 1);
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy6(byte* pdst, byte* psrc)
        {
            *(uint*)(pdst + 0) = *(uint*)(psrc + 0);
            *(uint*)(pdst + 2) = *(uint*)(psrc + 2);
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy7(byte* pdst, byte* psrc)
        {
            *(uint*)(pdst + 0) = *(uint*)(psrc + 0);
            *(uint*)(pdst + 3) = *(uint*)(psrc + 3);
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe void Copy8(byte* pdst, byte* psrc)
        {
            *(ulong*)pdst = *(ulong*)psrc;
        }

        [MethodImpl(Sys.SL_INLINE)]
        public static unsafe void MemCpy(byte* dst, int dstOffset, byte* src, int srcOffset, int byteLength)
        {
            byte* psrc = src + srcOffset;
            byte* pdst = dst + dstOffset;
            ulong longByteLength = (ulong)byteLength;
            ulong longWriteLimit = longByteLength & ~7ul;

            /* write as much as possible with 64 bit width */
            for (ulong cursor = 0; cursor < longWriteLimit; cursor += sizeof(long))
            {
                Copy8(pdst + cursor, psrc + cursor);
            }

            /* write the remaining < 8 bytes as efficiently as possible */
            switch (longByteLength & 7ul)
            {
                case 1:
                    Copy1(pdst + longWriteLimit, psrc + longWriteLimit);
                    break;
                case 2:
                    Copy2(pdst + longWriteLimit, psrc + longWriteLimit);
                    break;
                case 3:
                    Copy3(pdst + longWriteLimit, psrc + longWriteLimit);
                    break;
                case 4:
                    Copy4(pdst + longWriteLimit, psrc + longWriteLimit);
                    break;
                case 5:
                    Copy5(pdst + longWriteLimit, psrc + longWriteLimit);
                    break;
                case 6:
                    Copy6(pdst + longWriteLimit, psrc + longWriteLimit);
                    break;
                case 7:
                    Copy7(pdst + longWriteLimit, psrc + longWriteLimit);
                    break;
            }
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp1(byte* pdst, byte* psrc)
        {
            return (*pdst == *psrc);
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp2(byte* pdst, byte* psrc)
        {
            return (*(ushort*)pdst == *(ushort*)psrc);
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp3(byte* pdst, byte* psrc)
        {
            return (*(ushort*)(pdst + 0) == *(ushort*)(psrc + 0)) && (*(ushort*)(pdst + 1) == *(ushort*)(psrc + 1));
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp4(byte* pdst, byte* psrc)
        {
            return (*(uint*)pdst == *(uint*)psrc);
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp5(byte* pdst, byte* psrc)
        {
            return (*(uint*)(pdst + 0) == *(uint*)(psrc + 0)) && (*(uint*)(pdst + 1) == *(uint*)(psrc + 1));
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp6(byte* pdst, byte* psrc)
        {
            return (*(uint*)(pdst + 0) == *(uint*)(psrc + 0)) && (*(uint*)(pdst + 2) == *(uint*)(psrc + 2));
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp7(byte* pdst, byte* psrc)
        {
            return (*(uint*)(pdst + 0) == *(uint*)(psrc + 0)) && (*(uint*)(pdst + 3) == *(uint*)(psrc + 3));
        }

        [MethodImpl(Sys.SL_INLINE)]
        private static unsafe bool Cmp8(byte* pdst, byte* psrc)
        {
            return (*(ulong*)pdst == *(ulong*)psrc);
        }

        [MethodImpl(Sys.SL_INLINE)]
        public static unsafe bool MemCmp(byte* dst, int dstOffset, byte* src, int srcOffset, int byteLength)
        {
            byte* psrc = src + srcOffset;
            byte* pdst = dst + dstOffset;
            ulong longByteLength = (ulong)byteLength;
            ulong longWriteLimit = longByteLength & ~7ul;

            for (ulong cursor = 0; cursor < longWriteLimit; cursor += sizeof(long))
            {
                if (!Cmp8(pdst + cursor, psrc + cursor)) return false;
            }

            // write anything remaining under 8 bytes
            switch (longByteLength & 7ul)
            {
                case 1:
                    if (!Cmp1(pdst + longWriteLimit, psrc + longWriteLimit)) return false;
                    break;
                case 2:
                    if (!Cmp2(pdst + longWriteLimit, psrc + longWriteLimit)) return false;
                    break;
                case 3:
                    if (!Cmp3(pdst + longWriteLimit, psrc + longWriteLimit)) return false;
                    break;
                case 4:
                    if (!Cmp4(pdst + longWriteLimit, psrc + longWriteLimit)) return false;
                    break;
                case 5:
                    if (!Cmp5(pdst + longWriteLimit, psrc + longWriteLimit)) return false;
                    break;
                case 6:
                    if (!Cmp6(pdst + longWriteLimit, psrc + longWriteLimit)) return false;
                    break;
                case 7:
                    if (!Cmp7(pdst + longWriteLimit, psrc + longWriteLimit)) return false;
                    break;
            }

            return true;
        }
        #endregion
    }
}
