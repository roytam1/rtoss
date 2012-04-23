/*
	vmint.h
	define sized integer types

	Copyright (c) 2006 Ken Kato
*/

#ifndef _VMINT_H_
#define _VMINT_H_

/*
	typedefs for sized integer types (int?_t / uint?_t)
*/
#if HAVE_STDINT_H

/*	the system has stdint.h */
#include <stdint.h>

#elif HAVE_INTTYPES_H

/*	the system does not have stdint.h, but has inttypes.h */
#include <inttypes.h>

#elif defined(_MSC_VER)

/*	Microsoft C Compiler -- use built-in 64 bit integer type */
typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed long			int32_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned long		uint32_t;
typedef unsigned __int64	uint64_t;

#else

/* assume 32 bit environment and "long long" type support	*/
typedef signed char			int8_t;
typedef signed short		int16_t;
typedef signed int			int32_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;

#endif /* sized integer typedefs */

/*
	64 bit integer operation is not supported in some cases
*/
#if defined(__DMC__) && (__LONGLONG == 0)
/* DMC doesn't support 64 bit integers on 16 bit target */
#define NO_INT64
#endif
#if defined(__ACK__)
/* minix ACK C compiler doesn't support 64 bit integers */
#define NO_INT64
#endif

/*
	some utility macro for 64 bit integer handling
*/
#ifdef NO_INT64
/* no compiler native support for 64 bit integer operations */

typedef struct _uint64_t {
	uint32_t lo;
	uint32_t hi;
} uint64_t;

#define U64HI(v64)	((v64).hi)
#define U64LO(v64)	((v64).lo)

#define U64INIT(v64, v32a, v32b) \
	{ U64LO(v64) = v32a; U64HI(v64) = v32b; }

/* printf format text for 64 bit integer */
/* print as two long integers -- <low>:<high> */
/* well, yes it's ugly but is also the easiest */

#define I64D_FMT		"lu:%l"
#define I64X_FMT		"%08lx:%08lx"

/* and some limited 64 bit integer arithmetics	*/

#if defined(__DMC__)
/* 64 bit + 64 bit addition						*/
/* r64 = v64a + v64b							*/
/* r64:  local uint64_t variable (result)		*/
/* v64a: local uint64_t variable (add value 1)	*/
/* v64b: local uint64_t variable (add value 2)	*/
/* r64, v64a and v64b may be the same variable	*/
#define U64ADD64(r64, v64a, v64b) __asm {		\
	__asm mov eax, dword ptr [v64a]				\
	__asm add eax, dword ptr [v64b]				\
	__asm mov dword ptr [r64], eax				\
	__asm mov eax, dword ptr [v64a + 4]			\
	__asm adc eax, dword ptr [v64b + 4]			\
	__asm mov dword ptr [r64 + 4], eax			\
}

/* 64 bit + 32 bit addition						*/
/* r64 = v64 + v32								*/
/* r64: local uint64_t variable (result)		*/
/* v64: local uint64_t variable (add value 1)	*/
/* v32: local uint32_t variable (add value 2)	*/
/* r64, v64 and v32 may be the same variable	*/
#define U64ADD32(r64, v64, v32) __asm {			\
	__asm mov eax, dword ptr [v64]				\
	__asm add eax, dword ptr [v32]				\
	__asm mov dword ptr [r64], eax				\
	__asm mov eax, dword ptr [v64 + 4]			\
	__asm adc eax, 0							\
	__asm mov dword ptr [r64 + 4], eax			\
}

/* 64 bit - 64 bit subtraction					*/
/* r64 = v64a - v64b							*/
/* r64:  local uint64_t variable (result)		*/
/* v64a: local uint64_t variable (subtractee)	*/
/* v64b: local uint64_t variable (subtractor)	*/
/* r64, v64a and v64b may be the same variable	*/
#define U64SUB64(r64, v64a, v64b) __asm {		\
	__asm mov eax, dword ptr [v64a]				\
	__asm sub eax, dword ptr [v64b]				\
	__asm mov dword ptr [r64], eax				\
	__asm mov eax, dword ptr [v64a + 4]			\
	__asm sbb eax, dword ptr [v64b + 4]			\
	__asm mov dword ptr [r64 + 4], eax			\
}

/* 64 bit - 32 bit subtraction					*/
/* r64 = v64 - v32								*/
/* r64: local uint64_t variable (result)		*/
/* v64: local uint64_t variable (subtractee)	*/
/* v32: local uint32_t variable (subtractor)	*/
/* r64, v64 and v32 may be the same variable	*/
#define U64DEC32(r64, v64, v32) __asm {			\
	__asm mov eax, dword ptr [v64]				\
	__asm sub eax, dword ptr [v32]				\
	__asm mov dword ptr [r64], eax				\
	__asm mov eax, dword ptr [v64 + 4]			\
	__asm sbb eax, 0							\
	__asm mov dword ptr [r64 + 4], eax			\
}

/* 32 bit * 32 bit multiplication				*/
/* r64 = v32a * v32b							*/
/* r64:  local uint64_t variable (result)		*/
/* v32a: local uint32_t variable (multiplicand)	*/
/* v32b: local uint32_t variable (multiplier)	*/
/* r64, v32a and v32b may be the same variable	*/
#define U64MUL32(r64, v32a, v32b) __asm {		\
	__asm mov eax, dword ptr [v32a]				\
	__asm mov ecx, dword ptr [v32b]				\
	__asm mul ecx								\
	__asm mov dword ptr [r64], eax				\
	__asm mov dword ptr [r64 + 4], edx			\
}

/* 64 bit / 32 bit division						*/
/* (q64 = v64 / v32), (r32 = v64 % v32)			*/
/* q64: local uint64_t variable (quotient)		*/
/* r32: local uint32_t variable (remainder)		*/
/* v64: local uint64_t variable (divident)		*/
/* v32: local uint32_t variable (divisor)		*/
/* q64 and v64 may be the same variable			*/
/* r32 and v32 may be the same variable			*/
#define U64DIV32(q64, r32, v64, v32) __asm {	\
	__asm mov eax, dword ptr [v64 + 4]			\
	__asm xor edx, edx							\
	__asm mov ecx, dword ptr [v32]				\
	__asm div ecx								\
	__asm mov dword ptr [q64 + 4], eax			\
	__asm mov eax, dword ptr [v64]				\
	__asm div ecx								\
	__asm mov dword ptr [q64], eax				\
	__asm mov dword ptr [r32], edx				\
}

#else	/* !defined(__DMC__) */
/*
	the following 64 bit arithmetic functions are
	provided in a separate asm source for minix
*/
void u64add32(uint64_t *r64, uint64_t v64, uint32_t v32);
void u64add64(uint64_t *r64, uint64_t v64a, uint64_t v64b);
void u64sub32(uint64_t *r64, uint64_t v64, uint32_t v32);
void u64sub64(uint64_t *r64, uint64_t v64a, uint64_t v64b);
void u64mul32(uint64_t *r64, uint32_t v32a, uint32_t v32b);
void u64div32(uint64_t *q64, uint32_t *r32, uint64_t v64, uint32_t v32);

#define U64ADD32(r64, v64, v32)			u64add32(&r64, v64, v32)
#define U64ADD64(r64, v64a, v64b)		u64add64(&r64, v64a, v64b)
#define U64SUB32(r64, v64, v32)			u64sub32(&r64, v64, v32)
#define U64SUB64(r64, v64a, v64b)		u64sub64(&r64, v64a, v64b)
#define U64MUL32(r64, v32a, v32b)		u64mul32(&r64, v32a, v32b)
#define U64DIV32(q64, r32, v64, v32)	u64div32(&q64, &r32, v64, v32)

#endif	/* !defined(__DMC__) */

#else /* !NO_UINT64 */
/*
	64 bit operations are natively supported by the compiler
 */
#define U64HI(v64)						((uint32_t)(v64  >> 32))
#define U64LO(v64)						((uint32_t)(v64))
#define U64INIT(r64, c32a, c32b)		(r64 = ((uint64_t)c32b << 32) | (uint64_t)c32a)
#define U64ADD32(r64, v64, v32)			(r64 = (v64 + v32))
#define U64ADD64(r64, v64a, v64b)		(r64 = (v64a + v64b))
#define U64SUB32(r64, v64, v32)			(r64 = (v64 - v32))
#define U64SUB64(r64, v64a, v64b)		(r64 = (v64a - v64b))
#define U64MUL32(r64, v32a, v32b)		(r64 = ((uint64_t)v32a * (uint64_t)v32b))
#define U64DIV32(q64, r32, v64, v32)	(r32 = (uint32_t)(v64 % v32), q64 = (v64 / v32))
/* !!! r32 and v32 MUST NOT be the same variable !!! */

/* printf format text for 64 bit integer */
#if defined(_MSC_VER)
#define I64D_FMT		"I64"
#define I64X_FMT		"%016I64x"
#else
#define I64D_FMT		"ll"
#define I64X_FMT		"%016llx"
#endif

#endif /* !NO_UINT64 */

#endif /* _VMINT_H_ */
