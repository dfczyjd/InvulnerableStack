#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>
#include "printItem.h"
#include "SecurityLevels.h"

#ifndef FIRST_USE
#define FIRST_USE

/** Concatenate two params in source code */
#define CAT(X,Y) X##_##Y
/** Make template <CODE>name</CODE> an identifier of type <CODE>type</CODE>*/
#define OF_TYPE(name,type) CAT(name, type)

/// @name Common errors - returned by <CODE>stackCheck()</CODE>
///@{
/** Result is unknown. Never returned by <CODE>stackCheck()</CODE>. Should be used as a param for <CODE>printStack()</CODE>, if the error is unknown */
#define STACKCHECK_UNKNOWN -1
/** No errors */
#define STACKCHECK_OK 0
/** Stack pointer is <CODE>NULL</CODE> */
#define STACKCHECK_STACK_NULL 1
/** Stack's <CODE>data</CODE> pointer is <CODE>NULL</CODE> */
#define STACKCHECK_DATA_NULL 2
/** Stack was destroyed */
#define STACKCHECK_DESTROYED_STACK 3
/** Stack has invalid capacity (not a positive value) */
#define STACKCHECK_INVALID_CAPACITY 4
/** Stack size is over capacity */
#define STACKCHECK_SIZE_OVER_CAPACITY 5
/** Stack has a negative size */
#define STACKCHECK_NEGATIVE_SIZE 6
/** Data canary is broken */
#define STACKCHECK_DEAD_ARRAY_CANARY 7
/** Struct canary is broken */
#define STACKCHECK_DEAD_STRUCT_CANARY 8
/** Struct hash is broken */
#define STACKCHECK_INVALID_STRUCT_HASH 9
/** Data hash is broken */
#define STACKCHECK_INVALID_ARRAY_HASH 10
///@}
/// @name Special errors - returned by other functions
///@{
/** No errors */
#define ERROR_ALL_OK 0
/** Stack to be initialized is <CODE>NULL</CODE> */
#define ERROR_STACK_NULL 1
/** Stack is invalid (<CODE>statckCheck()</CODE> reported errors) */
#define ERROR_STACK_INVALID 2
/** Given capacity is invalid value (not positive) */
#define ERROR_INVALID_CAPACITY 3
/** Stack overflow */
#define ERROR_STACK_OVERFLOW 4
/** Stack underflow */
#define ERROR_STACK_UNDERFLOW 5
/** Given parameter pointer is <CODE>NULL</CODE> */
#define ERROR_PARAM_NULL 6
///@}

/// @name Invalid values ("poisons")
///@{
/** One-byte invalid value */
#define INVALID_1_BYTE 0xff
/** Two-bytes invalid value */
#define INVALID_2_BYTES 0xdead
/** Four-bytes invalid value */
#define INVALID_4_BYTES 0xdeadbeef
///@}

/// @name Canary values
///@{
/** Canaries around structure */
#define STRUCT_CANARY 0xbadf00d
/** Canaries around array */
#define ARRAY_CANARY 0xD15EA5E
///@}
typedef unsigned CANARY_TYPE;

/** Special value for hash of an invalid array */
#define INVALID_ARRAY_HASH 0x15FA11ED
typedef unsigned HASH_TYPE;

/** Multiplier for hash function */
const unsigned MULTIPLIER = 1001;
/** Modulo for hash function */
const unsigned MOD = (unsigned)1e9 + 123;
#endif

#ifndef STACK_TYPE
    #error "STACK_TYPE is not defined"
#endif

#ifndef SECURITY_LEVEL
    #define SECURITY_LEVEL SL_NOTHING
#endif

/** Stack struct */
typedef struct OF_TYPE(Stack, STACK_TYPE)
{
#if SECURITY_LEVEL & SL_CANARY
    /** Struct canary at the beginning */
    CANARY_TYPE beginCanary;
#endif
#if SECURITY_LEVEL & SL_HASHING
    /** Hash of struct since last change */
    HASH_TYPE structHash;
    /** Hash of data array since last change */
    HASH_TYPE arrayHash;
#endif
    /** Number of elements in stack */
    int size;
    /** Stack capacity */
    int capacity;
    /** Data array */
    STACK_TYPE* data;
#if SECURITY_LEVEL & SL_CANARY
    /** Struct canary at the beginning */
    CANARY_TYPE endCanary;
#endif
} OF_TYPE(Stack, STACK_TYPE);

/** Shorter name for stack of given type for use inside the file */
#define Stack OF_TYPE(Stack, STACK_TYPE)

/** Unwrapping of TRUE_PRINT_TYPE */
#define TRUE_PRINT_TYPE(type,out_s) OF_TYPE(printType,STACK_TYPE)(#type,out_s)
/** Print type name into a string */
#define PRINT_TYPE(type,out_s) TRUE_PRINT_TYPE(type,out_s)

/** 
    \brief Print type name into a string

    \param type Type name
    \param out_s Result string
*/
static void OF_TYPE(printType, STACK_TYPE)(char* type, char* out_s)
{
    sprintf(out_s, "[%s]", type);
}

/**
    \brief Hash the stack struct

    \param s Stack to be hashed
    \returns Hash of stack
*/
HASH_TYPE OF_TYPE(stackHashStruct, STACK_TYPE)(Stack *s)
{
#if SECURITY_LEVEL & SL_CANARY
    unsigned char* it = (unsigned char*)s;
    long long hash = 0;
    for (int i = 0; i < sizeof(CANARY_TYPE); ++i, ++it)
        hash = ((hash * MULTIPLIER) % MOD + *it) % MOD;
#else
    unsigned char* it = (unsigned char*)s;
    long long hash = 0;
#endif
    it += 2 * sizeof(HASH_TYPE);    // skipping stored hashes
    for (int i = it - (unsigned char*)s; i < sizeof(Stack); ++i, ++it)
        hash = ((hash * MULTIPLIER) % MOD + *it) % MOD;
    return (HASH_TYPE)hash;
}

/**
    \brief Hash the stack data array

    \param s Stack to be hashed
    \returns Hash of stack data
*/
HASH_TYPE OF_TYPE(stackHashArray, STACK_TYPE)(Stack *s)
{
    if (s->data == NULL || s->capacity <= 0)
        return INVALID_ARRAY_HASH;
    long long hash = 0;
    for (int i = 0; i < s->capacity * sizeof(STACK_TYPE); ++i)
        hash = ((hash * MULTIPLIER) % MOD + ((unsigned char*)s->data)[i]) % MOD;
    return (HASH_TYPE)hash;
}

/**
    \brief Check the stack for errors

    \param stack Stack to be checked
    \returns Error code - one of STACKCHECK_* values
*/
int OF_TYPE(stackCheck, STACK_TYPE)(Stack* stack);

/**
    \brief Print the stack dump

    \param stack Stack to be dump
    \param checkResult Should be result of <CODE>stackCheck()</CODE>, if called before, or STACKCHECK_UNKNOWN otherwise
*/
void OF_TYPE(printStack, STACK_TYPE)(Stack* stack, int checkResult)
{
    char typeName[64];
    PRINT_TYPE(STACK_TYPE, typeName);

    if (checkResult == STACKCHECK_UNKNOWN)
        checkResult = OF_TYPE(stackCheck, STACK_TYPE)(stack);

    fprintf(stderr, "Stack of type %s [0x%p]\n", typeName, stack);
    if (checkResult != STACKCHECK_OK)
        fprintf(stderr, "StackCheck found error #%d\n", checkResult);
    if (checkResult != STACKCHECK_STACK_NULL)
    {
        fprintf(stderr, "  Data:     0x%p\n  Size:     %d\n  Capacity: %d\n", stack->data, stack->size, stack->capacity);
        if (checkResult != STACKCHECK_DATA_NULL)
        {
            for (int i = 0; i < stack->size && i < stack->capacity; ++i)
            {
                fprintf(stderr, "    #%2d: ", i);
                OF_TYPE(printItem, STACK_TYPE)(stack->data + i);
                fprintf(stderr, "\n");
            }
        }
    }
    fflush(stderr);
}

int OF_TYPE(stackCheck, STACK_TYPE)(Stack* stack)
{
    int result = STACKCHECK_OK;
    if (stack == NULL)
        result = STACKCHECK_STACK_NULL;
#if SECURITY_LEVEL & SL_CANARY
    else if (stack->beginCanary != STRUCT_CANARY || stack->endCanary != STRUCT_CANARY)
        result = STACKCHECK_DEAD_STRUCT_CANARY;
#endif
#if SECURITY_LEVEL & SL_HASHING
    else if (stack->structHash != OF_TYPE(stackHashStruct, STACK_TYPE)(stack))
        result = STACKCHECK_INVALID_STRUCT_HASH;
#endif
    else if (stack->data == NULL)
        result = STACKCHECK_DATA_NULL;
    else if (stack->data == (STACK_TYPE*)1)
        result = STACKCHECK_DESTROYED_STACK;
    else if (stack->capacity <= 0)
        result = STACKCHECK_INVALID_CAPACITY;
#if SECURITY_LEVEL & SL_CANARY
    else if (((CANARY_TYPE*)stack->data)[-1] != ARRAY_CANARY || *(CANARY_TYPE*)(stack->data + stack->capacity) != ARRAY_CANARY)
        result = STACKCHECK_DEAD_ARRAY_CANARY;
#endif
#if SECURITY_LEVEL & SL_HASHING
    else if (stack->arrayHash != OF_TYPE(stackHashArray, STACK_TYPE)(stack))
        result = STACKCHECK_INVALID_ARRAY_HASH;
#endif
    else if (stack->size > stack->capacity)
        result = STACKCHECK_SIZE_OVER_CAPACITY;
    else if (stack->size < 0)
        result = STACKCHECK_NEGATIVE_SIZE;
    if (result != STACKCHECK_OK)
        OF_TYPE(printStack, STACK_TYPE)(stack, result);
    return result;
}

/**
    \brief Initialize the stack

    \param s Stack to be initialized
    \param capacity Capacity of the new stack
    \returns Error code - one of ERROR_* values
*/
int OF_TYPE(stackInit, STACK_TYPE)(Stack* stack, int capacity)
{
    if (stack == NULL)
        return ERROR_STACK_NULL;
    if (capacity <= 0)
        return ERROR_INVALID_CAPACITY;
    stack->size = 0;
    stack->capacity = capacity;
#if SECURITY_LEVEL & SL_CANARY
    stack->beginCanary = STRUCT_CANARY;
    stack->endCanary = STRUCT_CANARY;
    stack->data = (STACK_TYPE*)((CANARY_TYPE*)calloc(capacity * sizeof(*stack->data) + 2 * sizeof(CANARY_TYPE), 1) + 1);
    ((unsigned*)stack->data)[-1] = ARRAY_CANARY;
    *(CANARY_TYPE*)(stack->data + capacity) = ARRAY_CANARY;
#else
    stack->data = (STACK_TYPE*)calloc(capacity, sizeof(*stack->data));
#endif
    if (sizeof(STACK_TYPE) == 1)
        memset(stack->data, INVALID_1_BYTE, capacity);
    else if (sizeof(STACK_TYPE) == 2)
    {
        unsigned short value = INVALID_2_BYTES;
        for (int i = 0; i < capacity; ++i)
            stack->data[i] = *((STACK_TYPE*)&value); // = reinterpret_cast<STACK_TYPE>(value)
    }
    else
    {
        unsigned value = INVALID_4_BYTES;
        for (unsigned* it = (unsigned*)stack->data; it < (unsigned*)(stack->data + capacity); ++it)
            *it = value;
    }
#if SECURITY_LEVEL & SL_HASHING
    stack->structHash = OF_TYPE(stackHashStruct, STACK_TYPE)(stack);
    stack->arrayHash = OF_TYPE(stackHashArray, STACK_TYPE)(stack);
#endif
    return ERROR_ALL_OK;
}

/**
    \brief Destroy the stack

    \param s Stack to be destroyed
    \returns Error code - one of ERROR_* values
*/
int OF_TYPE(stackDestroy, STACK_TYPE)(Stack* stack)
{
#if SECURITY_LEVEL & SL_DUMP
    if (OF_TYPE(stackCheck, STACK_TYPE)(stack))
        return ERROR_STACK_INVALID;
#endif
    stack->size = 0;
    stack->capacity = -1;
    free((CANARY_TYPE*)stack->data - 1);
    stack->data = (STACK_TYPE*)1;
#if SECURITY_LEVEL & SL_HASHING
    stack->structHash = OF_TYPE(stackHashStruct, STACK_TYPE)(stack);
    stack->arrayHash = OF_TYPE(stackHashArray, STACK_TYPE)(stack);
#endif
    return ERROR_ALL_OK;
}

/**
    \brief Push a new value into the stack

    \param s Stack to be pushed into
    \param value Value to be pushed
    \returns Error code - one of ERROR_* values
*/
int OF_TYPE(stackPush, STACK_TYPE)(Stack* stack, STACK_TYPE value)
{
#if SECURITY_LEVEL & SL_DUMP
    if (OF_TYPE(stackCheck, STACK_TYPE)(stack))
        return ERROR_STACK_INVALID;
#endif
    if (stack->size >= stack->capacity)
        return ERROR_STACK_OVERFLOW;
    stack->data[stack->size++] = value;
#if SECURITY_LEVEL & SL_HASHING
    stack->structHash = OF_TYPE(stackHashStruct, STACK_TYPE)(stack);
    stack->arrayHash = OF_TYPE(stackHashArray, STACK_TYPE)(stack);
#endif
    return ERROR_ALL_OK;
}

/**
    \brief Pop a value from the stack

    \param s Stack to be popped from
    \param [out] result Address to where the popped value will be written
    \returns Error code - one of ERROR_* values
*/
int OF_TYPE(stackPop, STACK_TYPE)(Stack* stack, STACK_TYPE* result)
{
#if SECURITY_LEVEL & SL_DUMP
    if (OF_TYPE(stackCheck, STACK_TYPE)(stack))
        return ERROR_STACK_INVALID;
#endif
    if (result == NULL)
        return ERROR_PARAM_NULL;
    if (stack->size == 0)
        return ERROR_STACK_UNDERFLOW;
    *result = stack->data[--stack->size];
#if SECURITY_LEVEL & SL_HASHING
    stack->structHash = OF_TYPE(stackHashStruct, STACK_TYPE)(stack);
    stack->arrayHash = OF_TYPE(stackHashArray, STACK_TYPE)(stack);
#endif
    return ERROR_ALL_OK;
}

#undef Stack
