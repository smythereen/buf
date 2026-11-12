/* SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later */
/* Copyright (c) 2007-2026 John R. Smythe Jr. */
/*******************************************************************************
 * 
 *	This file defines the following function.
 *		Main( );
 ******************************************************************************/

#include	"buflocal.h"

static	void	*	Buffer_Stack[ 10 ];
void	**	Buffer_Context;

int	Buffer_Push_Context( void * buf )
{
	if( Buffer_Context == NULL || Buffer_Stack[ 0 ] == NULL ) {
		Buffer_Context = &Buffer_Stack[ 0 ];
		*Buffer_Context = buf;
		return( 0 );
	}
	if( Buffer_Context >= &Buffer_Stack[ 9 ] ) {
		return( -1 );				// Failure
	}
	*++Buffer_Context = buf;
	return( 1 );
}

int	Buffer_Pop_Context( void )
{
	if( Buffer_Context == NULL ) {
		Buffer_Context = &Buffer_Stack[ 0 ];
		*Buffer_Context = NULL;
		return( 0 );
	}
	if( Buffer_Context == &Buffer_Stack[ 0 ] ) {
		*Buffer_Context = NULL;
		return( 0 );
	}
	if( Buffer_Context > &Buffer_Stack[ 0 ] ) {
		*Buffer_Context-- = NULL;
		return( 1 );
	}
	return( -1 );
}

void	Buffer_Initialize( void * buf, u32 count, u32 size, u32 fill )
{
	int		pushed;
	buf_head_t *	hdr;

	Buffer_Push_Context( buf );
	hdr = (buf_head_t *)*Buffer_Context;
	hdr->count	= count;
	hdr->size	= size;
	hdr->lock_bytes	= ((count + 7) >> 3);
	hdr->fill_bytes	= fill;
	hdr->offset	= sizeof( buf_head_t ) + hdr->lock_bytes + hdr->fill_bytes;
	hdr->used	= 0;
	hdr->next	= 0;
	memset( &hdr[ 1 ], 0, hdr->lock_bytes );
}
