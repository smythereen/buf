/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	This file defines the following function.
 *		Main( );
 ******************************************************************************/

//#include	<asm/bitops.h>

#include	"buflocal.h"

s32	Buffer_Number_Allocate( void )
{
	buf_head_t *	h;
	uchar	*	lp;
	s32		i;
	s32		j;

	if( Buffer_Context == NULL || *Buffer_Context == NULL ) {
		Trace( 0, "ERROR: Buffer_Initialize Or Buffer_Push_Context has NOT been Called!" );
		return( -1 );
	}
	h = (buf_head_t *)*Buffer_Context;	// Header Pointer
	lp = (uchar *)&h[ 1 ];			// Lock Array Pointer
	for( i = h->next, j = 0; test_and_set_bit( i&7, (s32 *)&lp[i>>3] ) != 0; ) {
		if( ( i += 1 ) >= h->count ) i = 0;
		if( ( j += 1 ) <  h->count ) continue;
		Trace( 0, "No Free Buffer To Allocate! count=%d", h->count );
		return( -1 );
	}
	h->next = i + 1;
	if( h->used < h->count ) atomic_inc( &h->used );
	return( i );
}

void *	Buffer_Pointer_Allocate( void )
{
	return( Buffer_Pointer( Buffer_Number_Allocate( ) ) );
}
