#include "mytest.h"
#include "../src/SecurityLevels.h"
#define SECURITY_LEVEL SL_ALL
#define STACK_TYPE long
#include "../src/InvulnerableStack.h"
#undef STACK_TYPE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/**
    \brief Test of regular stack usage
*/
void regularTest()
{
    Stack_long st;
    EXPECT_EQUAL(stackInit_long(&st, 5), ERROR_ALL_OK);
    EXPECT_EQUAL(stackPush_long(&st, 10), ERROR_ALL_OK);
    EXPECT_EQUAL(stackPush_long(&st, 7), ERROR_ALL_OK);
    long res;
    EXPECT_EQUAL(stackPop_long(&st, &res), ERROR_ALL_OK);
    EXPECT_EQUAL(res, 7);
    EXPECT_EQUAL(stackDestroy_long(&st), ERROR_ALL_OK);
}

void commonErrorsTest()
{
    EXPECT_EQUAL(stackPush_long(NULL, 3), ERROR_STACK_INVALID);
    Stack_long st;
    EXPECT_EQUAL(stackPush_long(&st, 3), ERROR_STACK_INVALID);
    EXPECT_EQUAL(stackInit_long(&st, -1), ERROR_INVALID_CAPACITY);
    EXPECT_EQUAL(stackInit_long(NULL, 1), ERROR_STACK_NULL);

    EXPECT_EQUAL(stackInit_long(&st, 1), ERROR_ALL_OK);
    EXPECT_EQUAL(stackPop_long(&st, NULL), ERROR_PARAM_NULL);
    long tmp;
    EXPECT_EQUAL(stackPop_long(&st, &tmp), ERROR_STACK_UNDERFLOW);
    EXPECT_EQUAL(stackPush_long(&st, 3), ERROR_ALL_OK);
    EXPECT_EQUAL(stackPush_long(&st, 3), ERROR_STACK_OVERFLOW);
    EXPECT_EQUAL(stackDestroy_long(&st), ERROR_ALL_OK);
    EXPECT_EQUAL(stackDestroy_long(&st), ERROR_STACK_INVALID);
}

void stackHashStructTest()
{
    Stack_long st;
    EXPECT_EQUAL(stackInit_long(&st, 5), ERROR_ALL_OK);
    unsigned hash = stackHashStruct_long(&st);
    EXPECT_EQUAL(stackPush_long(&st, 10), ERROR_ALL_OK);
    unsigned newHash = stackHashStruct_long(&st);
    EXPECT_NOT_EQUAL(newHash, hash);
    long tmp;
    EXPECT_EQUAL(stackPop_long(&st, &tmp), ERROR_ALL_OK);
    EXPECT_EQUAL(stackHashStruct_long(&st), hash);
    EXPECT_EQUAL(stackPush_long(&st, 15), ERROR_ALL_OK);
    EXPECT_EQUAL(stackHashStruct_long(&st), newHash);       // Hash does not include value, so hash should not change, when element is changed
}

void stackHashArrayTest()
{
    Stack_long st;
    EXPECT_EQUAL(stackInit_long(&st, 5), ERROR_ALL_OK);
    unsigned hash = stackHashArray_long(&st);
    EXPECT_EQUAL(stackPush_long(&st, 10), ERROR_ALL_OK);
    unsigned newHash = stackHashArray_long(&st);
    EXPECT_NOT_EQUAL(newHash, hash);
    long tmp;
    EXPECT_EQUAL(stackPop_long(&st, &tmp), ERROR_ALL_OK);
    EXPECT_EQUAL(stackHashArray_long(&st), newHash);       // Value was not actually erased, so array hash should not change
}

int main()
{
    initializeTests(10);
    //registerTest("Regular", regularTest);
    registerTest("Common errors", commonErrorsTest);
    registerTest("Struct hashing", stackHashStructTest);
    registerTest("Array hashing", stackHashArrayTest);
    runTests();
    clearTests();
    return 0;
}