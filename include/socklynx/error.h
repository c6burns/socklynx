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

#ifndef SL_ERROR_H
#define SL_ERROR_H

#include <errno.h>

#define SL_OK 0
#define SL_ERR -1

#ifdef SL_C_MISSING_ASSERT
#    define SL_ASSERT(expr) ((void)0)
#else
#    include <assert.h>
#    define SL_ASSERT(expr) assert(expr)
#endif

#define SL_GUARD(expr) if ((expr)) return SL_ERR
#define SL_GUARD_NULL(expr) if (!(expr)) return SL_ERR
#define SL_GUARD_GOTO(lbl, expr) { if ((expr)) goto lbl; }
#define SL_GUARD_CLEANUP(expr) SL_GUARD_GOTO(cleanup, expr)
#define SL_GUARD_NULL_CLEANUP(expr) SL_GUARD_GOTO(cleanup, !(expr))

#endif
