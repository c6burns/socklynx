#ifndef SL_ERROR_H
#define SL_ERROR_H

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
