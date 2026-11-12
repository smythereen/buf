/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	This file defines the following function.
 ******************************************************************************/

#include	"buflocal.h"

s32	Buffer_Number( void * buf )
{
	buf_head_t *	h;
	uchar	*	bp;
	u32	i;

	if( Buffer_Context == NULL || *Buffer_Context == NULL ) {
		Trace( 0, "ERROR: Buffer_Initialize Or Buffer_Push_Context has NOT been Called!" );
		return( -1 );
	}
	h = (buf_head_t *)*Buffer_Context;
	if( h->offset < sizeof( buf_head_t ) ) {
		Trace( 0, "ERROR: Buffer_Initialize has NOT been Called!" );
		return( -1 );
	}
	bp = (uchar *)h;
	bp += h->offset;
	if( buf < (void *)bp ) {
		Trace(	0,
			"ERROR: Invalid Buffer Pointer 0x%08X! Data Start=0x%08X",
			buf,
			bp
		);
		return( -1 );
	}
	i = (u32)( buf - (void *)bp );
	i /= h->size;
	if( i >= h->count ) {
		Trace(	0,
			"ERROR: Invalid Buffer Number %u! Count=%u",
			i,
			h->count
		);
		return( -1 );
	}
	return( (s32)i );
}
