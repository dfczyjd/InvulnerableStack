#pragma once
#include <stdio.h>

/// @name Print a stack item
///@{
void printItem_uchar(unsigned char* item)
{
    fprintf(stderr, "%hhu (%c)", *item, *item);
}

void printItem_char(signed char* item)
{
    fprintf(stderr, "%hhd (%c)", *item, *item);
}

void printItem_ushort(unsigned short* item)
{
    fprintf(stderr, "%hu", *item);
}

void printItem_short(signed short* item)
{
    fprintf(stderr, "%hd", *item);
}

void printItem_uint(unsigned int* item)
{
    fprintf(stderr, "%u", *item);
}

void printItem_int(int* item)
{
    fprintf(stderr, "%d", *item);
}

void printItem_ulong(unsigned long* item)
{
    fprintf(stderr, "%lu", *item);
}

void printItem_long(long* item)
{
    fprintf(stderr, "%ld", *item);
}

void printItem_uint64(unsigned long long* item)
{
    fprintf(stderr, "%llu", *item);
}

void printItem_int64(long long* item)
{
    fprintf(stderr, "%lld", *item);
}

void printItem_float(float* item)
{
    fprintf(stderr, "%f", *item);
}

void printItem_double(double* item)
{
    fprintf(stderr, "%lf", *item);
}

void printItem_ldouble(long double* item)
{
    fprintf(stderr, "%Lf", *item);
}
///@}
