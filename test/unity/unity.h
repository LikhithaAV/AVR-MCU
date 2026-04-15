#ifndef UNITY_H
#define UNITY_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Version */
#define UNITY_VERSION "2.5.0"

/* Configuration */
#define UNITY_EXCLUDE_FLOAT
#define UNITY_OUTPUT_CHAR printf

/* Basic Assertions */
#define TEST_ASSERT(condition) \
    if (!(condition)) { \
        printf("FAIL: %s:%d\n", __FILE__, __LINE__); \
        exit(1); \
    }

#define TEST_ASSERT_EQUAL(expected, actual) \
    if ((expected) != (actual)) { \
        printf("FAIL: Expected %d, got %d (%s:%d)\n", (int)(expected), (int)(actual), __FILE__, __LINE__); \
        exit(1); \
    }

#define TEST_ASSERT_EQUAL_INT(expected, actual) TEST_ASSERT_EQUAL(expected, actual)

#define TEST_ASSERT_NOT_EQUAL(unexpected, actual) \
    if ((unexpected) == (actual)) { \
        printf("FAIL: Should not be %d (%s:%d)\n", (int)(actual), __FILE__, __LINE__); \
        exit(1); \
    }

#define TEST_ASSERT_NULL(ptr) \
    if ((ptr) != NULL) { \
        printf("FAIL: Expected NULL (%s:%d)\n", __FILE__, __LINE__); \
        exit(1); \
    }

#define TEST_ASSERT_NOT_NULL(ptr) \
    if ((ptr) == NULL) { \
        printf("FAIL: Expected NOT NULL (%s:%d)\n", __FILE__, __LINE__); \
        exit(1); \
    }

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    if (strcmp((expected), (actual)) != 0) { \
        printf("FAIL: Expected '%s', got '%s' (%s:%d)\n", (expected), (actual), __FILE__, __LINE__); \
        exit(1); \
    }

/* Test Suite Setup/Teardown */
extern void setUp(void);
extern void tearDown(void);

#endif /* UNITY_H */
