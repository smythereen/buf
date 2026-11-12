/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	BUFfer Routines Private Header
 *
 *	This file defines local structures, variables, and routines
 *	needed for interface to BUF.
 ******************************************************************************/
#ifndef	BUFLOCAL_H
#define	BUFLOCAL_H

#if	defined( __cplusplus )
extern	"C"	{
#endif

#include	<common.h>
#include	<buf.h>

#define	BUF_CLIENT_SIGNAL	45
#define	BUF_OWNER_SIGNAL	44

#define LOCK "lock ; "
#define ADDR (*(volatile s32 *) addr)

/*
 *	Private Structures
 */

/*
 *	Private Data
 */

extern	void    **      Buffer_Context;

/*
 *	Private	Functions
 */

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It may be reordered on other architectures than x86.
 * It also implies a memory barrier.
 */
static inline int test_and_set_bit( int nr, volatile s32 * addr )
{
        int oldbit;

        __asm__ __volatile__( LOCK
                "btsl %2,%1\n\tsbbl %0,%0"
                :"=r" (oldbit),"=m" (ADDR)
                :"Ir" (nr) : "memory");
        return oldbit;
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It can be reorderdered on other architectures other than x86.
 * It also implies a memory barrier.
 */
static inline int test_and_clear_bit( int nr, volatile s32 * addr )
{
        int oldbit;

        __asm__ __volatile__( LOCK
                "btrl %2,%1\n\tsbbl %0,%0"
                :"=r" (oldbit),"=m" (ADDR)
                :"Ir" (nr) : "memory");
        return oldbit;
}

/**
 * atomic_inc - increment atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically increments @v by 1.
 */
static __inline__ void atomic_inc( volatile s32 * addr )
{
        __asm__ __volatile__(
                LOCK "incl %0"
                :"=m" (ADDR)
                :"m" (ADDR));
}

/**
 * atomic_dec - decrement atomic variable
 * @v: pointer of type atomic_t
 *
 * Atomically decrements @v by 1.
 */
static __inline__ void atomic_dec( volatile s32 * addr )
{
        __asm__ __volatile__(
                LOCK "decl %0"
                :"=m" (ADDR)
                :"m" (ADDR));
}



#if	defined( __cplusplus )
}
#endif

#endif
