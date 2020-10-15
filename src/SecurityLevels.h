#pragma once
/// @name Security level masks
///@{
/** No security */
#define SL_NOTHING 0
/** Check for errors with <CODE>stackCheck()</CODE> and dump the stack to <CODE>stderr</CODE>, if any found */
#define SL_DUMP 1
/** Add canaries around the struct and data array */
#define SL_CANARY 2
/** Compute hashes of struct and data array each time the stack changes */
#define SL_HASHING 4
/** All of the above */
#define SL_ALL (SL_DUMP | SL_CANARY | SL_HASHING)
///@}