/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	This file defines the following function.
 ******************************************************************************/

//#include	<asm/bitops.h>

#include	"buflocal.h"

s32	Buffer_Number_Free( s32 b )
{
	buf_head_t *	h;
	uchar	*	lp;

	if( b < 0 ) return( b );
	if( Buffer_Context == NULL || *Buffer_Context == NULL ) {
		Trace( 0, "ERROR: Buffer_Initialize Or Buffer_Push_Context has NOT been Called!" );
		return( -1 );
	}
	h = (buf_head_t *)*Buffer_Context;	// Header Pointer
	if( b >= h->count ) {
		Trace( 0, "ERROR: Invalid Buffer Number: %d!, max=%d", b, h->count );
		return( -1 );
	}
	lp = (uchar *)&h[ 1 ];			// Lock Array Pointer
	if( test_and_clear_bit( b&7, (s32 *)&lp[b>>3] ) == 0 ) return( b );
	if( h->used > 0 ) atomic_dec( &h->used );
	return( b );
}

s32	Buffer_Pointer_Free( void * buf )
{
	return( Buffer_Number_Free( Buffer_Number( buf ) ) );
}
