/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	This file defines the following function.
 ******************************************************************************/

#include	"buflocal.h"

void *	Buffer_Pointer( s32 b )
{
	buf_head_t *	h;
	uchar	*	bp;

	if( b < 0 ) return( NULL );
	if( Buffer_Context == NULL || *Buffer_Context == NULL ) {
		Trace( 0, "ERROR: Buffer_Initialize Or Buffer_Push_Context has NOT been Called!" );
		return( NULL );
	}
	h = (buf_head_t *)*Buffer_Context;
	if( h->offset < sizeof( buf_head_t ) || h->size == 0 ) {
		Trace( 0, "ERROR: Buffer_Initialize has NOT been Called!" );
		return( NULL );
	}
	bp = (uchar *)h;
	bp += h->offset;
	bp += ( h->size * b );
	return( bp );
}
