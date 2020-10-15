#pragma once
#include <errno.h>
#include <stdbool.h>

#define PRECISION 1e-7      /**< Presicion of double comparison */

typedef void(*TEST_FUNCTION)(); /**< Simplified name for test function */

/// @name EXPECT macros - check condition and continue anyway
///@{
/** Check, if boolean value is <CODE>true</CODE> */
#define EXPECT_TRUE(x) _assert((x), __FILE__, __LINE__)
/** Check, if boolean value is <CODE>false</CODE> */
#define EXPECT_FALSE(x) _assert(!(x), __FILE__, __LINE__)
/** Check, if two values are equal */
#define EXPECT_EQUAL(x,y) EXPECT_TRUE((x) == (y))
/** Check, if two values are not equal */
#define EXPECT_NOT_EQUAL(x,y) EXPECT_TRUE((x) != (y))
/** Check, if two float-point values are equal */
#define EXPECT_FLOAT_EQUAL(x,y) EXPECT_TRUE((fabs((x) - (y)) <= PRECISION))
/** Check, if given error happened. Caller must set errno to 0 manually. */
#define EXPECT_ERROR(x) EXPECT_EQUAL((x), errno)
/** Check, if given pointer is not <CODE>NULL</CODE>. */
#define EXPECT_NOT_NULL(x) EXPECT_TRUE((x) != NULL)
///@}
/// @name ASSERT macros - check condition and abort test if check failed
///@{
/** Check, if boolean value is <CODE>true</CODE> */
#define ASSERT_TRUE(x) if (!EXPECT_TRUE(x)) return
/** Check, if boolean value is <CODE>false</CODE> */
#define ASSERT_FALSE(x) if (!EXPECT_FALSE(x)) return
/** Check, if two values are equal */
#define ASSERT_EQUAL(x,y) if (!EXPECT_EQUAL(x,y)) return
/** Check, if two values are not equal */
#define ASSERT_NOT_EQUAL(x,y) if (!EXPECT_NOT_EQUAL(x,y)) return
/** Check, if two float-point values are equal */
#define ASSERT_FLOAT_EQUAL(x,y) if (!EXPECT_FLOAT_EQUAL(x,y)) return
/** Check, if given error happened. Caller must set errno to 0 manually. */
#define ASSERT_ERROR(x) if (!EXPECT_ERROR(x)) return
/** Check, if given pointer is not <CODE>NULL</CODE>. */
#define ASSERT_NOT_NULL(x) if (!EXPECT_NOT_NULL(x)) return
///@}

bool lastTestResult;    /**< True, if last runned test was successful, false otherwise */

/**
    \brief Initialize tests
*/
void initializeTests(int maxTestCount /**< Maximum allowed number of tests */);

/**
    \brief Register a test

    Registers function <CODE>testFunction</CODE> as test named <CODE>name</CODE>.
    The function shall return <CODE>true</CODE> if test was passed and <CODE>false</CODE>
    otherwise.
    \param name Test's name
    \param testFunction Test's function
*/
void registerTest(const char* name, TEST_FUNCTION testFunction);

/**
    \brief Run tests

    Runs all registered tests
*/
void runTests();

/**
    \brief Clear tests

    Clear tests in order to free memory
*/
void clearTests();

/**
    \brief Checks, if given <CODE>value</CODE> is <CODE>true</CODE>

    Check, if given <CODE>value</CODE> is <CODE>true</CODE> and prints a warning,
    if not. Returns the given value back.
    Note: Do not call this function directly. Use <CODE>ASSERT</CODE> or <CODE>EXPECT</CODE> macros instead.
    \param value Value to check
    \param file Name of file with assertion
    \param line Number of line with assertion
    \returns Value of <CODE>value</CODE> parameter
*/
bool _assert(bool value, const char* file, int line);