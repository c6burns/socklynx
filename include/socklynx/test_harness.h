#ifndef SL_TEST_HARNESS_H
#define SL_TEST_HARNESS_H

#include "aws/common/common.h"
#include "aws/testing/aws_test_harness.h"
#include "socklynx/error.h"

#define SL_TEST_HARNESS

#define SL_TEST_CASE_PARAMS struct aws_allocator *allocator, void *_ctx

#define SL_TEST_CASE_BEGIN(fn)         \
    static int fn(SL_TEST_CASE_PARAMS) \
    {                                  \
        (void)_ctx;                     \
        (void)allocator;

#define SL_TEST_CASE_END(fn)           \
        return SL_OK;                  \
    }                                  \
    AWS_TEST_CASE(ctest_ ## fn, fn)

#endif
