#ifndef SL_ERROR_H
#define SL_ERROR_H

/*
 * Copyright 2019 Chris Burns <chris@kitty.city>
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#define SL_OK 0
#define SL_ERR -1

#ifdef SL_NO_ASSERT
#	define SL_ASSERT(expr) ((void)0)
#else
#	include <assert.h>
#	define SL_ASSERT(expr) assert(expr)
#endif

#define SL_GUARD(expr) if ((expr)) return TN_ERROR
#define SL_GUARD_NULL(expr) if (!(expr)) return TN_ERROR
#define SL_GUARD_GOTO(lbl, expr) { if ((expr)) goto lbl; }
#define SL_GUARD_CLEANUP(expr) TN_GUARD_GOTO(cleanup, expr)
#define SL_GUARD_NULL_CLEANUP(expr) TN_GUARD_GOTO(cleanup, !(expr))



#endif
