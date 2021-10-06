#pragma once
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int int16;
typedef unsigned short int uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int int64;
typedef unsigned long long int uint64;
#ifdef X32
typedef uint32 uintptr;
#else
typedef uint64 uintptr;
#endif

typedef void* voidptr;
typedef uint8 byte;