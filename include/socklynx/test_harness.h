#ifndef SL_TEST_HARNESS_H
#define SL_TEST_HARNESS_H

#include "aws/common/common.h"
#include "aws/testing/aws_test_harness.h"

#define SL_TEST_HARNESS

#define SL_TEST_CASE_PARAMS struct aws_allocator *allocator, void *ctx

#define SL_TEST_CASE_BEGIN(fn)         \
    static int fn(SL_TEST_CASE_PARAMS) \
    {                                  \
        (void)ctx;                     \
        (void)allocator;

#define SL_TEST_CASE_END(fn)           \
        return SL_OK;                  \
    }                                  \
    AWS_TEST_CASE(ctest_ ## fn, fn)

#include "socklynx/error.h"

#endif
