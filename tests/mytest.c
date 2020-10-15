#include "mytest.h"
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

/**
    Structure for test
*/
typedef struct
{
    const char* name;               /**< Test's name */
    TEST_FUNCTION testFunction;     /**< Test's function */
} Test;

Test* tests = NULL;  /**< Array of tests */
int testCount = 0;      /**< Number of registered tests */
int testsLength = 0;    /**< Size of tests array */

void initializeTests(int maxTestCount)
{
    testCount = 0;
    testsLength = maxTestCount;
    tests = (Test*)calloc(maxTestCount, sizeof(Test));
#if defined(_WIN32) || defined(WIN32)
    system("color");    // For some reason, colors do not work in Visual Studio without this
#endif
    setlocale(LC_ALL, "Rus");
}

void registerTest(const char* name, TEST_FUNCTION testFunction)
{
    if (testCount == testsLength)
        puts("\x1b[31mWarning: adding more tests than expected\x1b[37m");
    tests[testCount].name = name;
    tests[testCount].testFunction = testFunction;
    ++testCount;
}

void runTests()
{
    printf("Running all tests\n");
    int passCount = 0;
    for (int i = 0; i < testCount; ++i)
    {
        errno = 0;
        lastTestResult = true;
        tests[i].testFunction();
        if (errno != 0)
        {
            char prefix[1024];
            snprintf(prefix, 1024, "\x1b[31m\"%s\" failed with error\x1b[37m", tests[i].name);
            perror(prefix);
            continue;
        }
        if (lastTestResult)
        {
            printf("\x1b[32m\"%s\" passed\x1b[37m\n", tests[i].name);
            ++passCount;
        }
        else
        {
            printf("\x1b[31m\"%s\" failed\x1b[37m\n", tests[i].name);
        }
        fflush(stdout);
        fflush(stderr);
    }
    printf("Total\t%d tests\nPassed\t%d tests\nFailed\t%d tests\n", testCount, passCount, testCount - passCount);
}

void clearTests()
{
    free(tests);
}

bool _assert(bool value, const char* file, int line)
{
    if (!value)
    {
        int tmp = errno;
        printf("\tAssertion failed in file %s on line %d\n", file, line);
        errno = tmp;
    }
    lastTestResult = lastTestResult && value;
    return value;
}